#ifndef _SESSION_H
#define _SESSION_H

// #include "ws_gw/ws_gw.h"

#include <boost/json/object.hpp>
#include <functional>

struct WsGwContext;

class WebsocketSession
{
private:
    WsGwContext *m_cxt = nullptr;

public:
    static WebsocketSession &Instance();

    using MsgHandler = std::function<void(boost::json::object &req, boost::json::object &res)>;

    WebsocketSession(/* args */);

    ~WebsocketSession();

    void Post(boost::json::object &data);

    void Init();

    void OnMsgHandler(MsgHandler handler);

private:
    void OnHandle(boost::json::object &req, boost::json::object &res);

    MsgHandler _msg_handler;
};

#endif // _SESSION_H