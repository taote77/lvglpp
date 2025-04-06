#include "ws_session.h"
#include "boost/beast/http/status.hpp"
#include "boost/beast/websocket/rfc6455.hpp"
#include "boost/json/object.hpp"
#include "boost/json/parse.hpp"
#include "boost/json/serialize.hpp"
#include "boost/json/value.hpp"
#include "boost/utility/string_view_fwd.hpp"
#include "msg_dispatcher.h"
#include <boost/asio/coroutine.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/log/trivial.hpp>

namespace http = boost::beast::http;

WsSession::WsSession(WsGwContext *cxt, tcp::socket &socket) : m_cxt(cxt), m_ws(std::move(socket))
{
    m_accept = false;
}

void WsSession::run()
{
    net::dispatch(m_ws.get_executor(), beast::bind_front_handler(&WsSession::loop_read, shared_from_this(), beast::error_code{}, 0));

    net::dispatch(m_ws.get_executor(), beast::bind_front_handler(&WsSession::loop_write, shared_from_this(), beast::error_code{}, 0));
}

std::string WsSession::get_id()
{
    if (!m_accept)
    {
        return "";
    }
    return m_id;
}

void WsSession::send_msg(boost::json::object &msg)
{
    auto self(shared_from_this());
    net::post(m_ws.get_executor(), [msg = std::move(msg), self, this]() {
        bool running = !m_write_msgs.empty();
        m_write_msgs.push_back(boost::json::serialize(msg));
        if (!running)
        {
            loop_write();
        }
    });
}

bool WsSession::process_req()
{
    boost::system::error_code ec;
    boost::json::value        req = boost::json::parse(boost::string_view(static_cast<const char *>(m_read_buffer.cdata().data()), m_read_buffer.size()), ec);
    m_read_buffer.consume(m_read_buffer.size());
    if (ec)
    {
        PX_WARN() << ec.message();
        return false;
    }
    boost::json::object *req_obj = req.if_object();
    if (!req_obj)
    {
        PX_WARN() << "request not object";
        return false;
    }

    // 处理收到的json消息
    boost::json::object res_obj;
    if (m_cxt->callback)
    {
        m_cxt->callback(m_id, *req_obj, res_obj);
    }

    // 无需响应
    if (res_obj.empty())
    {
        return true;
    }

    bool running = !m_write_msgs.empty();
    // 序列化到m_response_buffer
    m_write_msgs.push_back(boost::json::serialize(res_obj));
    if (!running)
    {
        loop_write();
    }
    return true;
}

void WsSession::do_close()
{
    auto self(shared_from_this());
    m_cxt->dispatcher->remove_session(self);
}

#include <boost/asio/yield.hpp>
void WsSession::loop_read(boost::system::error_code ec, size_t n)
{
    auto self(shared_from_this());
    reenter(m_read_co)
    {
        yield http::async_read(m_ws.next_layer(), m_read_buffer, m_req, std::bind(&WsSession::loop_read, self, std::placeholders::_1, std::placeholders::_2));
        if (ec)
        {
            PX_WARN() << ec.message();
            return;
        }
        PX_INFO() << m_req;
        if (!websocket::is_upgrade(m_req))
        {
            PX_WARN() << "is not upgrade req: " << m_req;
            return;
        }
        {
            auto ep = m_ws.next_layer().socket().remote_endpoint(ec);
            if (ec)
            {
                PX_WARN() << ec.message();
                return;
            }
            m_check_req = m_cxt->dispatcher->check_req(ep, m_req, m_id);
        }

        m_ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
        m_ws.set_option(websocket::stream_base::decorator([check_req = this->m_check_req](websocket::response_type &res) {
            if (!check_req)
            {
                res.result(http::status::unauthorized);
            }
        }));
        m_ws.binary(true);
        yield m_ws.async_accept(m_req, std::bind(&WsSession::loop_read, self, std::placeholders::_1, 0));
        if (ec)
        {
            PX_WARN() << ec.message();
            return;
        }

        if (!m_check_req)
        {
            PX_WARN() << "check failed";
            do_close();
            return;
        }
        m_cxt->dispatcher->add_session(self);

        m_accept = true;
        for (;;)
        {
            yield m_ws.async_read(m_read_buffer, std::bind(&WsSession::loop_read, self, std::placeholders::_1, std::placeholders::_2));
            if (ec)
            {
                PX_WARN() << ec.message();
                do_close();
                return;
            }

            if (!process_req())
            {
                continue;
            }
        }
    }
}

void WsSession::loop_write(boost::system::error_code ec, size_t n)
{
    auto self(shared_from_this());
    reenter(m_write_co)
    {
        for (;;)
        {
            if (!m_write_msgs.empty())
            {
                yield m_ws.async_write(net::buffer(m_write_msgs.front()), std::bind(&WsSession::loop_write, self, std::placeholders::_1, std::placeholders::_2));
                m_write_msgs.pop_front();
                if (ec)
                {
                    BOOST_LOG_TRIVIAL(error) << ec.message();
                    return;
                }
            } else
            {
                yield;
            }
        }
    }
}
#include <boost/asio/unyield.hpp>
