#include "ws_gw.h"

#include "msg_dispatcher.h"
#include "net_pub_def.h"
#include "tcp_listener.h"

#include <memory>

WsGwContext *ws_gw_start(const WsGwSetting &setting)
{
    auto *cxt = new WsGwContext();
    cxt->dispatcher = std::make_unique<MsgDispatcher>();
    cxt->callback   = setting.callback;
    cxt->listener   = std::make_unique<TcpListener>(cxt, setting.bind_ip, setting.port, setting.thread_num);

    if (!cxt->listener->start())
    {
        PX_ERROR() << "failed to start WebSocket gateway on " << setting.bind_ip << ":"
                   << setting.port << std::endl;
    }

    return cxt;
}

WsGwError ws_gw_send_nofity(WsGwContext *cxt, const std::string &id, wsgw::Json &msg)
{
    if (!cxt)
    {
        return WSGW_NOT_START;
    }
    return cxt->dispatcher->send_notify(id, msg);
}
