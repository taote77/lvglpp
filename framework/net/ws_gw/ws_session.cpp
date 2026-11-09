#include "ws_session.h"

#include "msg_dispatcher.h"
#include "net_pub_def.h"
#include "ws_util.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <exception>
#include <map>
#include <sstream>

#include <sys/socket.h>
#include <unistd.h>

namespace {

constexpr std::size_t kMaxHandshakeSize = 64 * 1024;
constexpr std::size_t kMaxMessageSize   = 64 * 1024 * 1024;
constexpr std::size_t kReceiveChunk     = 4096;

std::string trim(const std::string &value)
{
    std::size_t begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return {};
    std::size_t end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::string toLower(std::string value)
{
    for (char &c : value)
    {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return value;
}

bool containsToken(const std::string &value, const char *token)
{
    return toLower(value).find(token) != std::string::npos;
}

} // namespace

WsSession::WsSession(WsGwContext *cxt, int fd, std::string remoteIp)
    : m_cxt(cxt), m_fd(fd), m_remoteIp(std::move(remoteIp))
{}

WsSession::~WsSession()
{
    close_connection();
}

void WsSession::run()
{
    if (!handshake())
    {
        close_connection();
        return;
    }

    auto self = shared_from_this();
    m_cxt->dispatcher->add_session(self);
    PX_INFO() << "session " << m_id << " connected from " << m_remoteIp << std::endl;

    while (!m_closed.load())
    {
        std::string payload;
        if (!read_message(payload)) break;

        wsgw::Json request;
        try
        {
            request = wsgw::Json::parse(payload);
        }
        catch (const std::exception &e)
        {
            PX_WARN() << "invalid JSON from " << m_id << ": " << e.what() << std::endl;
            continue;
        }

        if (!request.is_object())
        {
            PX_WARN() << "request from " << m_id << " is not a JSON object" << std::endl;
            continue;
        }

        wsgw::Json response;
        if (m_cxt->callback)
        {
            m_cxt->callback(m_id, request, response);
        }

        if (!response.is_null() && !response.empty())
        {
            send_msg(response);
        }
    }

    m_cxt->dispatcher->remove_session(self);
    PX_INFO() << "session " << m_id << " disconnected" << std::endl;
    close_connection();
}

bool WsSession::handshake()
{
    std::string request;
    char        buffer[kReceiveChunk];
    std::size_t headerEnd = std::string::npos;

    while ((headerEnd = request.find("\r\n\r\n")) == std::string::npos)
    {
        ssize_t received = ::recv(m_fd, buffer, sizeof(buffer), 0);
        if (received < 0)
        {
            if (errno == EINTR) continue;
            return false;
        }
        if (received == 0) return false;

        request.append(buffer, static_cast<std::size_t>(received));
        if (request.size() > kMaxHandshakeSize)
        {
            send_http_error(431, "Request Header Fields Too Large");
            return false;
        }
    }

    m_pendingData.assign(request.data() + headerEnd + 4, request.size() - headerEnd - 4);
    request.resize(headerEnd);

    std::istringstream stream(request);
    std::string        requestLine;
    if (!std::getline(stream, requestLine)) return false;
    requestLine = trim(requestLine);
    if (requestLine.rfind("GET ", 0) != 0)
    {
        send_http_error(400, "Bad Request");
        return false;
    }

    std::map<std::string, std::string> headers;
    std::string                        line;
    while (std::getline(stream, line))
    {
        line = trim(line);
        if (line.empty()) continue;

        std::size_t colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string name  = toLower(trim(line.substr(0, colon)));
        std::string value = trim(line.substr(colon + 1));
        headers[name]     = value;
    }

    auto upgrade = headers.find("upgrade");
    if (upgrade == headers.end() || !containsToken(upgrade->second, "websocket"))
    {
        send_http_error(400, "Bad Request");
        return false;
    }

    auto connection = headers.find("connection");
    if (connection == headers.end() || !containsToken(connection->second, "upgrade"))
    {
        send_http_error(400, "Bad Request");
        return false;
    }

    auto version = headers.find("sec-websocket-version");
    if (version == headers.end() || trim(version->second) != "13")
    {
        send_http_error(426, "Upgrade Required", "Sec-WebSocket-Version: 13\r\n");
        return false;
    }

    auto key = headers.find("sec-websocket-key");
    if (key == headers.end() || key->second.empty())
    {
        send_http_error(400, "Bad Request");
        return false;
    }

    // Local clients are identified as "127.0.0.1"; remote clients must send
    // an "id" header.
    if (m_remoteIp == "127.0.0.1" || m_remoteIp == "::1" || m_remoteIp == "::ffff:127.0.0.1")
    {
        m_id = "127.0.0.1";
    }
    else
    {
        auto id = headers.find("id");
        if (id == headers.end() || id->second.empty())
        {
            send_http_error(401, "Unauthorized");
            return false;
        }
        m_id = id->second;
    }

    const std::string response =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " +
        wsgw::webSocketAcceptKey(key->second) + "\r\n\r\n";

    return write_all(response.data(), response.size());
}

bool WsSession::read_message(std::string &payload)
{
    payload.clear();

    bool    fragmented      = false;
    uint8_t fragmentedOpcode = 0;

    for (;;)
    {
        uint8_t header[2];
        if (!read_exact(header, sizeof(header))) return false;

        const bool    finalFrame = (header[0] & 0x80) != 0;
        const uint8_t opcode     = header[0] & 0x0F;
        const bool    masked     = (header[1] & 0x80) != 0;
        uint64_t      length     = header[1] & 0x7F;

        if ((header[0] & 0x70) != 0)
        {
            send_close(1002);
            return false;
        }

        if (length == 126)
        {
            uint8_t extended[2];
            if (!read_exact(extended, sizeof(extended))) return false;
            length = (static_cast<uint64_t>(extended[0]) << 8) | extended[1];
        }
        else if (length == 127)
        {
            uint8_t extended[8];
            if (!read_exact(extended, sizeof(extended))) return false;
            length = 0;
            for (uint8_t byte : extended)
            {
                length = (length << 8) | byte;
            }
        }

        if (length > kMaxMessageSize)
        {
            send_close(1009);
            return false;
        }

        // Frames from the client must always be masked.
        if (!masked)
        {
            send_close(1002);
            return false;
        }

        uint8_t mask[4];
        if (!read_exact(mask, sizeof(mask))) return false;

        if (opcode >= 0x8)
        {
            // Control frames must not be fragmented and must be <= 125 bytes.
            if (!finalFrame || length > 125)
            {
                send_close(1002);
                return false;
            }

            std::string control(static_cast<std::size_t>(length), '\0');
            if (length > 0 && !read_exact(&control[0], control.size())) return false;
            for (std::size_t i = 0; i < control.size(); ++i)
            {
                control[i] = static_cast<char>(control[i] ^ mask[i % 4]);
            }

            if (opcode == 0x8)
            {
                // Echo the close frame and finish.
                send_frame(0x8, control.data(), control.size());
                return false;
            }
            if (opcode == 0x9)
            {
                if (!send_frame(0xA, control.data(), control.size())) return false;
                continue;
            }
            if (opcode == 0xA)
            {
                continue;
            }

            send_close(1002);
            return false;
        }

        std::string message(static_cast<std::size_t>(length), '\0');
        if (length > 0 && !read_exact(&message[0], message.size())) return false;
        for (std::size_t i = 0; i < message.size(); ++i)
        {
            message[i] = static_cast<char>(message[i] ^ mask[i % 4]);
        }

        if (opcode == 0x0)
        {
            if (!fragmented)
            {
                send_close(1002);
                return false;
            }
            if (payload.size() + message.size() > kMaxMessageSize)
            {
                send_close(1009);
                return false;
            }
            payload += message;
            if (finalFrame)
            {
                (void)fragmentedOpcode;
                return true;
            }
            continue;
        }

        if (opcode == 0x1 || opcode == 0x2)
        {
            if (fragmented)
            {
                send_close(1002);
                return false;
            }
            if (!finalFrame)
            {
                fragmented       = true;
                fragmentedOpcode = opcode;
                payload          = std::move(message);
                continue;
            }
            payload = std::move(message);
            return true;
        }

        send_close(1002);
        return false;
    }
}

bool WsSession::read_exact(void *data, std::size_t size)
{
    auto *out    = static_cast<uint8_t *>(data);
    std::size_t offset = 0;

    if (!m_pendingData.empty())
    {
        std::size_t count = std::min(size, m_pendingData.size());
        std::memcpy(out, m_pendingData.data(), count);
        m_pendingData.erase(0, count);
        offset = count;
    }

    while (offset < size)
    {
        ssize_t received = ::recv(m_fd, out + offset, size - offset, 0);
        if (received < 0)
        {
            if (errno == EINTR) continue;
            return false;
        }
        if (received == 0) return false;
        offset += static_cast<std::size_t>(received);
    }

    return true;
}

bool WsSession::send_frame(uint8_t opcode, const void *data, std::size_t size)
{
    std::lock_guard<std::mutex> lock(m_writeMutex);
    return send_frame_locked(opcode, data, size);
}

bool WsSession::send_frame_locked(uint8_t opcode, const void *data, std::size_t size)
{
    if (m_closed.load()) return false;

    uint8_t     header[10];
    std::size_t headerSize = 0;

    header[0]  = static_cast<uint8_t>(0x80 | opcode);
    if (size < 126)
    {
        header[1]  = static_cast<uint8_t>(size);
        headerSize = 2;
    }
    else if (size <= 0xFFFF)
    {
        header[1]  = 126;
        header[2]  = static_cast<uint8_t>((size >> 8) & 0xFF);
        header[3]  = static_cast<uint8_t>(size & 0xFF);
        headerSize = 4;
    }
    else
    {
        header[1] = 127;
        for (int i = 0; i < 8; ++i)
        {
            header[2 + i] = static_cast<uint8_t>((static_cast<uint64_t>(size) >> (56 - i * 8)) & 0xFF);
        }
        headerSize = 10;
    }

    if (!write_all(header, headerSize)) return false;
    if (size > 0 && !write_all(data, size)) return false;
    return true;
}

bool WsSession::write_all(const void *data, std::size_t size)
{
    const auto *bytes  = static_cast<const uint8_t *>(data);
    std::size_t offset = 0;

    while (offset < size)
    {
        ssize_t sent = ::send(m_fd, bytes + offset, size - offset, MSG_NOSIGNAL);
        if (sent < 0)
        {
            if (errno == EINTR) continue;
            m_closed.store(true);
            ::shutdown(m_fd, SHUT_RDWR);
            return false;
        }
        if (sent == 0)
        {
            m_closed.store(true);
            ::shutdown(m_fd, SHUT_RDWR);
            return false;
        }
        offset += static_cast<std::size_t>(sent);
    }

    return true;
}

void WsSession::send_msg(const wsgw::Json &msg)
{
    std::string payload = msg.serialize();

    std::lock_guard<std::mutex> lock(m_writeMutex);
    send_frame_locked(0x2, payload.data(), payload.size());
}

void WsSession::close_connection()
{
    std::lock_guard<std::mutex> lock(m_writeMutex);
    if (!m_closed.exchange(true))
    {
        ::shutdown(m_fd, SHUT_RDWR);
    }
}

void WsSession::send_close(uint16_t code)
{
    uint8_t payload[2] = {static_cast<uint8_t>((code >> 8) & 0xFF),
                          static_cast<uint8_t>(code & 0xFF)};
    send_frame(0x8, payload, sizeof(payload));
}

void WsSession::send_http_error(int statusCode, const char *reason, const char *extraHeaders)
{
    std::string response = "HTTP/1.1 " + std::to_string(statusCode) + " " + reason + "\r\n";
    response += "Connection: close\r\n";
    if (extraHeaders != nullptr) response += extraHeaders;
    response += "Content-Length: 0\r\n\r\n";

    std::lock_guard<std::mutex> lock(m_writeMutex);
    if (!m_closed.load())
    {
        write_all(response.data(), response.size());
    }
}
