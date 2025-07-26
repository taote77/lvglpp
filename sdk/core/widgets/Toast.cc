
#include "Toast.h"
#include "core/log/log.h"

namespace lvglpp {
namespace widgets {

// Toast *Toast::instance_ = nullptr;
void Toast::showToast(const std::string &msg, Toast::Type level)
{
    getInstance().toast_impl_->setMessage(msg, (ToastImpl::IconType)level);
    if (!getInstance().is_running_)
    {
        getInstance().toast_impl_->setOpacity(0);
        getInstance().toast_impl_->setVisible(true);
        lv_anim_start(&getInstance().anim_show_t_);
        getInstance().is_running_ = true;
        LogDebug << "show toast:-- " << msg << " --";
    } else
    {
        LogWarn << "toast is running!not show:-- " << msg << " --";
    }
}

Toast &Toast::getInstance()
{
    static Toast instance;
    return instance;
}

Toast::Toast()
{
    toast_impl_ = std::make_shared<ToastImpl>();
    toast_impl_->setVisible(false);
    lv_anim_init(&anim_show_t_);
    lv_anim_set_exec_cb(&anim_show_t_, [](void *p, int32_t progress) -> void {
        if (p != nullptr)
        {
            ((Toast *)p)->toast_impl_->setOpacity(progress / 100.0);
        }
    });
    lv_anim_set_values(&anim_show_t_, 0, 100);
    lv_anim_set_time(&anim_show_t_, 300);
    lv_anim_set_path_cb(&anim_show_t_, lv_anim_path_ease_out);
    lv_anim_set_var(&anim_show_t_, this);
    lv_anim_set_ready_cb(&anim_show_t_, [](struct _lv_anim_t *anim_t) -> void {
        lv_timer_set_repeat_count(lv_timer_create(
                                      [](lv_timer_t *t) -> void {
                                          auto user_data = lv_timer_get_user_data(t);
                                          if (user_data != nullptr)
                                          {
                                              auto p = (Toast *)user_data;
                                              lv_anim_start(&p->anim_hide_t_);
                                          }
                                      },
                                      3000, anim_t->var),
                                  1);
    });

    lv_anim_init(&anim_hide_t_);
    lv_anim_set_exec_cb(&anim_hide_t_, [](void *p, int32_t progress) -> void {
        if (p != nullptr)
        {
            ((Toast *)p)->toast_impl_->setOpacity((100 - progress) / 100.0);
        }
    });
    lv_anim_set_ready_cb(&anim_hide_t_, [](struct _lv_anim_t *t) -> void {
        ((Toast *)t->var)->toast_impl_->setVisible(false);
        ((Toast *)t->var)->is_running_ = false;
    });
    lv_anim_set_values(&anim_hide_t_, 0, 100);
    lv_anim_set_time(&anim_hide_t_, 300);
    lv_anim_set_path_cb(&anim_hide_t_, lv_anim_path_ease_out);
    lv_anim_set_var(&anim_hide_t_, this);
}
} // namespace widgets
} // namespace lvglpp
