
#include "tcp_listener.h"
#include "boost/system/detail/error_code.hpp"
#include "msg_dispatcher.h"
#include "net_pub_def.h"
#include "ws_session.h"
#include <boost/log/trivial.hpp>

TcpListener::TcpListener(WsGwContext *cxt, net::io_context &ioc, const char *ip, uint16_t port) : m_cxt(cxt), m_ioc(ioc), m_acceptor(ioc), m_socket(net::make_strand(m_ioc))
{
    boost::system::error_code ec;
    auto                      adr = boost::asio::ip::make_address(ip, ec);
    if (ec)
    {
        PX_WARN() << __LINE__ << ec.message();
        return;
    }
    tcp::endpoint endpoint{adr, port};
    m_acceptor.open(endpoint.protocol(), ec);
    if (ec)
    {
        PX_WARN() << __LINE__ << ec.message();
        return;
    }

    // Allow address reuse
    m_acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
    {
        PX_WARN() << "fail to set reuse properity" << ec.message();
        return;
    }

    // Bind to the server address
    m_acceptor.bind(endpoint, ec);
    if (ec)
    {
        PX_WARN() << __LINE__ << ec.message();
        return;
    }

    // Start listening for connections
    m_acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec)
    {
        PX_WARN() << __LINE__ << ec.message();
        return;
    }
}

#include <boost/asio/yield.hpp>
void TcpListener::loop(boost::system::error_code ec)
{
    reenter(this) for (;;)
    {
        yield m_acceptor.async_accept(m_socket, [this](boost::system::error_code ec) { this->loop(ec); });
        if (ec)
        {
            PX_WARN() << __LINE__ << ec.message();
            if (ec == net::error::no_descriptors)
            {
                continue;
            } else
            {
                yield break;
            }
        } else
        {
            std::shared_ptr<WsSession> session(new WsSession(m_cxt, m_socket));
            session->run();
        }
        m_socket = tcp::socket(net::make_strand(m_ioc));
    }
}
#include <boost/asio/unyield.hpp>
