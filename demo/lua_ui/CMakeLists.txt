project(lua_ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua-5.2)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    main.cc
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sdk/core
    ${CMAKE_SOURCE_DIR}/sdk/wrappers/lua
    ${CMAKE_SOURCE_DIR}/sdk/lvgl
    ${CMAKE_SOURCE_DIR}/demo/clua/include
    ${LUA_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp_lua
    lvglpp
    lvgl
    ${LUA_LIBRARIES}
)

if (TARGET SDL2::SDL2)
    target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
endif ()

# Copy Lua scripts to build directory
file(GLOB LUA_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/*.lua)
foreach(SCRIPT ${LUA_SCRIPTS})
    get_filename_component(SCRIPT_NAME ${SCRIPT} NAME)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SCRIPT} ${CMAKE_CURRENT_BINARY_DIR}/../../target/bin/${SCRIPT_NAME}
        COMMENT "Copying Lua script ${SCRIPT_NAME} to build directory")
endforeach()
