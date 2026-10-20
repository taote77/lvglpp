#include "session.h"

#include "net/ws_gw/net_pub_def.h"
#include "net/ws_gw/ws_gw.h"

WebsocketSession &WebsocketSession::Instance()
{
    static WebsocketSession instance;
    return instance;
}

WebsocketSession::WebsocketSession(/* args */)
{}

WebsocketSession::~WebsocketSession()
{}

void WebsocketSession::Post(wsgw::Json &data)
{
    WsGwError err = ws_gw_send_nofity(m_cxt, "127.0.0.1", data);
    if (err != WSGW_OK)
    {
        PX_ERROR() << "ws_gw_send_nofity failed:" << err;
    }
}

void WebsocketSession::Init()
{
    int         port = 9998;
    WsGwSetting setting;
    setting.bind_ip  = "127.0.0.1";
    setting.port     = port;
    setting.callback = [this](const std::string &id, wsgw::Json &req, wsgw::Json &res) {
        (void)(id);

        this->OnHandle(req, res);
    };

    m_cxt = ws_gw_start(setting);
    PX_ERROR() << "gateway init";
}

void WebsocketSession::OnMsgHandler(MsgHandler handler)
{
    _msg_handler = handler;
}

void WebsocketSession::OnHandle(wsgw::Json &req, wsgw::Json &res)
{
    _msg_handler(req, res);
}
