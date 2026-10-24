
cmake_minimum_required(VERSION 3.24)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
project(lvgl_wrapper)

include(cmake/build.cmake)
include(cmake/cmrc.cmake)


set(LVGLPP_PLATFORM "AUTO" CACHE STRING "Display backend: AUTO, SDL, FRAMEBUF or DRM")
set_property(CACHE LVGLPP_PLATFORM PROPERTY STRINGS AUTO SDL FRAMEBUF DRM)
string(TOUPPER "${LVGLPP_PLATFORM}" _lvglpp_platform)
if (_lvglpp_platform STREQUAL "AUTO")
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        set(_lvglpp_platform "FRAMEBUF")
    else ()
        set(_lvglpp_platform "SDL")
    endif ()
endif ()
if (NOT _lvglpp_platform MATCHES "^(SDL|FRAMEBUF|DRM)$")
    message(FATAL_ERROR "LVGLPP_PLATFORM must be AUTO, SDL, FRAMEBUF or DRM (got '${LVGLPP_PLATFORM}')")
endif ()
set(LVGLPP_PLATFORM "${_lvglpp_platform}" CACHE STRING "Display backend: AUTO, SDL, FRAMEBUF or DRM" FORCE)

add_compile_definitions(LVGLPP_PLATFORM_${LVGLPP_PLATFORM}=1)
message(STATUS "LVGL++ platform backend: ${LVGLPP_PLATFORM}")

if (LVGLPP_PLATFORM STREQUAL "SDL")
    find_package(SDL2 REQUIRED SDL2)
    include_directories(${SDL2_INCLUDE_DIRS})
endif ()


option(ENABLE_LUA "Enable Lua bindings (requires lua-5.4)" OFF)

add_subdirectory(sdk)

# Asset pack/directory staging (needs the respack target from sdk/).
include(cmake/assets.cmake)

set(ENABLE_DEMO ON CACHE BOOL "Enable demo application")
if (ENABLE_DEMO)
    add_subdirectory(demo)
endif ()

add_subdirectory(smart_home)

add_subdirectory(test)

# Design-token headers are committed; this target verifies they match
# design/tokens.json. Run it in CI or after editing the token file.
find_package(Python3 COMPONENTS Interpreter QUIET)
if (Python3_Interpreter_FOUND)
    add_custom_target(design-check
        COMMAND ${Python3_EXECUTABLE} -I ${CMAKE_CURRENT_LIST_DIR}/sdk/tools/design/sync_tokens.py generate --check
        COMMENT "Checking design tokens and generated C++ headers"
        VERBATIM)
else ()
    message(STATUS "Python3 not found: design-check target is disabled")
endif ()
