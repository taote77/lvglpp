#ifndef HG_MSG_DISPATCHER_H__
#define HG_MSG_DISPATCHER_H__

#include "ws_gw.h"

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>

class WsSession;
class TcpListener;

class MsgDispatcher
{
public:
    void add_session(const std::shared_ptr<WsSession> &session);
    void remove_session(const std::shared_ptr<WsSession> &session);

    WsGwError send_notify(const std::string &id, wsgw::Json &value);

private:
    std::mutex                            m_mutex;
    std::list<std::shared_ptr<WsSession>> m_sessions;
};

struct WsGwContext {
    WsGwContext();
    ~WsGwContext();

    std::unique_ptr<MsgDispatcher> dispatcher;
    std::unique_ptr<TcpListener>   listener;
    std::function<void(const std::string &id, wsgw::Json &req, wsgw::Json &res)> callback;
};

#endif // HG_MSG_DISPATCHER_H__
