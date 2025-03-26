
#ifdef USED_WAYLAND
#  include "WaylandApplication.h"
// #include "lv_drivers/wayland/wayland.h"
#  include "lvgl.h"
namespace lvglpp {
namespace sys {
WaylandApplication::WaylandApplication(int argc, char **argv) : Application(argc, argv) { }

bool WaylandApplication::initApp()
{
    if (!Application::initApp()) {
        return false;
    }

    lv_disp_t *disp;

    // lv_wayland_init();
    // disp = lv_wayland_create_window(LV_HOR_RES_MAX, LV_VER_RES_MAX, "Window Title", NULL);

    disp = lv_wayland_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX, "Window Title", NULL);

    if (disp != NULL) {
        // lv_wayland_window_set_fullscreen(disp, true);
    } else {
        return false;
    }

    return true;
}

void WaylandApplication::exit(int c)
{
    // lv_wayland_deinit();
}
} // namespace sys
} // namespace lvglpp
#endif