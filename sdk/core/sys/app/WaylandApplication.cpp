#include "WaylandApplication.h"
#include "lvgl.h"
#include "core/sys/StackView.h"

namespace lvglpp::sys {
WaylandApplication::WaylandApplication(int argc, char **argv) : Application(argc, argv)
{}

bool WaylandApplication::initApp()
{
    if (!Application::initApp())
    {
        return false;
    }

    lv_disp_t *disp;

    // lv_wayland_init();
    // disp = lv_wayland_create_window(LV_HOR_RES_MAX, LV_VER_RES_MAX, "Window Title", NULL);
#ifdef USED_WAYLAND
    disp = lv_wayland_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX, "Window Title", NULL);
#endif
    if (disp != NULL)
    {
        // lv_wayland_window_set_fullscreen(disp, true);
    } else
    {
        return false;
    }

    return true;
}

void WaylandApplication::exit(int c)
{
    StackView::getInstance().shutdown();
    std::exit(c);
}
} // namespace lvglpp::sys
