/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-19

Class:LinuxFramebufApplication

Description:

**************************************************************************/

#ifndef LVGL_XFJ_DEMO_LINUXFRAMEBUFAPPLICATION_H
#define LVGL_XFJ_DEMO_LINUXFRAMEBUFAPPLICATION_H
#ifdef USED_FRAMEBUFF

#include "Application.h"

namespace heygears
{
    namespace sys
    {
        class LinuxFramebufApplication : public Application
        {
        public:
            LinuxFramebufApplication(int argc, char *argv[]);

            void clearAppScreen();

            void setPointerFeedbackCb(void (*mouseFeedback)(_lv_indev_drv_t *, uint8_t));

            virtual void exit(int c) override;

            virtual bool initApp() override;
        protected:

        private:
            void framebufInit();
            lv_disp_drv_t disp_drv;

            lv_indev_t *mouse_indev_;
        };
    }
}
#endif

#endif //LVGL_XFJ_DEMO_LINUXFRAMEBUFAPPLICATION_H
