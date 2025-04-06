
#ifndef HG_WS_GW_H__
#define HG_WS_GW_H__

#include "boost/json/value.hpp"
#include <boost/json/object.hpp>
#include <cstdint>
#include <functional>
#include <string>
struct WsGwSetting {
    std::string                                                                                    bind_ip    = "0.0.0.0";
    uint16_t                                                                                       port       = 1120;
    uint16_t                                                                                       thread_num = 3;
    std::function<void(const std::string &id, boost::json::object &req, boost::json::object &res)> callback;
};

struct WsGwContext;

enum WsGwError {
    WSGW_OK               = 0,
    WSGW_NOT_FOUND_CLIENT = 1,
    WSGW_NOT_START        = 2,
};

// 初始化, 首先调用
WsGwContext *ws_gw_start(const WsGwSetting &setting);

// 向ws客户端发送消息
WsGwError ws_gw_send_nofity(WsGwContext *cxt, const std::string &id, boost::json::object &msg);

#endif