#ifdef USED_WAYLAND
#  include "WaylandApplication.h"
// #  include "lv_drivers/wayland/wayland.h"
#  include "lvgl.h"
namespace heygears {
namespace sys {
WaylandApplication::WaylandApplication(int argc, char **argv) : Application(argc, argv) { }

bool WaylandApplication::initApp()
{
    if (!Application::initApp()) {
        return false;
    }
    // lv_wayland_init();
    lv_wayland_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX, (char *)"Wayland window",
                             [](lv_disp_t *disp) -> bool {
                                 lv_wayland_window_close(disp);
                                 return true;
                             });
    return true;
}

void WaylandApplication::exit(int c)
{
    // lv_wayland_deinit();
}
} // namespace sys
} // namespace heygears
#endif