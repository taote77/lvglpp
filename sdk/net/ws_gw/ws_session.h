#ifndef NET_WS_SESSION_H__
#define NET_WS_SESSION_H__

#include "json.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

struct WsGwContext;

// One WebSocket connection.  run() blocks until the connection is closed and
// is therefore executed by a worker thread of TcpListener.
class WsSession : public std::enable_shared_from_this<WsSession>
{
public:
    WsSession(WsGwContext *cxt, int fd, std::string remoteIp);
    ~WsSession();

    WsSession(const WsSession &)            = delete;
    WsSession &operator=(const WsSession &) = delete;

    void run();

    const std::string &get_id() const { return m_id; }

    // Thread-safe: can be called from any thread.
    void send_msg(const wsgw::Json &msg);

private:
    bool handshake();
    bool read_message(std::string &payload);
    bool read_exact(void *data, std::size_t size);

    bool send_frame(uint8_t opcode, const void *data, std::size_t size);
    bool send_frame_locked(uint8_t opcode, const void *data, std::size_t size);
    bool write_all(const void *data, std::size_t size);

    void close_connection();
    void send_close(uint16_t code);
    void send_http_error(int statusCode, const char *reason, const char *extraHeaders = "");

    WsGwContext *m_cxt;
    int          m_fd;
    std::string  m_remoteIp;
    std::string  m_id;

    // Bytes received together with the HTTP handshake.
    std::string m_pendingData;

    std::mutex       m_writeMutex;
    std::atomic_bool m_closed{false};
};

#endif // NET_WS_SESSION_H__
