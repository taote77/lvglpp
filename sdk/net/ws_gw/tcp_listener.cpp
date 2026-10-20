#include "tcp_listener.h"

#include "msg_dispatcher.h"
#include "net_pub_def.h"
#include "ws_session.h"

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <exception>

#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace {
constexpr std::size_t kMaxPendingConnections = 128;
}

TcpListener::TcpListener(WsGwContext *cxt, std::string bindIp, uint16_t port, uint16_t threadNum)
    : m_cxt(cxt), m_bindIp(std::move(bindIp)), m_port(port), m_threadNum(threadNum)
{}

TcpListener::~TcpListener()
{
    stop();
}

int TcpListener::createListenSocket()
{
    addrinfo hints{};
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    addrinfo *results = nullptr;
    std::string portText = std::to_string(m_port);
    int rc = ::getaddrinfo(m_bindIp.c_str(), portText.c_str(), &hints, &results);
    if (rc != 0)
    {
        PX_ERROR() << "getaddrinfo(" << m_bindIp << ":" << m_port << ") failed: "
                   << ::gai_strerror(rc) << std::endl;
        return -1;
    }

    int listenFd = -1;
    for (addrinfo *entry = results; entry != nullptr; entry = entry->ai_next)
    {
        int fd = ::socket(entry->ai_family, entry->ai_socktype | SOCK_CLOEXEC, entry->ai_protocol);
        if (fd < 0) continue;

        int reuse = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

        if (::bind(fd, entry->ai_addr, entry->ai_addrlen) == 0 && ::listen(fd, SOMAXCONN) == 0)
        {
            listenFd = fd;
            break;
        }

        PX_WARN() << "bind/listen failed: " << std::strerror(errno) << std::endl;
        ::close(fd);
    }

    ::freeaddrinfo(results);

    if (listenFd >= 0)
    {
        PX_INFO() << "listening on " << m_bindIp << ":" << m_port << std::endl;
    }
    return listenFd;
}

bool TcpListener::start()
{
    if (m_running.load()) return true;

    m_listenFd = createListenSocket();
    if (m_listenFd < 0) return false;

    m_running.store(true);
    m_acceptThread = std::thread(&TcpListener::acceptLoop, this);

    uint16_t workerCount = m_threadNum == 0 ? 1 : m_threadNum;
    m_workers.reserve(workerCount);
    for (uint16_t i = 0; i < workerCount; ++i)
    {
        m_workers.emplace_back(&TcpListener::workerLoop, this);
    }

    return true;
}

void TcpListener::stop()
{
    if (!m_running.exchange(false)) return;

    m_queueCv.notify_all();

    // Wake sessions blocked in recv().
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        for (int fd : m_activeFds)
        {
            ::shutdown(fd, SHUT_RDWR);
        }
    }

    if (m_acceptThread.joinable())
    {
        m_acceptThread.join();
    }

    // Close connections that were accepted but never picked up by a worker.
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_pending.empty())
        {
            int fd = m_pending.front().first;
            m_pending.pop_front();
            ::shutdown(fd, SHUT_RDWR);
            ::close(fd);
        }
    }

    for (auto &worker : m_workers)
    {
        if (worker.joinable()) worker.join();
    }
    m_workers.clear();

    if (m_listenFd >= 0)
    {
        ::close(m_listenFd);
        m_listenFd = -1;
    }
}

void TcpListener::acceptLoop()
{
    while (m_running.load())
    {
        pollfd descriptor{};
        descriptor.fd     = m_listenFd;
        descriptor.events = POLLIN;

        int rc = ::poll(&descriptor, 1, 200);
        if (rc < 0)
        {
            if (errno == EINTR) continue;
            if (!m_running.load()) break;
            PX_WARN() << "poll failed: " << std::strerror(errno) << std::endl;
            break;
        }
        if (!m_running.load()) break;
        if (rc == 0) continue;
        if (descriptor.revents & (POLLERR | POLLNVAL | POLLHUP)) break;
        if (!(descriptor.revents & POLLIN)) continue;

        sockaddr_storage address{};
        socklen_t        addressLength = sizeof(address);
        int fd = ::accept(m_listenFd, reinterpret_cast<sockaddr *>(&address), &addressLength);
        if (fd < 0)
        {
            if (errno == EINTR) continue;
            if (!m_running.load()) break;
            PX_WARN() << "accept failed: " << std::strerror(errno) << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        int keepAlive = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive));

        char host[NI_MAXHOST] = {0};
        if (::getnameinfo(reinterpret_cast<sockaddr *>(&address), addressLength,
                          host, sizeof(host), nullptr, 0, NI_NUMERICHOST) != 0)
        {
            std::snprintf(host, sizeof(host), "unknown");
        }

        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            if (!m_running.load() || m_pending.size() >= kMaxPendingConnections)
            {
                ::shutdown(fd, SHUT_RDWR);
                ::close(fd);
                continue;
            }
            m_activeFds.insert(fd);
            m_pending.emplace_back(fd, std::string(host));
        }
        m_queueCv.notify_one();
    }
}

void TcpListener::workerLoop()
{
    for (;;)
    {
        std::pair<int, std::string> connection;

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queueCv.wait(lock, [this] {
                return !m_running.load() || !m_pending.empty();
            });
            if (!m_running.load()) return;

            connection = std::move(m_pending.front());
            m_pending.pop_front();
        }

        try
        {
            auto session = std::make_shared<WsSession>(m_cxt, connection.first, connection.second);
            session->run();
        }
        catch (const std::exception &e)
        {
            PX_WARN() << "session failed: " << e.what() << std::endl;
        }
        catch (...)
        {
            PX_WARN() << "session failed with unknown error" << std::endl;
        }

        {
            std::lock_guard<std::mutex> lock(m_queueMutex);
            m_activeFds.erase(connection.first);
        }
        ::close(connection.first);
    }
}
