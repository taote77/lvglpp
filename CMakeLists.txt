
cmake_minimum_required(VERSION 3.16)



if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_definitions(-DUSED_FRAMEBUFF)
    #add_definitions(-DUSED_WAYLAND)
else ()
    find_package(SDL2 REQUIRED SDL2)
    add_definitions(-DUSED_SDL)
    add_definitions(-DLV_USE_SDL)

    include_directories(${SDL2_INCLUDE_DIRS})
endif ()


add_subdirectory(lvglpp)

add_subdirectory(test)