#ifdef USED_FRAMEBUFF
#  include "LinuxFramebufApplication.h"
// #include "lv_drivers/display/fbdev.h"
// #include "lv_drivers/indev/evdev.h"
#  include "lvgl/src/core/lv_global.h"

// #define DISP_BUF_SIZE (128 * 1024)
#  define DISP_BUF_SIZE (LV_HOR_RES_MAX * LV_VER_RES_MAX * 4)

#  if LV_USE_EVDEV
static void indev_deleted_cb(lv_event_t *e)
{
    if (LV_GLOBAL_DEFAULT()->deinit_in_progress)
        return;
    lv_obj_t *cursor_obj = static_cast<lv_obj_t *>(lv_event_get_user_data(e));
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
        // LV_IMAGE_DECLARE(mouse_cursor_icon);
        // lv_obj_t *cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
        // lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        // lv_indev_set_cursor(indev, cursor_obj);

        /* delete the mouse cursor icon if the device is removed */
        // lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);
    }
}

#  endif

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ?: dflt;
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
    // LV_IMAGE_DECLARE(mouse_cursor_icon);
    // lv_obj_t *cursor_obj = lv_image_create(lv_display_get_screen_active(disp));
    // lv_image_set_src(cursor_obj, &mouse_cursor_icon);
    // lv_indev_set_cursor(touch, cursor_obj);
}

namespace lvglpp {
namespace sys {

#  if 1
LinuxFramebufApplication::LinuxFramebufApplication(int argc, char **argv) : Application(argc, argv)
{
}

void LinuxFramebufApplication::framebufInit()
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    _disp = lv_linux_fbdev_create();

#    if LV_USE_EVDEV
    lv_linux_init_input_pointer(_disp);
#    endif

    lv_linux_fbdev_set_file(_disp, device);
}

void LinuxFramebufApplication::exit(int c)
{
    abort();
}

bool LinuxFramebufApplication::initApp()
{
    if (!Application::initApp()) {
        LV_LOG_WARN("Application init failed!");
    }
    framebufInit();
    return true;
}

void LinuxFramebufApplication::clearAppScreen()
{
    // lv_area_t   flush_area{0, 0, lv_disp_get_hor_res(nullptr) - 1, lv_disp_get_ver_res(nullptr) -
    // 1}; lv_color_t *flush_buf = new lv_color_t[lv_disp_get_hor_res(nullptr) *
    // lv_disp_get_ver_res(nullptr)]; memset(flush_buf, 0, sizeof(lv_color_t) *
    // lv_disp_get_hor_res(nullptr) * lv_disp_get_ver_res(nullptr)); fbdev_flush(&disp_drv,
    // &flush_area, flush_buf); delete[] flush_buf;
}

// void LinuxFramebufApplication::setPointerFeedbackCb(void (*mouseFeedback)(lv_indev_drv_t *,
// uint8_t))
// {
//     if (mouseFeedback == nullptr)
//         return;

//     lv_indev_set_feedback_cb(mouse_indev_, [](lv_indev_drv_t *drv, uint8_t event) {
//         mouseFeedback(drv, event);
//     });
// }
#  else
LinuxFramebufApplication::LinuxFramebufApplication(int argc, char **argv)
    : Application(argc, argv), disp_drv()
{
}

void LinuxFramebufApplication::framebufInit()
{
    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE], buf1[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, buf1, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.full_refresh = 1;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and
     * state*/
    indev_drv_1.read_cb = evdev_read;
    mouse_indev_ = lv_indev_drv_register(&indev_drv_1);

    // #if 0
    //             /*Set a cursor for the mouse*/
    //             LV_IMG_DECLARE(mouse_cursor_icon)
    //             lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for
    //             the cursor */ lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the
    //             image source*/ lv_indev_set_cursor(mouse_indev, cursor_obj); /*Connect the image
    //             object to the driver*/
    // #endif
}

void LinuxFramebufApplication::exit(int c)
{
    abort();
}

bool LinuxFramebufApplication::initApp()
{
    if (!Application::initApp()) {
        LV_LOG_WARN("Application init failed!");
    }
    framebufInit();
    return true;
}

void LinuxFramebufApplication::clearAppScreen()
{
    lv_area_t flush_area{ 0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX };
    // lv_color_t flush_buf[LV_HOR_RES_MAX*LV_VER_RES_MAX*4]={0};
    lv_color_t *flush_buf = new lv_color_t[LV_HOR_RES_MAX * LV_VER_RES_MAX * 4];
    memset(flush_buf, 0, sizeof(lv_coord_t) * (LV_HOR_RES_MAX * LV_VER_RES_MAX * 4));
    fbdev_flush(&disp_drv, &flush_area, flush_buf);
    delete[] flush_buf;
}

// void LinuxFramebufApplication::setPointerFeedbackCb(void (*mouseFeedback)(_lv_indev_drv_t *,
// uint8_t))
// {
//     if (mouseFeedback == nullptr)
//         return;

//     mouse_indev_->driver->feedback_cb = mouseFeedback;
//     lv_indev_drv_update(mouse_indev_, mouse_indev_->driver);
// }
#  endif
} // namespace sys
} // namespace lvglpp
#endif
