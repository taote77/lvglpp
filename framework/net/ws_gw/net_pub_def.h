#ifndef NET_PUB_DEF_H
#define NET_PUB_DEF_H

#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

// Lightweight logging macros for ws_gw (no Boost.Log dependency).
#define PX_LOG_DEBUG() std::cerr << "[WSGW][DEBUG] "
#define PX_LOG_INFO() std::cerr << "[WSGW][INFO] "
#define PX_LOG_WARN() std::cerr << "[WSGW][WARN] "
#define PX_LOG_ERROR() std::cerr << "[WSGW][ERROR] "

#define PX_DEBUG PX_LOG_DEBUG
#define PX_INFO PX_LOG_INFO
#define PX_WARN PX_LOG_WARN
#define PX_ERROR PX_LOG_ERROR

#endif // NET_PUB_DEF_H
