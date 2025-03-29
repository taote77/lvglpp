
#include "Navigators.h"
#include "Activity.h"
#include <iostream>
#include <lvgl.h>

namespace lvglpp {
namespace sys {
Navigators *Navigators::instance_ = nullptr;

Navigators *Navigators::getInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = new Navigators();
    }
    return instance_;
}

void Navigators::pushView(const std::shared_ptr<Activity> &view, void *arg)
{
    if (working_)
    {
        if (!view_manager_.empty())
        {
            view_manager_.back()->onPause();
        }
        view_manager_.push_back(view);
        view_manager_.back()->onCreate(arg);
        view_manager_.back()->onResume();
        enterAnim(view_manager_.back().get());

    } else
    {
        view_manager_.push_back(view);
    }
}

void Navigators::popView()
{
    if (view_manager_.size() <= 1)
    {
        return;
    }
    if (working_)
    {
        popAnim(view_manager_.back().get());
    } else
    {
        view_manager_.pop_back();
    }
}

void Navigators::start()
{
    if (!view_manager_.empty())
    {
        view_manager_.back()->onCreate(nullptr);
        view_manager_.back()->onResume();
    }
    // 屏幕切换时不显示滚动条
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);
    working_ = true;
}

Navigators::Navigators()
{
    view_manager_.reserve(10);
}

void Navigators::enterAnim(const Activity *act)
{
    auto root_item = act->getRootItem();
    if (root_item == nullptr)
    {
        return;
    }
    auto d = lv_obj_get_disp(root_item->getLvglItem());
    if (d == nullptr)
    {
        return;
    }
    lv_anim_t a_enter;
    lv_anim_init(&a_enter);
    lv_anim_set_var(&a_enter, root_item->getLvglItem());
    lv_anim_set_ready_cb(&a_enter, [](struct _lv_anim_t *) -> void {
        // hDebug() << "enter anim finished!!";
    });
    lv_anim_set_time(&a_enter, enter_anim_ms_);
    lv_anim_set_exec_cb(&a_enter, [](void *obj, int32_t v) -> void {
        if (obj != nullptr)
        {
            lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)v);
        }
    });
    lv_anim_set_values(&a_enter, lv_disp_get_hor_res(d), 0);
    lv_anim_start(&a_enter);
}

void Navigators::popAnim(const Activity *act)
{
    auto root_item = act->getRootItem();
    if (root_item == nullptr)
    {
        return;
    }
    auto d = lv_obj_get_disp(root_item->getLvglItem());
    if (d == nullptr)
    {
        return;
    }
    lv_anim_t a_pop;
    lv_anim_init(&a_pop);
    lv_anim_set_var(&a_pop, root_item->getLvglItem());
    lv_anim_set_user_data(&a_pop, this);
    lv_anim_set_ready_cb(&a_pop, [](struct _lv_anim_t *an_t) -> void {
        // hDebug() << "pop anim finished!!";
        if (an_t->user_data != nullptr)
        {
            ((Navigators *)an_t->user_data)->popViewAndRunCb();
        }
    });
    lv_anim_set_time(&a_pop, pop_anim_ms_);
    lv_anim_set_exec_cb(&a_pop, [](void *obj, int32_t v) -> void {
        if (obj != nullptr)
        {
            lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)v);
        }
    });
    auto end_num = lv_disp_get_hor_res(d);
    lv_anim_set_values(&a_pop, 0, end_num);
    lv_anim_start(&a_pop);
}

void Navigators::popViewAndRunCb()
{
    view_manager_.back()->onPause();
    delete_view_manager_.push_back(view_manager_.back());
    view_manager_.pop_back();
    view_manager_.back()->onResume();
}

void Navigators::pushViewImmediately(const std::shared_ptr<Activity> &view, void *arg)
{
    if (view_manager_.size() >= 1)
    {
        view_manager_.back()->onPause();
    }
    view_manager_.push_back(view);
    view_manager_.back()->onCreate(arg);
    view_manager_.back()->onResume();
}

void Navigators::popViewImmediately()
{
    if (view_manager_.size() <= 1)
    {
        return;
    }
    view_manager_.back()->onPause();
    delete_view_manager_.push_back(view_manager_.back());
    view_manager_.pop_back();
    view_manager_.back()->onResume();
}

void Navigators::pushViewAndReplaced(const std::shared_ptr<Activity> &view, void *arg)
{
    if (!working_)
    {
        return;
    }
    if (!view_manager_.empty())
    {
        view_manager_.back()->onPause();
        delete_view_manager_.push_back(view_manager_.back());
        view_manager_.pop_back();
    }

    view_manager_.push_back(view);
    view_manager_.back()->onCreate(arg);
    view_manager_.back()->onResume();
    enterAnim(view_manager_.back().get());
}

void Navigators::pushViewAndReplacedImmediately(const std::shared_ptr<Activity> &view, void *arg)
{
    if (!working_)
    {
        return;
    }
    if (!view_manager_.empty())
    {
        view_manager_.back()->onPause();
        delete_view_manager_.push_back(view_manager_.back());
        view_manager_.pop_back();
    }

    view_manager_.push_back(view);
    view_manager_.back()->onCreate(arg);
    view_manager_.back()->onResume();
}

Navigators::~Navigators() = default;

void Navigators::notifyAllUi(const Event &e)
{
    for (auto &it : view_manager_)
    {
        it->onNotifyUI(e);
    }
}

void Navigators::clearDeleteVec()
{
    if (delete_view_manager_.empty())
        return;

    while (delete_view_manager_.size() > 0)
    {
        LOG_DEBUG() << "clearDeleteVec()";
        delete_view_manager_.back()->onDestroy();
        delete_view_manager_.pop_back();
    }
}
} // namespace sys
} // namespace lvglpp
