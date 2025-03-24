
#ifndef LVGLPP_DRM_APPLICATION_H
#define LVGLPP_DRM_APPLICATION_H

#ifdef USE_LINUX_DRM

#include "Application.h"

namespace heygears::sys
{
    class DrmApplication : public Application
    {
    public:
        DrmApplication(int argc, char *argv[]);

        virtual bool initApp() override;

        virtual void exit(int code) override;


        lv_display_t *getDisplay();
        /**
         * @brief Set the Theme object
         * 
         * @param th 主题
         */
        void
        setTheme(lv_theme_t *theme);

    private:
        lv_display_t *_display{nullptr};
    };

}// namespace heygears::sys

#endif
#endif// LVGLPP_DRM_APPLICATION_H
