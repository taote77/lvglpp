#include "ws_gw.h"
#include "msg_dispatcher.h"
#include "tcp_listener.h"
#include <thread>

//////////////////////////////////////////////////////////////
WsGwContext *ws_gw_start(const WsGwSetting &setting)
{
    WsGwContext *cxt;
    cxt = new WsGwContext();
    cxt->dispatcher.reset(new MsgDispatcher(cxt->ioc));
    cxt->listener.reset(new TcpListener(cxt, cxt->ioc, setting.bind_ip.c_str(), setting.port));
    cxt->callback = setting.callback;

    cxt->listener->loop();
    for (int i = 0; i < setting.thread_num; ++i)
    {
        std::thread([cxt]() { cxt->ioc.run(); }).detach();
    }

    return cxt;
}

WsGwError ws_gw_send_nofity(WsGwContext *cxt, const std::string &id, boost::json::object &msg)
{
    if (!cxt)
    {
        return WSGW_NOT_START;
    }
    return cxt->dispatcher->send_notify(id, msg);
}
