# QGL PDK - LVGL C++ 框架 CMake 配置文件

# 设置 QGL 库的源文件
set(QGL_CORE_SOURCES
    core/Types.cpp
    core/Variant.cpp
    core/Object.cpp
)

set(QGL_EVENTS_SOURCES
    events/Event.cpp
    events/EventFactory.cpp
    events/EventBus.cpp
    events/MouseEvent.cpp
    events/KeyEvent.cpp
    # events/LvglAdapter.cpp - 暂时禁用以解决编译问题
)

set(QGL_SIGNALS_SOURCES
    signals/Signal.cpp
)

set(QGL_WIDGETS_SOURCES
    widgets/Widget.cpp
    widgets/Label.cpp
)

set(QGL_LAYOUTS_SOURCES
    layouts/Layout.cpp
    layouts/BoxLayout.cpp
)

set(QGL_PROPERTIES_SOURCES
    properties/Property.cpp
    properties/StyleSheet.cpp
)

set(QGL_TIMER_SOURCES
    timer/Timer.cpp
)

set(QGL_SIGNALS_SOURCES)

# QGL 主文件
set(QGL_MAIN_SOURCES
    QGL.cpp
)

# 合并所有源文件
set(QGL_SOURCES
    ${QGL_CORE_SOURCES}
    ${QGL_EVENTS_SOURCES}
    ${QGL_SIGNALS_SOURCES}
    ${QGL_WIDGETS_SOURCES}
    ${QGL_LAYOUTS_SOURCES}
    ${QGL_PROPERTIES_SOURCES}
    ${QGL_TIMER_SOURCES}
    ${QGL_SIGNALS_SOURCES}
    ${QGL_MAIN_SOURCES}
)

# 创建 QGL 静态库
add_library(qgl_framework STATIC ${QGL_SOURCES})

# 设置包含目录
target_include_directories(qgl_framework PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../sdk/lvgl
)

# 链接 LVGL 库
target_link_libraries(qgl_framework PUBLIC lvgl)

# 设置 C++ 标准
target_compile_features(qgl_framework PUBLIC cxx_std_17)

# 添加编译定义
target_compile_definitions(qgl_framework PUBLIC
    QGL_EVENTS_ENABLED=1
    QGL_LVGL_ADAPTER_ENABLED=1
)

# 启用线程支持
find_package(Threads REQUIRED)
target_link_libraries(qgl_framework PUBLIC Threads::Threads)

# # 创建 QGL 演示程序
# add_executable(qgl_demo qgl_demo.cpp)
# target_link_libraries(qgl_demo qgl_framework)
# target_include_directories(qgl_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# # 创建 QGL 测试程序
# add_executable(qgl_tests test/QGLTests.cpp)
# target_link_libraries(qgl_tests qgl_framework)
# target_include_directories(qgl_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# 导出头文件
set(QGL_HEADERS
    core/Types.h
    core/Variant.h
    core/Object.h
    events/Event.h
    events/EventFactory.h
    events/EventBus.h
    events/MouseEvent.h
    events/KeyEvent.h
    events/FocusEvent.h
    events/PageEvents.h
    events/LvglAdapter.h
    signals/Signal.h
    widgets/Widget.h
    widgets/Label.h
    layouts/Layout.h
    layouts/BoxLayout.h
    timer/Timer.h
    signals/Signal.h
    properties/Property.h
    properties/StyleSheet.h
    QGL.h
)

# 安装头文件（可选）
# install(FILES ${QGL_HEADERS} DESTINATION include/qgl)