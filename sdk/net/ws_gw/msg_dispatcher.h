
#ifndef HG_MSG_DISPATCHER_H__
#define HG_MSG_DISPATCHER_H__

#include "boost/beast.hpp"
#include "boost/json/value.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"
#include "net_pub_def.h"
#include "tcp_listener.h"
#include "ws_gw.h"
#include "ws_session.h"
#include <boost/json.hpp>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <stdint.h>

namespace beast     = boost::beast;
namespace websocket = beast::websocket;
namespace net       = boost::asio;
using tcp           = boost::asio::ip::tcp;

class WsSession;

class MsgDispatcher
{
public:
    MsgDispatcher(net::io_context &ioc);
    void add_session(std::shared_ptr<WsSession> &ses);
    void remove_session(std::shared_ptr<WsSession> &ses);

    WsGwError send_notify(const std::string &id, boost::json::object &v);
    bool      check_req(const tcp::endpoint &remote_ep, const websocket::request_type &req, std::string &id);

private:
    net::io_context                      &m_ioc;
    std::mutex                            m_mutex;
    std::list<std::shared_ptr<WsSession>> m_sessions;
};

struct WsGwContext {
    WsGwContext();
    ~WsGwContext();

    net::io_context                                                                                ioc;
    std::unique_ptr<MsgDispatcher>                                                                 dispatcher;
    std::unique_ptr<TcpListener>                                                                   listener;
    std::function<void(const std::string &id, boost::json::object &req, boost::json::object &res)> callback;
};

#endif
