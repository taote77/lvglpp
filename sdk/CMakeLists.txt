cmake_minimum_required(VERSION 3.16)

# using c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project(lvglpp C CXX)

add_compile_options(-fPIC)


enable_language(ASM)

if (ENABLE_LUA)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LUA REQUIRED lua-5.4)
endif ()

# set(LV_CONF_BUILD_DISABLE_EXAMPLES 1)
# set(LV_CONF_BUILD_DISABLE_DEMOS 1)


if (LVGLPP_PLATFORM STREQUAL "DRM")
    include_directories(/usr/include/drm)
endif ()


if (ENABLE_LUA)
    include_directories(${LUA_INCLUDE_DIRS})
endif ()

# Define options for LVGL with default values (OFF)
option(LV_USE_DRAW_SDL "Use SDL draw unit" OFF)
option(LV_USE_LIBPNG "Use libpng to decode PNG" OFF)
option(LV_USE_LIBJPEG_TURBO "Use libjpeg turbo to decode JPEG" OFF)
option(LV_USE_FFMPEG "Use libffmpeg to display video using lv_ffmpeg" ON)
option(LVGLPP_ENABLE_FREETYPE "Enable FreeType vector font support (default: bitmap fonts only)" OFF)
option(LVGLPP_ENABLE_ROTATION "Enable runtime four-orientation support" ON)
option(LVGLPP_ENABLE_RTL "Enable runtime RTL layout mirroring" ON)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Keep EXECUTABLE_OUTPUT_PATH inherited from the top-level build
# (build/target/bin) so subprojects never write artifacts into the source tree.
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})


# Architecture-specific optimizations (opt-in via -DUSE_MARCH_NATIVE=ON)
option(USE_MARCH_NATIVE "Compile with -march=native for best performance on this CPU" OFF)
if(USE_MARCH_NATIVE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# Add compile definitions based on the selected options
add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBPNG}>:LV_USE_LIBPNG=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBJPEG_TURBO}>:LV_USE_LIBJPEG_TURBO=1>)
add_compile_definitions($<$<BOOL:${LV_USE_FFMPEG}>:LV_USE_FFMPEG=1>)

# Add LVGL subdirectory
add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})
target_compile_definitions(lvgl PUBLIC
    LVGLPP_ENABLE_ROTATION=$<BOOL:${LVGLPP_ENABLE_ROTATION}>
    LVGLPP_ENABLE_RTL=$<BOOL:${LVGLPP_ENABLE_RTL}>)

# FreeType is an opt-in, project-level dependency. It no longer lives inside
# the vendored LVGL tree; when disabled, LVGL is built with LV_USE_FREETYPE=0
# and the SDK uses the compiled-in bitmap fonts only.
if(LVGLPP_ENABLE_FREETYPE)
    # Keep the vendored FreeType build hermetic: no optional compression /
    # shaping dependencies, no install rules, and always a static library so
    # liblvglpp.so never depends on an uninstalled libfreetype.so.
    set(FT_DISABLE_ZLIB TRUE CACHE BOOL "" FORCE)
    set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "" FORCE)
    set(FT_DISABLE_PNG TRUE CACHE BOOL "" FORCE)
    set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "" FORCE)
    set(FT_DISABLE_BROTLI TRUE CACHE BOOL "" FORCE)
    set(SKIP_INSTALL_ALL TRUE)

    set(_lvglpp_had_build_shared_libs FALSE)
    if(DEFINED BUILD_SHARED_LIBS)
        set(_lvglpp_had_build_shared_libs TRUE)
        set(_lvglpp_saved_build_shared_libs "${BUILD_SHARED_LIBS}")
    endif()
    set(BUILD_SHARED_LIBS OFF)

    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype
                     ${CMAKE_CURRENT_BINARY_DIR}/third_party/freetype
                     EXCLUDE_FROM_ALL)

    if(_lvglpp_had_build_shared_libs)
        set(BUILD_SHARED_LIBS "${_lvglpp_saved_build_shared_libs}")
        unset(_lvglpp_saved_build_shared_libs)
    else()
        unset(BUILD_SHARED_LIBS)
    endif()
    unset(_lvglpp_had_build_shared_libs)
    unset(SKIP_INSTALL_ALL)

    # PUBLIC so LVGL, lvglpp and every app/test see the same macro value.
    target_compile_definitions(lvgl PUBLIC LV_USE_FREETYPE=1)
    target_link_libraries(lvgl PUBLIC freetype)
