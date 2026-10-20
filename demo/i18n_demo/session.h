#ifndef _SESSION_H
#define _SESSION_H

#include "net/ws_gw/ws_gw.h"

class WebsocketSession
{
private:
    WsGwContext *m_cxt = nullptr;

public:
    static WebsocketSession &Instance();

    WebsocketSession(/* args */);

    ~WebsocketSession();

    void Post(wsgw::Json &data);

    void Init();
};

#endif // _SESSION_H