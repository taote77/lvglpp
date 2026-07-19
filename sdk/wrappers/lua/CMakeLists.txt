project(lvglpp_lua)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua-5.4)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE
    LvglLuaBind.cc
    WidgetLuaBind.cc
    LuaUtils.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sdk/core
    ${CMAKE_SOURCE_DIR}/sdk/lvgl
    ${CMAKE_SOURCE_DIR}/demo/clua/include
    ${LUA_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    lvglpp
    lvgl
    ${LUA_LIBRARIES}
)

if (TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
endif ()
