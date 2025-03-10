/**************************************************************************

Copyright:HeyGears

Author: LnJan

Date:2022-03-17

Class:${CLASS}

Description:

**************************************************************************/
#if (defined(USED_SDL)) || (defined(USED_SDL_GPU))

#include "SdlSimulateApplication.h"
#include <lvgl.h>

#if defined(USED_SDL)
#include <sdl/sdl.h>
#elif defined(USED_SDL_GPU)
#include <sdl/sdl_gpu.h>
#endif

#include <unistd.h>

namespace heygears
{
    namespace sys
    {
        SdlSimulateApplication::SdlSimulateApplication(int argc, char **argv) : Application(argc, argv)
        {

        }

        bool SdlSimulateApplication::initApp()
        {
            if (!Application::initApp())
            {
                LV_LOG_WARN("Application init failed!");
            }
            halInit();
            return true;
        }

        void SdlSimulateApplication::exit(int c)
        {
            quick_exit(c);
        }

        /**
          * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
          * library
        */
        void SdlSimulateApplication::halInit()
        {
            /* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
            sdl_init();

            /*Create a display buffer*/
            static lv_disp_draw_buf_t disp_buf1;
            static lv_color_t buf1_1[SDL_HOR_RES * 100];

#if defined(USED_SDL)
            lv_disp_draw_buf_init(&disp_buf1, buf1_1, NULL, SDL_HOR_RES * 100);
#elif defined(USED_SDL_GPU)
            sdl_gpu_disp_draw_buf_init(&disp_buf1);
#endif

            /*Create a display*/
            static lv_disp_drv_t disp_drv;
#if defined(USED_SDL)
            lv_disp_drv_init(&disp_drv); /*Basic initialization*/
#elif defined(USED_SDL_GPU)
            sdl_gpu_disp_drv_init(&disp_drv);
#endif

            disp_drv.draw_buf = &disp_buf1;
            disp_drv.flush_cb = sdl_display_flush;
            disp_drv.hor_res = SDL_HOR_RES;
            disp_drv.ver_res = SDL_VER_RES;

            lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

            lv_theme_t *th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                                                   lv_palette_main(LV_PALETTE_RED),
                                                   LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
            lv_disp_set_theme(disp, th);

            lv_group_t *g = lv_group_create();
            lv_group_set_default(g);

            /* Add the mouse as input device
             * Use the 'mouse' driver which reads the PC's mouse*/
            static lv_indev_drv_t indev_drv_1;
            lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
            indev_drv_1.type = LV_INDEV_TYPE_POINTER;

            /*This function will be called periodically (by the library) to get the mouse position and state*/
            indev_drv_1.read_cb = sdl_mouse_read;
            mouse_indev_ = lv_indev_drv_register(&indev_drv_1);

            static lv_indev_drv_t indev_drv_2;
            lv_indev_drv_init(&indev_drv_2); /*Basic initialization*/
            indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
            indev_drv_2.read_cb = sdl_keyboard_read;
            lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
            lv_indev_set_group(kb_indev, g);

            static lv_indev_drv_t indev_drv_3;
            lv_indev_drv_init(&indev_drv_3); /*Basic initialization*/
            indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
            indev_drv_3.read_cb = sdl_mousewheel_read;
            lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_3);
            lv_indev_set_group(enc_indev, g);

#if 0
            /*Set a cursor for the mouse*/
        LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
        lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
        lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
        lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
#endif
        }

        void SdlSimulateApplication::setPointerFeedbackCb(void (*mouseFeedback)(_lv_indev_drv_t *, uint8_t))
        {
            if(mouseFeedback == nullptr)
                return;

            mouse_indev_->driver->feedback_cb = mouseFeedback;
            lv_indev_drv_update(mouse_indev_, mouse_indev_->driver);
        }
    }
}
#endif
