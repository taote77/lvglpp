# ESP-IDF component for lvglpp + EspPlatform.
#
# Status: scaffold. It is intentionally not part of the desktop CMake build.
# The core files listed below still need the ESP_PLATFORM guards described in
# docs/ESP32_PORTING.md (§2) before this component links on a real target.
#
# Usage in the ESP-IDF project:
#   set(EXTRA_COMPONENT_DIRS
#       "${CMAKE_CURRENT_LIST_DIR}/../../framework/lvgl"
#       "${CMAKE_CURRENT_LIST_DIR}")
#   include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#
# LVGL 9.5 has its own ESP-IDF support (framework/lvgl/env_support/cmake/esp.cmake)
# and is registered as the `lvgl` component.

set(LVGLPP_SDK_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../framework")

# Core sources that do not depend on a desktop OS. Keep this list explicit so
# portability regressions are visible; globs are used only inside the portable
# subdirectories.
file(GLOB_RECURSE LVGLPP_CORE_SOURCES CONFIGURE_DEPENDS
    "${LVGLPP_SDK_ROOT}/core/base/*.cpp"
    "${LVGLPP_SDK_ROOT}/core/widgets/*.cpp"
    "${LVGLPP_SDK_ROOT}/core/style/*.cpp"
    "${LVGLPP_SDK_ROOT}/core/sys/*.cpp"
    "${LVGLPP_SDK_ROOT}/core/tools/*.cpp"
)
list(APPEND LVGLPP_CORE_SOURCES
    "${LVGLPP_SDK_ROOT}/config/color_sheet.cpp"
    "${LVGLPP_SDK_ROOT}/core/sys/platform/PlatformFactory.cpp"
)

# Desktop-only backends and optional subsystems not built for ESP-IDF.
list(FILTER LVGLPP_CORE_SOURCES EXCLUDE REGEX "/core/sys/platform/(Sdl|Framebuf|Drm)Platform\\.cpp$")
list(FILTER LVGLPP_CORE_SOURCES EXCLUDE REGEX "/core/sys/platform/EvdevInput\\.cpp$")
list(FILTER LVGLPP_CORE_SOURCES EXCLUDE REGEX "/core/sys/automation/")
list(FILTER LVGLPP_CORE_SOURCES EXCLUDE REGEX "/core/log/log\\.cpp$")


# AssetManager/ImageManager/AssetPack now have ESP_PLATFORM branches:
# embedded/SPIFFS pack, no mmap/filesystem directory source, synchronous
# prefetch. Linux keeps the original implementations.

set(LVGLPP_LV_CONF "${CMAKE_CURRENT_LIST_DIR}/lv_conf_esp32.h")

idf_component_register(
    SRCS
        "EspPlatform.cpp"
        ${LVGLPP_CORE_SOURCES}
    INCLUDE_DIRS
        "."
        "include"
        "${LVGLPP_SDK_ROOT}"
    REQUIRES
        lvgl
        esp_timer
        esp_lcd
        driver
)

# Make the ESP config authoritative for both LVGL and lvglpp. LV_KCONFIG_IGNORE
# prevents lv_conf_kconfig.h from redefining the same macros.
if(TARGET lvgl)
    target_compile_definitions(lvgl PUBLIC
        LV_CONF_PATH="${LVGLPP_LV_CONF}"
        LV_KCONFIG_IGNORE=1
    )
endif()

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    ESP_PLATFORM=1
    LVGLPP_PLATFORM_ESP32=1
    LV_CONF_PATH="${LVGLPP_LV_CONF}"
    LV_KCONFIG_IGNORE=1
)

# Keep third-party/stb warnings non-fatal under IDF's global -Werror.
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error)

# Fonts: do NOT glob framework/core/tools/fonts/*.c. Add only the profile generated
# by framework/tools/i18n/gen_fonts.py (--langs/--sizes/--bpp/--compress), for
# example:
#
# target_sources(${COMPONENT_LIB} PRIVATE
#     "${CMAKE_CURRENT_LIST_DIR}/fonts/Noto_zh_cn_16_Normal.c"
#     "${CMAKE_CURRENT_LIST_DIR}/fonts/Noto_zh_cn_24_Normal.c"
# )
