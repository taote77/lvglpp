#if defined(LVGLPP_PLATFORM_DRM)

#include "DrmPlatform.h"

#include "EvdevInput.h"
#include "core/log/log.h"

#include <cstdlib>

#include <unistd.h>

namespace lvglpp::sys {

DrmPlatform::DrmPlatform(int argc, char **argv)
{
    (void)argc;
    (void)argv;
}

DrmPlatform::~DrmPlatform()
{
    shutdown();
}

bool DrmPlatform::init()
{
#if LV_USE_LINUX_DRM
    const char *device = std::getenv("LV_LINUX_DRM_CARD");
    const char *path   = (device != nullptr && *device != '\0') ? device : "/dev/dri/card0";
    if (::access(path, R_OK | W_OK) != 0)
    {
        LOG_ERROR() << "DRM card device is not accessible: " << path;
        return false;
    }

    display_ = lv_linux_drm_create();
    if (display_ == nullptr)
    {
        LOG_ERROR() << "failed to create the DRM display";
        return false;
    }

    lv_linux_drm_set_file(display_, path, -1);
    lv_display_set_default(display_);
    detail::attachEvdevPointer(display_);
    return true;
#else
    LOG_ERROR() << "DRM platform selected but LV_USE_LINUX_DRM is disabled";
    return false;
#endif
}

void DrmPlatform::shutdown()
{
    // The display is owned and deleted by lv_deinit().
    display_ = nullptr;
}

void DrmPlatform::setTheme(lv_theme_t *theme)
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

#endif // LVGLPP_PLATFORM_DRM
