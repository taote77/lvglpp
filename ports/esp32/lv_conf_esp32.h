#ifndef LVGLPP_LV_CONF_ESP32_H
#define LVGLPP_LV_CONF_ESP32_H

/*
 * ESP-IDF LVGL configuration.
 *
 * This file is additive: it reuses the desktop framework/lv_conf.h and then
 * overrides only the settings that do not fit an MCU. Desktop builds never
 * include it (they use framework/lv_conf.h directly).
 *
 * Select it with -DLV_CONF_PATH=".../ports/esp32/lv_conf_esp32.h" and
 * -DLV_KCONFIG_IGNORE=1 so ESP-IDF Kconfig does not redefine these values.
 */

#include "../../framework/lv_conf.h"

/* ST77916 is RGB565. */
#undef LV_COLOR_DEPTH
#define LV_COLOR_DEPTH 16

/* Use libc heap (malloc/free) instead of LVGL's fixed built-in pool. */
#undef LV_USE_STDLIB_MALLOC
#define LV_USE_STDLIB_MALLOC LV_STDLIB_CLIB
#undef LV_USE_STDLIB_STRING
#define LV_USE_STDLIB_STRING LV_STDLIB_CLIB
#undef LV_USE_STDLIB_SPRINTF
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB

/* Not used with LV_STDLIB_CLIB, kept for reference. */
#undef LV_MEM_SIZE
#define LV_MEM_SIZE (128U * 1024U)

/* Desktop-only / too large for the MCU. Re-enable per product if needed. */
#undef LV_USE_THORVG_INTERNAL
#define LV_USE_THORVG_INTERNAL 0
#undef LV_USE_THORVG_EXTERNAL
#define LV_USE_THORVG_EXTERNAL 0

#undef LV_USE_LOTTIE
#define LV_USE_LOTTIE 0

#undef LV_USE_FFMPEG
#define LV_USE_FFMPEG 0

#undef LV_USE_FREETYPE
#define LV_USE_FREETYPE 0

#undef LV_USE_TINY_TTF
#define LV_USE_TINY_TTF 0

#undef LV_USE_GIF
#define LV_USE_GIF 0

#undef LV_USE_QRCODE
#define LV_USE_QRCODE 0

#undef LV_USE_DEMO_EBIKE
#define LV_USE_DEMO_EBIKE 0

/* Keep the desktop LVGL log hooked into Application's callback, which the
 * ESP log backend forwards to esp_log_write(). */

#endif // LVGLPP_LV_CONF_ESP32_H
