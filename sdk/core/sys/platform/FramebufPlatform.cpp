#if defined(LVGLPP_PLATFORM_FRAMEBUF)

#include "FramebufPlatform.h"

#include "EvdevInput.h"
#include "core/log/log.h"

#include <cstdlib>

#include <unistd.h>

namespace lvglpp::sys {

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
