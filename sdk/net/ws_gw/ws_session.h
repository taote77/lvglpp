#ifndef NET_WS_SESSION_H__
#define NET_WS_SESSION_H__

#include "boost/beast.hpp"
#include "boost/json.hpp"
#include "boost/json/object.hpp"
#include "boost/json/value.hpp"
#include "boost/system/detail/error_code.hpp"
#include <atomic>
#include <cstddef>
#include <list>
#include <memory>

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

struct WsGwContext;

class WsSession : public std::enable_shared_from_this<WsSession>
{
public:
    explicit WsSession(WsGwContext *cxt, tcp::socket &socket);
    std::string get_id();
    void        send_msg(boost::json::object &msg);

    void run();

private:
    void loop_read(boost::system::error_code ec = {}, size_t n = 0);

    void loop_write(boost::system::error_code ec = {}, size_t n = 0);

private:
    bool process_req();

    void do_close();

private:
    WsGwContext                         *m_cxt;
    websocket::stream<beast::tcp_stream> m_ws;

    beast::flat_buffer m_read_buffer;
    std::string        m_id;
    net::coroutine     m_read_co;
    bool               m_check_req = true;
    std::atomic_bool   m_accept;
    // std::string m_response_buffer;

    net::coroutine          m_write_co;
    std::list<std::string>  m_write_msgs;
    websocket::request_type m_req;
};

#endif // NET_WS_SESSION_H__
