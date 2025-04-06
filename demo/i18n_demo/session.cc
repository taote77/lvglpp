#include "session.h"

#include "ws_gw/ws_gw.h"
#include <boost/json/object.hpp>

WebsocketSession *Instance()
{
    static WebsocketSession instance;
    return &instance;
}

WebsocketSession::WebsocketSession(/* args */)
{}

WebsocketSession::~WebsocketSession()
{}

void WebsocketSession::Post(const boost::json::object &data)
{
    WsGwError err = ws_gw_send_nofity(m_cxt, "127.0.0.1", data);
    if (err != WSGW_OK)
    {
        LOG_ERROR() << "ws_gw_send_nofity failed:" << err;
    }
}

void Init()
{
    WsGwSetting setting;
    setting.bind_ip  = "127.0.0.1";
    setting.port     = port;
    setting.callback = [this](const std::string &id, boost::json::object &req, boost::json::object &res) {
        boost::ignore_unused(id);

        LOG_DEBUG() << req["topic"];

        auto reply = Request(proto_px::GW_WS_REQUEST, req, 30 * 1000);
        if (reply.IsValid())
        {
            res = reply.Data<boost::json::object>();
        } else
        {
            PX_INFO() << reply.Error().what();
        }
    };

    m_cxt = ws_gw_start(setting);
    LOG_DEBUG() << "gateway init";
}