#ifndef _SESSION_H
#define _SESSION_H

// #include "ws_gw/ws_gw.h"

#include <boost/json/object.hpp>
struct WsGwContext;

class WebsocketSession
{
private:
    WsGwContext *m_cxt = nullptr;

public:
    static WebsocketSession &Instance();

    WebsocketSession(/* args */);

    ~WebsocketSession();

    void Post(boost::json::object &data);

    void Init();
};

#endif // _SESSION_H