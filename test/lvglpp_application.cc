
#include "lvglpp_application.h"
#include "translations/lv_i18n.h"

#include <unistd.h>

constexpr bool fullscreen = false;
constexpr bool maximize = false;

/**********************
 *  STATIC VARIABLES
 **********************/
// static lv_obj_t *main_cont;
#if LV_USE_LOTTIE
#  if LV_DEMO_EBIKE_PORTRAIT
LV_DRAW_BUF_DEFINE_STATIC(lottie_ebike_draw_buf, 120, 120, LV_COLOR_FORMAT_ARGB8888);
#  else
LV_DRAW_BUF_DEFINE_STATIC(lottie_ebike_draw_buf, 100, 100, LV_COLOR_FORMAT_ARGB8888);
#  endif
#endif

// static lv_theme_t *theme_original;

/*The language subject is globally available*/
// lv_subject_t ebike_subject_language;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

// static void language_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
// {
//     LV_UNUSED(observer);

//     static uint32_t lang_current = 0;
//     uint32_t lang_new = lv_subject_get_int(subject);
//     if (lang_new != lang_current) {
//         lang_current = lang_new;
//         switch (lang_current) {
//         case 0:
//             lv_i18n_set_locale("en");
//             break;
//         case 1:
//             lv_i18n_set_locale("zh");
//             break;
//         case 2:
//             lv_i18n_set_locale("ar");
//             break;
//         }
//         lv_obj_clean(main_cont);
//         lv_demo_ebike_settings_create(main_cont);
//     }
// }

LvglApplication::LvglApplication(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    lv_init();

    lv_linux_disp_init();

    lv_i18n_init(lv_i18n_language_pack);

    _main_container = lv_obj_create(lv_screen_active());
#if LV_USE_LOTTIE
    LV_DRAW_BUF_INIT_STATIC(lottie_ebike_draw_buf);
#endif

    // lv_subject_init_int(&ebike_subject_language, 0);
    // lv_subject_add_observer_obj(&ebike_subject_language, language_observer_cb,
    // lv_screen_active(),
    //                             NULL);
}

LvglApplication::~LvglApplication()
{

    ///
    ///
}

lv_obj_t *LvglApplication::get_main_cont(void)
{
    return _main_container;
}

int LvglApplication::exec()
{
    lv_linux_run_loop();

    return 0;
}

void LvglApplication::lv_linux_run_loop(void)
{

    bool completed;

    /* Handle LVGL tasks */
    while (1) {

        completed = lv_wayland_timer_handler();

        if (completed) {
            /* wait only if the cycle was completed */
            usleep(LV_DEF_REFR_PERIOD * 1000);
        }

        /* Run until the last window closes */
        if (!lv_wayland_window_is_open(NULL)) {
            break;
        }
    }
}

void LvglApplication::lv_linux_disp_init(void)
{
    lv_display_t *disp;
    lv_group_t *g;

    char *win_title = (char *)"LVGL Simulator";
    disp = lv_wayland_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, win_title, NULL);

    if (fullscreen) {
        lv_wayland_window_set_fullscreen(disp, fullscreen);
    } else if (maximize) {
        lv_wayland_window_set_maximized(disp, maximize);
    }

    g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(lv_wayland_get_keyboard(disp), g);
    lv_indev_set_group(lv_wayland_get_pointeraxis(disp), g);
}
