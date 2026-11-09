
#define LV_HOR_RES_MAX 800
#define LV_VER_RES_MAX 480

#define LV_SIM_WINDOW_WIDTH 800
#define LV_SIM_WINDOW_HEIGHT 480

#define LV_USE_MATRIX 1

#ifndef LVGLPP_ENABLE_ROTATION
#define LVGLPP_ENABLE_ROTATION 1
#endif
#if LVGLPP_ENABLE_ROTATION
// Enable matrix rotation for DIRECT renderers (SDL/DRM). FBDEV uses the
// driver's PARTIAL flush rotation and does not need this.
#define LV_DRAW_TRANSFORM_USE_MATRIX 1
#endif
// #define LV_USE_LINUX_FBDEV	0
// #define LV_USE_LINUX_DRM	0
// #define LV_USE_SDL		0

// FreeType is controlled at CMake level: -DLVGLPP_ENABLE_FREETYPE=ON
// (default OFF; bitmap fonts are used otherwise).

// option
// #define LV_WAYLAND_WINDOW_DECORATIONS 1

// #define LV_WAYLAND_WL_SHELL 1

#define LV_USE_LOTTIE 1

#define LV_USE_DEMO_EBIKE 1

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 480

// #define LV_USE_BIDI 1

// #define LV_USE_ARABIC_PERSIAN_CHARS    1