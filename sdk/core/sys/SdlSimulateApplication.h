
#ifndef HEYGEARS_LVGL_DEMO_SDLSIMULATEAPPLICATION_H
#define HEYGEARS_LVGL_DEMO_SDLSIMULATEAPPLICATION_H

#if (defined(USED_SDL)) || (defined(USED_SDL_GPU))

#  include "Application.h"

namespace heygears::sys {
class SdlSimulateApplication : public Application
{
public:
    SdlSimulateApplication(int argc, char *argv[]);

    lv_display_t *getDisplay();
    /**
     * @brief Set the Theme object
     *
     * @param th 主题
     */
    void setTheme(lv_theme_t *theme);

    virtual void exit(int code) override;

    virtual bool initApp() override;

protected:
    void lv_linux_disp_init();

private:
    void halInit();

    lv_display_t *_display{ nullptr };

    lv_indev_t *_mouse_indev;
};
} // namespace heygears::sys

#endif

#endif // HEYGEARS_LVGL_DEMO_SDLSIMULATEAPPLICATION_H
