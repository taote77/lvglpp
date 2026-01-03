#ifdef USE_LINUX_DRM

#include "DrmApplication.h"

#include "lvgl.h"
#include "lvgl/src/core/lv_global.h"

#define DISABLE_CURSOR 1

static void configure_simulator(int argc, char **argv);

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ?: dflt;
}

static void lv_linux_disp_init()
{
    const char   *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t *disp   = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
}

namespace lvglpp::sys {

DrmApplication::DrmApplication(int argc, char **argv) : Application(argc, argv)
{}

DrmApplication::~DrmApplication()
{}

bool DrmApplication::initApp()
{
    if (!Application::initApp())
    {
        return false;
    }

    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    _display           = lv_linux_drm_create();

    lv_linux_drm_set_file(_display, device, -1);

    return true;
}

void DrmApplication::exit(int code)
{}

lv_display_t *DrmApplication::getDisplay()
{
    return _display;
}
/**
 * @brief Set the Theme object
 *
 * @param th 主题
 */
void DrmApplication::setTheme(lv_theme_t *theme)
{
    if (!theme)
    {
        LOG_WARN() << "theme invalid";
        return;
    }

    if (_display)
    {
        lv_display_set_theme(_display, theme);
    }
}

} // namespace lvglpp::sys

#endif //