endif()

if (LVGLPP_PLATFORM STREQUAL "SDL")
    target_link_libraries(lvgl PUBLIC ${SDL2_LIBRARIES})
elseif (LVGLPP_PLATFORM STREQUAL "DRM")
    target_link_libraries(lvgl PUBLIC drm)
endif ()


file(GLOB_RECURSE TOOLS_SRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h
)


add_library(${PROJECT_NAME}  SHARED 
    ${WAYLAND_SRC}
    ${TOOLS_SRC}
    mouse_cursor_icon.c
)

add_subdirectory(net)
add_subdirectory(wrappers)


target_link_libraries(${PROJECT_NAME}
    # wayland-cursor
    # wayland-client
    # wayland-server
    # wayland-egl
)

if (ENABLE_LUA)
    target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})
endif ()


target_include_directories(${PROJECT_NAME} PRIVATE
    lvgl/src/
    lvgl/src/core
)

if (ENABLE_LUA)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/demo/clua/include
    )
endif ()

# Define LVGL configuration as a simple include
target_compile_definitions(${PROJECT_NAME} PRIVATE
    LV_CONF_INCLUDE_SIMPLE
)

target_link_libraries(${PROJECT_NAME}
    lvgl
    lvgl::thorvg
    # ${SDL2_LIBRARIES}
    m
    pthread
    ws_gw
    avformat avcodec avutil swscale
)


# Custom target to run the executable
add_custom_target(run COMMAND ${EXECUTABLE_OUTPUT_PATH}/${PROJECT_NAME} DEPENDS ${PROJECT_NAME})

# Conditionally include and link SDL2_image if LV_USE_DRAW_SDL is enabled
if(LV_USE_DRAW_SDL)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
    find_package(SDL2_image REQUIRED)
    target_include_directories(lvgl PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_IMAGE_LIBRARIES})
endif()


# Conditionally include and link libpng if LV_USE_LIBPNG is enabled
if(LV_USE_LIBPNG)
    find_package(PNG REQUIRED)
    target_include_directories(lvgl PUBLIC ${PNG_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARIES})
endif()


# Conditionally include and link libjpeg-turbo if LV_USE_LIBJPEG_TURBO is enabled
if(LV_USE_LIBJPEG_TURBO)
    find_package(JPEG REQUIRED)
    target_include_directories(lvgl PUBLIC ${JPEG_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${JPEG_LIBRARIES})
endif()


# Conditionally include and link FFmpeg libraries if LV_USE_FFMPEG is enabled
if(LV_USE_FFMPEG)
    target_link_libraries(${PROJECT_NAME} avformat avcodec avutil swscale z)
endif()

# Apply additional compile options if the build type is Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug mode enabled")

    target_compile_options(lvgl PRIVATE
        # -pedantic-errors
        # -Wall
        # -Wclobbered
        # -Wdeprecated
        # -Wdouble-promotion
        # -Wempty-body
        # -Wextra
        # -Wformat-security
        # -Wmaybe-uninitialized
        # -Wmissing-prototypes
        # -Wpointer-arith
        # -Wmultichar
        # -Wno-pedantic # ignored for now, we convert functions to pointers for properties table.
        # -Wreturn-type
        # -Wshadow
        # -Wshift-negative-value
        # -Wsizeof-pointer-memaccess
        # -Wtype-limits
        # -Wundef
        # -Wuninitialized
        # -Wunreachable-code
        # -Wfloat-conversion
        # -Wstrict-aliasing
    )

if (ASAN)
    message(STATUS "AddressSanitizer enabled")

    # Add AddressSanitizer flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
else()
    message(STATUS "AddressSanitizer disabled")
endif()
endif()


add_subdirectory(tools/respack)