#if defined(LVGLPP_PLATFORM_DRM)

#include "DrmPlatform.h"

#include "EvdevInput.h"
#include "core/log/log.h"

#include <cstdlib>
#include <cstring>

#include <unistd.h>

namespace lvglpp::sys {

namespace {

bool envFlag(const char *name)
{
    const char *value = std::getenv(name);
    return value != nullptr
        && (std::strcmp(value, "1") == 0 || std::strcmp(value, "true") == 0
            || std::strcmp(value, "yes") == 0 || std::strcmp(value, "on") == 0);
}

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

    native_width_    = lv_display_get_horizontal_resolution(display_);
    native_height_   = lv_display_get_vertical_resolution(display_);
    rotation_offset_ = envInt("LVGLPP_PANEL_ROTATION_OFFSET", 0);
    rotation_enabled_ = !envFlag("LVGLPP_DISABLE_PANEL_ROTATION");

#if LV_DRAW_TRANSFORM_USE_MATRIX
    // DRM uses DIRECT rendering; the core applies the matrix rotation before
    // the dumb buffer is committed. This is software rotation (CPU cost).
    lv_display_set_matrix_rotation(display_, true);
#else
    rotation_enabled_ = false;
#endif

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

bool DrmPlatform::applyOrientation(Orientation orientation)
{
    if (display_ == nullptr || !rotation_enabled_)
    {
        return false;
    }

    lv_display_set_rotation(display_, rotationFromDegrees(static_cast<int>(orientation) + rotation_offset_));
    return true;
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
