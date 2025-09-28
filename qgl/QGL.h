#ifndef QGL_H
#define QGL_H

// QGL PDK - LVGL C++ 框架主头文件
// 版本: 1.0.0
// 基于设计文档实现的现代化 C++ GUI 框架

// 核心系统
#include "core/Types.h"
#include "core/Variant.h"
#include "core/Object.h"

// 事件系统
#include "events/Event.h"
#include "events/EventFactory.h"

// 信号槽系统
#include "signals/Signal.h"

// 控件系统
#include "widgets/Widget.h"

// 布局系统
#include "layouts/Layout.h"
#include "layouts/BoxLayout.h"
#include "layouts/GridLayout.h"
#include "layouts/FlexLayout.h"

// 属性系统
#include "properties/Property.h"
#include "properties/StyleSheet.h"

// 定时器系统
#include "timer/Timer.h"

// 框架信息
namespace QGL {

struct FrameworkInfo {
    static constexpr const char* name = "QGL PDK";
    static constexpr const char* version = "1.0.0";
    static constexpr const char* description = "Qt-like GUI Library Platform Development Kit";
    static constexpr const char* basedOn = "LVGL 9.3+";
    static constexpr const char* cppStandard = "C++17";
};

// 框架初始化函数
void initialize();
void cleanup();

// 版本信息
String getVersion();
String getVersionString();

} // namespace QGL

#endif // QGL_H