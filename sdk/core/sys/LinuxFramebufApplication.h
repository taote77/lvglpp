
#ifndef LINUXFRAMEBUF_APPLICATION_H
#define LINUXFRAMEBUF_APPLICATION_H

#ifdef USED_FRAMEBUFF

#include "Application.h"
#include "lvgl.h"

namespace lvglpp {
namespace sys {
class LinuxFramebufApplication : public Application
{
public:
    LinuxFramebufApplication(int argc, char *argv[]);

    void clearAppScreen();

    // void setPointerFeedbackCb(void (*mouseFeedback)(lv_indev_drv_t *, uint8_t));

    virtual void exit(int c) override;

    virtual bool initApp() override;

protected:
private:
    void          framebufInit();
    lv_display_t *_disp;

    // lv_indev_t *mouse_indev_;
};
} // namespace sys
} // namespace lvglpp
#endif

#endif // LINUXFRAMEBUF_APPLICATION_H
