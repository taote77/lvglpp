#if defined(LVGLPP_PLATFORM_FRAMEBUF)

#include "FramebufPlatform.h"

#include "EvdevInput.h"
#include "core/log/log.h"

#include <cstdlib>
#include <cstring>

#include <unistd.h>

namespace lvglpp::sys {

namespace {

int envInt(const char *name, int fallback)
{
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0')
    {
        return fallback;
    }
    char *end = nullptr;
    const long parsed = std::strtol(value, &end, 10);
    return (end != value) ? static_cast<int>(parsed) : fallback;
}

lv_display_rotation_t rotationFromDegrees(int degrees)
{
    int normalized = degrees % 360;
    if (normalized < 0)
    {
        normalized += 360;
    }
    switch (normalized)
    {
    case 90:
        return LV_DISPLAY_ROTATION_90;
    case 180:
        return LV_DISPLAY_ROTATION_180;
    case 270:
        return LV_DISPLAY_ROTATION_270;
    default:
        return LV_DISPLAY_ROTATION_0;
    }
}

} // namespace

FramebufPlatform::FramebufPlatform(int argc, char **argv)
{
    (void)argc;
    (void)argv;
}

FramebufPlatform::~FramebufPlatform()
{
    shutdown();
}

bool FramebufPlatform::init()
{
#if LV_USE_LINUX_FBDEV
    const char *device = std::getenv("LV_LINUX_FBDEV_DEVICE");
    const char *path   = (device != nullptr && *device != '\0') ? device : "/dev/fb0";
    if (::access(path, R_OK | W_OK) != 0)
    {
        LOG_ERROR() << "framebuffer device is not accessible: " << path;
        return false;
    }

    display_ = lv_linux_fbdev_create();
    if (display_ == nullptr)
    {
        LOG_ERROR() << "failed to create the framebuffer display";
        return false;
    }

    lv_display_set_default(display_);
    lv_linux_fbdev_set_file(display_, path);

    native_width_    = lv_display_get_horizontal_resolution(display_);
    native_height_   = lv_display_get_vertical_resolution(display_);
    rotation_offset_ = envInt("LVGLPP_PANEL_ROTATION_OFFSET", 0);

    detail::attachEvdevPointer(display_);
    return true;
#else
    LOG_ERROR() << "FRAMEBUF platform selected but LV_USE_LINUX_FBDEV is disabled";
    return false;
#endif
}

void FramebufPlatform::shutdown()
{
    // The display is owned and deleted by lv_deinit().
    display_ = nullptr;
}

bool FramebufPlatform::applyOrientation(Orientation orientation)
{
    if (display_ == nullptr)
    {
        return false;
    }

    // The framebuffer size is fixed; the LVGL fbdev driver rotates the
    // PARTIAL flush buffer in software.
    lv_display_set_rotation(display_, rotationFromDegrees(static_cast<int>(orientation) + rotation_offset_));
    return true;
}

void FramebufPlatform::setTheme(lv_theme_t *theme)
{
    if (theme == nullptr)
    {
        LOG_WARN() << "theme invalid";
        return;
    }
    if (display_ != nullptr)
    {
        lv_display_set_theme(display_, theme);
    }
}

} // namespace lvglpp::sys

#endif // LVGLPP_PLATFORM_FRAMEBUF
