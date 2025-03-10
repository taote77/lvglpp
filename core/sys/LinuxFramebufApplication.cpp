/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-19

Class:${CLASS}

Description:

**************************************************************************/
#ifdef USED_FRAMEBUFF
#include "LinuxFramebufApplication.h"
#include "lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"

//#define DISP_BUF_SIZE (128 * 1024)
#define DISP_BUF_SIZE (LV_HOR_RES_MAX*LV_VER_RES_MAX*4)
namespace heygears
{
    namespace sys
    {
        LinuxFramebufApplication::LinuxFramebufApplication(int argc, char **argv) : Application(argc, argv),
        disp_drv()
        {
        }

        void LinuxFramebufApplication::framebufInit()
        {
            /*Linux frame buffer device init*/
            fbdev_init();

            /*A small buffer for LittlevGL to draw the screen's content*/
            static lv_color_t buf[DISP_BUF_SIZE],buf1[DISP_BUF_SIZE];

            /*Initialize a descriptor for the buffer*/
            static lv_disp_draw_buf_t disp_buf;
            lv_disp_draw_buf_init(&disp_buf, buf, buf1, DISP_BUF_SIZE);

            /*Initialize and register a display driver*/
            lv_disp_drv_init(&disp_drv);
            disp_drv.draw_buf = &disp_buf;
            disp_drv.flush_cb = fbdev_flush;
            disp_drv.hor_res = LV_HOR_RES_MAX;
            disp_drv.ver_res = LV_VER_RES_MAX;
            disp_drv.full_refresh=1;
            lv_disp_drv_register(&disp_drv);

            evdev_init();
            static lv_indev_drv_t indev_drv_1;
            lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
            indev_drv_1.type = LV_INDEV_TYPE_POINTER;

            /*This function will be called periodically (by the library) to get the mouse position and state*/
            indev_drv_1.read_cb = evdev_read;
            mouse_indev_ = lv_indev_drv_register(&indev_drv_1);

#if 0
            /*Set a cursor for the mouse*/
            LV_IMG_DECLARE(mouse_cursor_icon)
            lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
            lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
            lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
#endif
        }

        void LinuxFramebufApplication::exit(int c)
        {
            abort();
        }

        bool LinuxFramebufApplication::initApp()
        {
            if (!Application::initApp())
            {
                LV_LOG_WARN("Application init failed!");
            }
            framebufInit();
            return true;
        }

        void LinuxFramebufApplication::clearAppScreen()
        {
            lv_area_t flush_area{0,0,LV_HOR_RES_MAX,LV_VER_RES_MAX};
            //lv_color_t flush_buf[LV_HOR_RES_MAX*LV_VER_RES_MAX*4]={0};
            lv_color_t * flush_buf = new lv_color_t[LV_HOR_RES_MAX*LV_VER_RES_MAX*4];
            memset(flush_buf,0,sizeof(lv_coord_t)*(LV_HOR_RES_MAX*LV_VER_RES_MAX*4));
            fbdev_flush(&disp_drv,&flush_area, flush_buf);
            delete[] flush_buf;
        }

        void LinuxFramebufApplication::setPointerFeedbackCb(void (*mouseFeedback)(_lv_indev_drv_t *, uint8_t))
        {
            if(mouseFeedback == nullptr)
                return;

            mouse_indev_->driver->feedback_cb = mouseFeedback;
            lv_indev_drv_update(mouse_indev_, mouse_indev_->driver);
        }
    }
}
#endif


