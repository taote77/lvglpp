cmake_minimum_required(VERSION 3.16)

project(ws_gw)

add_library(ws_gw SHARED
    json.cpp
    msg_dispatcher.cpp
    tcp_listener.cpp
    ws_gw.cpp
    ws_session.cpp
    ws_util.cpp
)

# The public API is included as "net/ws_gw/ws_gw.h", so expose the sdk root.
target_include_directories(ws_gw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)

target_compile_features(ws_gw PRIVATE cxx_std_17)

message("arch is ${CMAKE_SYSTEM_PROCESSOR}")

find_package(Threads REQUIRED)
target_link_libraries(ws_gw PUBLIC Threads::Threads)
