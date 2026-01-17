
#ifndef LV_TOAST_H
#define LV_TOAST_H

#include "ToastImpl.h"
#include <lvgl.h>
#include <memory>
#include <string>

namespace lvglpp::widgets {

class Toast
{
public:
    static void success(const std::string &msg);

    static void warn(const std::string &msg);

    static void error(const std::string &msg);

private:
    enum Type { Succeed, Error, Warn };

    static Toast &getInstance();

    Toast();
    Toast(const Toast &) = default;

    static void showToast(const std::string &info, Type type);

    std::unique_ptr<ToastImpl> toast_impl_;

    lv_anim_t anim_show_t_{};
    lv_anim_t anim_hide_t_{};
    bool      is_running_{false};
};

} // namespace lvglpp::widgets

#endif // LV_TOAST_H
