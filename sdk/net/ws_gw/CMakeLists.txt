cmake_minimum_required(VERSION 3.16)

project(ws_gw)

find_package(Boost  REQUIRED COMPONENTS log log_setup filesystem thread date_time json)

add_library(ws_gw SHARED
   msg_dispatcher.cpp
   tcp_listener.cpp
   ws_gw.cpp
   ws_session.cpp
)

target_include_directories(ws_gw PRIVATE ${Boost_INCLUDE_DIR})

message("arch is ${CMAKE_SYSTEM_PROCESSOR}")

target_compile_features(ws_gw PRIVATE cxx_std_14)

target_compile_definitions(ws_gw PUBLIC BOOST_ALL_DYN_LINK)

target_link_libraries(ws_gw PUBLIC ${Boost_LIBRARIES})

