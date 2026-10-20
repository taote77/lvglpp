#ifndef LVGLPP_EVDEVINPUT_H
#define LVGLPP_EVDEVINPUT_H

#include <lvgl.h>

namespace lvglpp::sys::detail {

/**
 * Attach an evdev pointer device to the display.
 *
 * Uses LV_LINUX_EVDEV_POINTER_DEVICE when set, otherwise starts LVGL's evdev
 * discovery. No-op when LV_USE_EVDEV is disabled.
 */
void attachEvdevPointer(lv_display_t *display);

} // namespace lvglpp::sys::detail

#endif // LVGLPP_EVDEVINPUT_H
