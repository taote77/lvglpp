#include "session.h"

#include "net/ws_gw/net_pub_def.h"
#include "net/ws_gw/ws_gw.h"
#include <boost/json/object.hpp>

WebsocketSession &WebsocketSession::Instance()
{
    static WebsocketSession instance;
    return instance;
}

WebsocketSession::WebsocketSession(/* args */)
{}

WebsocketSession::~WebsocketSession()
{}

void WebsocketSession::Post(boost::json::object &data)
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
    setting.callback = [this](const std::string &id, boost::json::object &req, boost::json::object &res) {
        (void)(id);

        PX_ERROR() << req["topic"];

        // auto reply = Request(proto_px::GW_WS_REQUEST, req, 30 * 1000);
        // if (reply.IsValid())
        // {
        //     res = reply.Data<boost::json::object>();
        // } else
        // {
        //     PX_INFO() << reply.Error().what();
        // }
    };

    m_cxt = ws_gw_start(setting);
    PX_ERROR() << "gateway init";
}