/**************************************************************************


Description:一个基于SDL的lvgl应用程序类（调试时使用）

**************************************************************************/

#ifndef HEYGEARS_LVGL_DEMO_SDLSIMULATEAPPLICATION_H
#define HEYGEARS_LVGL_DEMO_SDLSIMULATEAPPLICATION_H
#if (defined(USED_SDL)) || (defined(USED_SDL_GPU))

#  include "Application.h"

namespace heygears {
namespace sys {
class SdlSimulateApplication : public Application
{
public:
    SdlSimulateApplication(int argc, char *argv[]);

    void setPointerFeedbackCb(void (*mouseFeedback)(_lv_indev_drv_t *, uint8_t));

    virtual void exit(int c) override;

    virtual bool initApp() override;

protected:
private:
    void halInit();

    lv_indev_t *mouse_indev_;
};
} // namespace sys
} // namespace heygears
#endif

#endif // HEYGEARS_LVGL_DEMO_SDLSIMULATEAPPLICATION_H
