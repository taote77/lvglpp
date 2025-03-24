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

#if LV_USE_EVDEV
static void indev_deleted_cb(lv_event_t *e)
{
    if (LV_GLOBAL_DEFAULT()->deinit_in_progress) return;
    auto cursor_obj = static_cast<lv_obj_t *>(lv_event_get_user_data(e));
    lv_obj_delete(cursor_obj);
}

static void discovery_cb(lv_indev_t *indev, lv_evdev_type_t type, void *user_data)
{
    LV_LOG_USER("new '%s' device discovered", type == LV_EVDEV_TYPE_REL ? "REL" : type == LV_EVDEV_TYPE_ABS ? "ABS"
                                                                          : type == LV_EVDEV_TYPE_KEY       ? "KEY"
                                                                                                            : "unknown");

    auto disp = static_cast<lv_display_t *>(user_data);
    lv_indev_set_display(indev, disp);

    if (type == LV_EVDEV_TYPE_REL)
    {
        /* Set the cursor icon */

#if !DISABLE_CURSOR
        LV_IMAGE_DECLARE(mouse_cursor_icon);
        lv_obj_t *cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(indev, cursor_obj);

        /* delete the mouse cursor icon if the device is removed */
        lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);
#endif
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
    //    const char *input_device = getenv_default("LV_LINUX_EVDEV_POINTER_DEVICE", "/dev/input/event1");
    // const char *input_device = getenv("LV_LINUX_EVDEV_POINTER_DEVICE");
    const char *input_device = "/dev/input/event1";

    if (input_device == nullptr)
    {
        LV_LOG_USER("the LV_LINUX_EVDEV_POINTER_DEVICE environment variable is not set. using evdev automatic discovery.");
        lv_evdev_discovery_start(discovery_cb, disp);
        return;
    }

    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, input_device);
    lv_indev_set_display(touch, disp);

    /* Set the cursor icon */
#if !DISABLE_CURSOR
    LV_IMAGE_DECLARE(mouse_cursor_icon);
    lv_obj_t *cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
    lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    lv_indev_set_cursor(touch, cursor_obj);
#endif
}
#endif

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = "/dev/fb0";
    lv_display_t *disp = lv_linux_fbdev_create();

#if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
#endif

    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init()
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t *disp = lv_linux_drm_create();

#if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
#endif

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{
    lv_sdl_window_create(window_width, window_height);
}
#elif LV_USE_WAYLAND
/* see backend/wayland.c */
#else
#error Unsupported configuration
#endif

#if LV_USE_WAYLAND == 0
void lv_linux_run_loop()
{
    uint32_t idle_time{0};

    /*Handle LVGL tasks*/
    while (true)
    {
        idle_time = lv_timer_handler(); /*Returns the time to the next timer execution*/
        usleep(idle_time * 1000);
    }
}
#endif


namespace heygears::sys
{

    DrmApplication::DrmApplication(int argc, char **argv) : Application(argc, argv)
    {
    }

    bool DrmApplication::initApp()
    {
        if (!Application::initApp())
        {
            return false;
        }

        const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
        _display = lv_linux_drm_create();
#if LV_USE_EVDEV
        lv_linux_init_input_pointer(_display);
#endif

        lv_linux_drm_set_file(_display, device, -1);

        return true;
    }

    void DrmApplication::exit(int code)
    {
    }


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

}// namespace heygears::sys

#endif
