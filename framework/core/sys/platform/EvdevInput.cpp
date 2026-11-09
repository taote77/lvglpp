#include "EvdevInput.h"

#if LV_USE_EVDEV

#include "core/log/log.h"

#include <cstdlib>

namespace lvglpp::sys::detail {

namespace {

void discoveryCallback(lv_indev_t *indev, lv_evdev_type_t type, void *userData)
{
    (void)type;
    auto *display = static_cast<lv_display_t *>(userData);
    if (indev == nullptr || display == nullptr)
    {
        return;
    }
    lv_indev_set_display(indev, display);
}

} // namespace

void attachEvdevPointer(lv_display_t *display)
{
    if (display == nullptr)
    {
        return;
    }

    const char *device = std::getenv("LV_LINUX_EVDEV_POINTER_DEVICE");
    if (device == nullptr || *device == '\0')
    {
        if (lv_evdev_discovery_start(discoveryCallback, display) != LV_RESULT_OK)
        {
            LOG_WARN() << "evdev automatic discovery could not be started";
        }
        return;
    }

    lv_indev_t *indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, device);
    if (indev == nullptr)
    {
        LOG_WARN() << "cannot open evdev pointer device: " << device;
        return;
    }
    lv_indev_set_display(indev, display);
}

} // namespace lvglpp::sys::detail

#else

namespace lvglpp::sys::detail {

void attachEvdevPointer(lv_display_t *display)
{
    (void)display;
}

} // namespace lvglpp::sys::detail

#endif // LV_USE_EVDEV
