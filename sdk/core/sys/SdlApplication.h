
#ifndef SDL_APPLICATION_H
#define SDL_APPLICATION_H

#if (defined(USED_SDL)) || (defined(USED_SDL_GPU))

#include "Application.h"

namespace lvglpp::sys {

/**
 * @brief SDL模拟程序
 * @note facade Pattern, 隐藏底层复杂的初始化流程
         包括lvgl 渲染，驱动注册，定时器, png处理, 字体等
 */
class SdlApplication : public Application
{
public:
    SdlApplication(int argc, char *argv[]);

    lv_display_t *getDisplay();
    /**
     * @brief Set the Theme object
     *
     * @param th 主题
     */
    void setTheme(lv_theme_t *theme);

    virtual void exit(int code) override;

    virtual bool initApp() override;

    static void simulate_click_at(int32_t x, int32_t y);

    static void simulate_click_at(lv_obj_t *obj, int32_t x, int32_t y);

protected:
    void lv_linux_disp_init();

private:
    void halInit();

    lv_display_t *_display{nullptr};

    lv_indev_t *_mouse_indev{nullptr};

    lv_indev_t *_keybaord_indev{nullptr};
};
} // namespace lvglpp::sys

#endif

#endif // SDL_APPLICATION_H
