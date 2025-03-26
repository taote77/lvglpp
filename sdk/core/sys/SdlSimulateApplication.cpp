/**************************************************************************


**************************************************************************/

#include "core/log/log.h"

#if (defined(USED_SDL)) || (defined(USED_SDL_GPU))

#  include "SdlSimulateApplication.h"
#  include "lvgl/src/core/lv_global.h"
#  include <lvgl.h>

#  if defined(USED_SDL)
// #include <sdl/sdl.h>
#  elif defined(USED_SDL_GPU)
#    include <sdl/sdl_gpu.h>
#  endif

#  include <unistd.h>

uint16_t window_width;
uint16_t window_height;
bool fullscreen;
bool maximize;

static void configure_simulator(int argc, char **argv)
{
    int opt = 0;
    bool err = false;

    /* Default values */
    fullscreen = maximize = false;
    window_width = atoi(getenv("LV_SIM_WINDOW_WIDTH") ?: "800");
    window_height = atoi(getenv("LV_SIM_WINDOW_HEIGHT") ?: "480");

    /* Parse the command-line options. */
    while ((opt = getopt(argc, argv, "fmw:h:")) != -1) {
        switch (opt) {
        case 'f':
            fullscreen = true;
            if (LV_USE_WAYLAND == 0) {
                fprintf(stderr, "The SDL driver doesn't support fullscreen mode on start\n");
                exit(1);
            }
            break;
        case 'm':
            maximize = true;
            if (LV_USE_WAYLAND == 0) {
                fprintf(stderr, "The SDL driver doesn't support maximized mode on start\n");
                exit(1);
            }
            break;
        case 'w':
            window_width = atoi(optarg);
            break;
        case 'h':
            window_height = atoi(optarg);
            break;
        case ':':
            fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            exit(1);
        case '?':
            fprintf(stderr, "Unknown option -%c.\n", optopt);
            exit(1);
        }
    }
}

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ?: dflt;
}

#  if LV_USE_EVDEV
static void indev_deleted_cb(lv_event_t *e)
{
    if (LV_GLOBAL_DEFAULT()->deinit_in_progress) {
        return;
    }
    auto cursor_obj = static_cast<lv_obj_t *>(lv_event_get_user_data(e));
    lv_obj_delete(cursor_obj);
}

static void discovery_cb(lv_indev_t *indev, lv_evdev_type_t type, void *user_data)
{
    LV_LOG_USER("new '%s' device discovered",
                type == LV_EVDEV_TYPE_REL           ? "REL"
                        : type == LV_EVDEV_TYPE_ABS ? "ABS"
                        : type == LV_EVDEV_TYPE_KEY ? "KEY"
                                                    : "unknown");

    auto disp = static_cast<lv_display_t *>(user_data);
    lv_indev_set_display(indev, disp);

    if (type == LV_EVDEV_TYPE_REL) {
        /* Set the cursor icon */
        LV_IMAGE_DECLARE(mouse_cursor_icon);
        lv_obj_t *cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(indev, cursor_obj);

        /* delete the mouse cursor icon if the device is removed */
        lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);
    }
}

static void lv_linux_init_input_pointer(lv_display_t *disp)
{
    /* Enables a pointer (touchscreen/mouse) input device
     * Use 'evtest' to find the correct input device. /dev/input/by-id/ is recommended if possible
     * Use /dev/input/by-id/my-mouse-or-touchscreen or /dev/input/eventX
     *
     * If LV_LINUX_EVDEV_POINTER_DEVICE is not set, automatic evdev disovery will start
     */
    const char *input_device = getenv("LV_LINUX_EVDEV_POINTER_DEVICE");

    if (input_device == NULL) {
        LV_LOG_USER("the LV_LINUX_EVDEV_POINTER_DEVICE environment variable is not set. using "
                    "evdev automatic discovery.");
        lv_evdev_discovery_start(discovery_cb, disp);
        return;
    }

    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, input_device);
    lv_indev_set_display(touch, disp);

    /* Set the cursor icon */
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t *cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(touch, cursor_obj);
}
#  endif

void lv_linux_run_loop()
{
    uint32_t idle_time = 0;

    /*Handle LVGL tasks*/
    while (true) {

        idle_time = lv_timer_handler(); /*Returns the time to the next timer execution*/
        usleep(idle_time * 1000);
    }
}

namespace lvglpp::sys {
SdlSimulateApplication::SdlSimulateApplication(int argc, char **argv) : Application(argc, argv)
{
    configure_simulator(argc, argv);
}

lv_display_t *SdlSimulateApplication::getDisplay()
{
    return _display;
}

bool SdlSimulateApplication::initApp()
{
    if (!Application::initApp()) {
        LV_LOG_WARN("Application init failed!");
    }
    halInit();
    return true;
}

void SdlSimulateApplication::setTheme(lv_theme_t *theme)
{
    if (!theme) {
        LOG_WARN() << "theme invalid";
        return;
    }

    if (_display) {
        lv_display_set_theme(_display, theme);
    }
}

void SdlSimulateApplication::exit(int code)
{
    quick_exit(code);
}

void SdlSimulateApplication::lv_linux_disp_init()
{

    _display = lv_sdl_window_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);

    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, _display);
    lv_display_set_default(_display);
}

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
void SdlSimulateApplication::halInit()
{
    /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
    lv_linux_disp_init();
}
} // namespace lvglpp::sys

#endif
