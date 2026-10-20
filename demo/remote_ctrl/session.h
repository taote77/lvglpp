#ifndef _SESSION_H
#define _SESSION_H

#include "net/ws_gw/ws_gw.h"

#include <functional>

class WebsocketSession
{
private:
    WsGwContext *m_cxt = nullptr;

public:
    static WebsocketSession &Instance();

    using MsgHandler = std::function<void(wsgw::Json &req, wsgw::Json &res)>;

    WebsocketSession(/* args */);

    ~WebsocketSession();

    void Post(wsgw::Json &data);

    void Init();

    void OnMsgHandler(MsgHandler handler);

private:
    void OnHandle(wsgw::Json &req, wsgw::Json &res);

    MsgHandler _msg_handler;
};

#endif // _SESSION_H