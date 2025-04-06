cmake_minimum_required(VERSION 3.16)

# using c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project(lvglpp C CXX)

add_compile_options(-fPIC)


# -DUSED_WAYLAND = ON

enable_language(ASM)

set(LV_CONF_BUILD_DISABLE_EXAMPLES 1)
set(LV_CONF_BUILD_DISABLE_DEMOS 1)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    add_definitions(-DUSED_FRAMEBUFF)
    #add_definitions(-DUSED_WAYLAND)
else ()
    find_package(SDL2 REQUIRED SDL2)
    add_definitions(-DUSED_SDL)
    add_definitions(-DLV_USE_SDL)

    include_directories(${SDL2_INCLUDE_DIRS})
endif ()

add_compile_definitions(USED_SDL ON)

    
# add_compile_definitions(DLV_USE_SDL ON)


# Main include files of the project
include_directories(${PROJECT_SOURCE_DIR}/main/inc)

# Define options for LVGL with default values (OFF)
option(LV_USE_DRAW_SDL "Use SDL draw unit" OFF)
option(LV_USE_LIBPNG "Use libpng to decode PNG" OFF)
option(LV_USE_LIBJPEG_TURBO "Use libjpeg turbo to decode JPEG" OFF)
option(LV_USE_FFMPEG "Use libffmpeg to display video using lv_ffmpeg" ON)
option(LV_USE_FREETYPE "Use freetype library" OFF)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output path for the executable
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Find and include SDL2 library
find_package(SDL2 REQUIRED)

# Remove ARM-specific compile and linker options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

# Add compile definitions based on the selected options
add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBPNG}>:LV_USE_LIBPNG=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBJPEG_TURBO}>:LV_USE_LIBJPEG_TURBO=1>)
add_compile_definitions($<$<BOOL:${LV_USE_FFMPEG}>:LV_USE_FFMPEG=1>)

# Add LVGL subdirectory
add_subdirectory(lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR} ${SDL2_INCLUDE_DIRS})


file(GLOB_RECURSE TOOLS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/core/*.c *.cpp  *.cc *.hpp *.h)


add_library(${PROJECT_NAME}  SHARED 
    ${WAYLAND_SRC}
    ${TOOLS_SRC}
    mouse_cursor_icon.c
)

add_subdirectory(net)



target_link_libraries(${PROJECT_NAME} 
    wayland-cursor
    wayland-client
    wayland-server
    wayland-egl
    xkbcommon
)


target_include_directories(${PROJECT_NAME} PRIVATE
    lvgl/src/
    lvgl/src/core
)

# Define LVGL configuration as a simple include
target_compile_definitions(${PROJECT_NAME} PRIVATE
    LV_CONF_INCLUDE_SIMPLE
)

find_package(Boost REQUIRED COMPONENTS log  log_setup filesystem json)

target_link_libraries(${PROJECT_NAME}
    lvgl
    # lvgl::examples
    # lvgl::demos
    lvgl::thorvg
    ${SDL2_LIBRARIES}
    ${Boost_LIBRARIES}
    sqlite3
    m
    pthread
    ws_gw
    avformat avcodec avutil swscale
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    BOOST_ALL_DYN_LINK
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

# Conditionally include and link FreeType if LV_USE_FREETYPE is enabled
if(LV_USE_FREETYPE)
    find_package(Freetype REQUIRED)
    target_include_directories(lvgl PUBLIC ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
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



# add_subdirectory(test)