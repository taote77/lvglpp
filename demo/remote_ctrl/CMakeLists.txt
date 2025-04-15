project(remote_ctrl)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_definitions(-DUSED_FRAMEBUFF)
    #add_definitions(-DUSED_WAYLAND)
else ()
    find_package(SDL2 REQUIRED SDL2)
    add_definitions(-DUSED_SDL)
    include_directories(${SDL2_INCLUDE_DIRS})
endif ()


include(CheckCXXCompilerFlag)


set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PRIVATE
    main.cc
    RemoteClient.h
    RemoteClient.cc
    session.h
    session.cc
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    lvglpp
    sqlite3 ${SDL2_LIBRARIES} ${Boost_LIBRARIES}
    lvgl
)


include_directories(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/
    ${CMAKE_CURRENT_LIST_DIR}/../lvgl/src/
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ./    
)

SET(ASSET_SOURCE_DIR ${PROJECT_SOURCE_DIR}/assets)

set(COPY_ASSET_RES 1)

if (COPY_ASSET_RES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSET_SOURCE_DIR} ${EXECUTABLE_OUTPUT_PATH}/assets)
endif ()


add_executable(server  server.cc)