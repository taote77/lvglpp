
#ifndef HG_TCP_LISTENER_H__
#define HG_TCP_LISTENER_H__

#include "boost/asio.hpp"
#include "net_pub_def.h"
#include <memory>
#include <stdint.h>

namespace net = boost::asio;          // from <boost/asio.hpp>
using tcp     = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>
struct WsGwContext;
// new之后生存期一直到进程结束
class TcpListener : public boost::asio::coroutine
{
public:
    TcpListener(WsGwContext *cxt, net::io_context &ioc, const char *ip, uint16_t port);
    void loop(boost::system::error_code ec = {});

private:
    WsGwContext     *m_cxt;
    net::io_context &m_ioc;
    tcp::acceptor    m_acceptor;
    tcp::socket      m_socket;
};

#endif