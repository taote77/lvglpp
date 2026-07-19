
#ifndef LVGLPP_DRM_APPLICATION_H
#define LVGLPP_DRM_APPLICATION_H

#include "../Application.h"

namespace lvglpp::sys {

class DrmApplication : public Application
{
public:
    DrmApplication(int argc, char *argv[]);

    ~DrmApplication();

    virtual bool initApp() override;

    virtual void exit(int code) override;

    lv_display_t *getDisplay();
    /**
     * @brief Set the Theme object
     *
     * @param th 主题
     */
    void setTheme(lv_theme_t *theme);

private:
    lv_display_t *_display{nullptr};
};

} // namespace lvglpp::sys

#endif // LVGLPP_DRM_APPLICATION_H
