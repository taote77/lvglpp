#ifndef LVGLPP_UI_FEATURES_H
#define LVGLPP_UI_FEATURES_H

/*
 * Compile-time UI feature switches.
 *
 * Both default to 1 so a plain CMake build keeps the runtime mechanism. A
 * product that never rotates or mirrors can configure:
 *
 *   -DLVGLPP_ENABLE_ROTATION=OFF -DLVGLPP_ENABLE_RTL=OFF
 *
 * and the rotation/RTL code paths, per-widget mirror state and matrix
 * transform configuration are removed at compile time.
 */

#ifndef LVGLPP_ENABLE_ROTATION
#define LVGLPP_ENABLE_ROTATION 1
#endif

#ifndef LVGLPP_ENABLE_RTL
#define LVGLPP_ENABLE_RTL 1
#endif

#define LVGLPP_DYNAMIC_UI (LVGLPP_ENABLE_ROTATION || LVGLPP_ENABLE_RTL)

#endif // LVGLPP_UI_FEATURES_H
