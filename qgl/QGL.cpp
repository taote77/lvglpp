#include "QGL.h"

namespace QGL {

void initialize() {
    // QGL 框架初始化
    // 在这里可以进行 LVGL 的初始化和配置
}

void cleanup() {
    // QGL 框架清理
    // 在这里可以进行资源清理
}

String getVersion() {
    return FrameworkInfo::version;
}

String getVersionString() {
    return String(FrameworkInfo::name) + " " + FrameworkInfo::version + 
           " (based on " + FrameworkInfo::basedOn + ")";
}

} // namespace QGL