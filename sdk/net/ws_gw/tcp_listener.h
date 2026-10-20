#ifndef HG_TCP_LISTENER_H__
#define HG_TCP_LISTENER_H__

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

struct WsGwContext;

// Creates a listening socket and dispatches accepted connections to a small
// worker pool.  Each worker owns its connection until it is closed.
class TcpListener
{
public:
    TcpListener(WsGwContext *cxt, std::string bindIp, uint16_t port, uint16_t threadNum);
    ~TcpListener();

    TcpListener(const TcpListener &)            = delete;
    TcpListener &operator=(const TcpListener &) = delete;

    bool start();
    void stop();

private:
    int  createListenSocket();
    void acceptLoop();
    void workerLoop();

    WsGwContext *m_cxt;
    std::string  m_bindIp;
    uint16_t     m_port;
    uint16_t     m_threadNum;

    int              m_listenFd = -1;
    std::atomic_bool m_running{false};

    std::thread              m_acceptThread;
    std::vector<std::thread> m_workers;

    std::mutex                              m_queueMutex;
    std::condition_variable                 m_queueCv;
    std::deque<std::pair<int, std::string>> m_pending;
    std::set<int>                           m_activeFds;
};

#endif // HG_TCP_LISTENER_H__
