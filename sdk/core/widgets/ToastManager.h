
#ifndef LV_TOAST_MANAGER_H
#define LV_TOAST_MANAGER_H
#include "Toast.h"
#include <memory>
#include <string>

namespace lvglpp {
namespace widgets {

class ToastManager
{
public:
    enum ToastLevel { Succeed, Error, Warn };
    static void showToast(const std::string &msg, ToastLevel level);

private:
    ToastManager();
    ToastManager(const ToastManager &) = default;
    std::shared_ptr<Toast> toast_impl_;
    static ToastManager   *instance_;
    lv_anim_t              anim_show_t_{};
    lv_anim_t              anim_hide_t_{};
    bool                   is_running_ = false;
};

} // namespace widgets
} // namespace lvglpp

#endif // LV_TOAST_MANAGER_H
