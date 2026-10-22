#include "StackView.h"
#include "Activity.h"
#include "UiConfig.h"
#include <lvgl.h>
#include <utility>

namespace lvglpp::sys {

StackView &StackView::getInstance()
{
    // LVGL's SDL driver calls lv_deinit() + exit(0) on SDL_QUIT. A static
    // destructor here would then try to delete lv_objs after LVGL is gone.
    // The stack is process-lifetime; call shutdown() explicitly if needed.
    static StackView *instance = new StackView();
    return *instance;
}

void StackView::runOrQueue(std::function<void()> op)
{
    if (animating_)
    {
        pending_ops_.push_back(std::move(op));
        return;
    }
    op();
}

void StackView::processPendingOps()
{
    while (!animating_ && !pending_ops_.empty())
    {
        auto op = std::move(pending_ops_.front());
        pending_ops_.pop_front();
        op();
    }
}

void StackView::pushView(const std::shared_ptr<Activity> &view, std::any arg)
{
    if (!view)
    {
        return;
    }

    if (!working_)
    {
        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));
        return;
    }

    runOrQueue([this, view, arg = std::move(arg)]() mutable {
        if (!view_manager_.empty())
        {
            view_manager_.back()->onPause();
        }

        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));

        view_manager_.back()->onCreate(view_args_.back());
        view_manager_.back()->onResume();
        enterAnim(view_manager_.back().get());
    });
}

void StackView::popView()
{
    if (view_manager_.size() <= 1)
    {
        return;
    }

    if (!working_)
    {
        view_manager_.pop_back();
        if (!view_args_.empty())
        {
            view_args_.pop_back();
        }
        return;
    }

    runOrQueue([this]() {
        if (view_manager_.size() <= 1)
        {
            return;
        }
        animating_ = true;
        popAnim(view_manager_.back().get());
    });
}

void StackView::start()
{
    if (working_)
    {
        return;
    }

    working_ = true;

    // Views pushed before exec() were only queued. Create them bottom-up and
    // leave only the top one resumed.
    view_args_.resize(view_manager_.size());
    for (std::size_t i = 0; i < view_manager_.size(); ++i)
    {
        auto &view = view_manager_[i];
        view->onCreate(view_args_[i]);
        if (i + 1 < view_manager_.size())
        {
            view->onPause();
        } else
        {
            view->onResume();
        }
    }

    // 屏幕切换时不显示滚动条
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);
}

StackView::StackView()
{
    view_manager_.reserve(10);
    view_args_.reserve(10);
}

void StackView::enterAnim(const Activity *act)
{
    auto root_item = act->getRoot();
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
    lv_anim_set_time(&a_enter, enter_anim_ms_);
    lv_anim_set_exec_cb(&a_enter, [](void *obj, int32_t v) -> void {
        if (obj != nullptr)
        {
            lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)v);
        }
    });
#if LVGLPP_ENABLE_RTL
    const lv_coord_t enter_from = UiConfig::instance().isRtl() ? -lv_disp_get_hor_res(d) : lv_disp_get_hor_res(d);
#else
    const lv_coord_t enter_from = lv_disp_get_hor_res(d);
#endif
    lv_anim_set_values(&a_enter, enter_from, 0);
    lv_anim_start(&a_enter);
}

void StackView::popAnim(const Activity *act)
{
    auto root_item = act->getRoot();
    if (root_item == nullptr)
    {
        animating_ = false;
        processPendingOps();
        return;
    }
    auto d = lv_obj_get_disp(root_item->getLvglItem());
    if (d == nullptr)
    {
        animating_ = false;
        processPendingOps();
        return;
    }
    lv_anim_t a_pop;
    lv_anim_init(&a_pop);
    lv_anim_set_var(&a_pop, root_item->getLvglItem());
    lv_anim_set_user_data(&a_pop, this);
    lv_anim_set_ready_cb(&a_pop, [](struct _lv_anim_t *an_t) -> void {
        if (an_t->user_data != nullptr)
        {
            ((StackView *)an_t->user_data)->popViewAndRunCb();
        }
    });
    lv_anim_set_time(&a_pop, pop_anim_ms_);
    lv_anim_set_exec_cb(&a_pop, [](void *obj, int32_t v) -> void {
        if (obj != nullptr)
        {
            lv_obj_set_x((lv_obj_t *)obj, (lv_coord_t)v);
        }
    });
#if LVGLPP_ENABLE_RTL
    auto end_num = UiConfig::instance().isRtl() ? -lv_disp_get_hor_res(d) : lv_disp_get_hor_res(d);
#else
    auto end_num = lv_disp_get_hor_res(d);
#endif
    lv_anim_set_values(&a_pop, 0, end_num);
    lv_anim_start(&a_pop);
}

void StackView::popTop()
{
    if (view_manager_.size() <= 1)
    {
        animating_ = false;
        processPendingOps();
        return;
    }

    // Allow navigation from onPause/onResume callbacks while unwinding.
    animating_ = false;

    auto top = view_manager_.back();
    top->onPause();
    delete_view_manager_.push_back(top);

    view_manager_.pop_back();
    if (!view_args_.empty())
    {
        view_args_.pop_back();
    }

    view_manager_.back()->onResume();
    processPendingOps();
}

void StackView::popViewAndRunCb()
{
    popTop();
}

void StackView::pushViewImmediately(const std::shared_ptr<Activity> &view, std::any arg)
{
    if (!view)
    {
        return;
    }

    if (!working_)
    {
        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));
        return;
    }

    runOrQueue([this, view, arg = std::move(arg)]() mutable {
        if (!view_manager_.empty())
        {
            view_manager_.back()->onPause();
        }

        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));

        view_manager_.back()->onCreate(view_args_.back());
        view_manager_.back()->onResume();
    });
}

void StackView::popViewImmediately()
{
    if (view_manager_.size() <= 1)
    {
        return;
    }

    if (!working_)
    {
        view_manager_.pop_back();
        if (!view_args_.empty())
        {
            view_args_.pop_back();
        }
        return;
    }

    runOrQueue([this]() { popTop(); });
}

void StackView::pushViewAndReplaced(const std::shared_ptr<Activity> &view, std::any arg)
{
    if (!view)
    {
        return;
    }

    if (!working_)
    {
        if (!view_manager_.empty())
        {
            view_manager_.pop_back();
            if (!view_args_.empty())
            {
                view_args_.pop_back();
            }
        }
        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));
        return;
    }

    runOrQueue([this, view, arg = std::move(arg)]() mutable {
        if (!view_manager_.empty())
        {
            view_manager_.back()->onPause();
            delete_view_manager_.push_back(view_manager_.back());
            view_manager_.pop_back();
            if (!view_args_.empty())
            {
                view_args_.pop_back();
            }
        }

        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));

        view_manager_.back()->onCreate(view_args_.back());
        view_manager_.back()->onResume();
        enterAnim(view_manager_.back().get());
    });
}

void StackView::pushViewAndReplacedImmediately(const std::shared_ptr<Activity> &view, std::any arg)
{
    if (!view)
    {
        return;
    }

    if (!working_)
    {
        if (!view_manager_.empty())
        {
            view_manager_.pop_back();
            if (!view_args_.empty())
            {
                view_args_.pop_back();
            }
        }
        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));
        return;
    }

    runOrQueue([this, view, arg = std::move(arg)]() mutable {
        if (!view_manager_.empty())
        {
            view_manager_.back()->onPause();
            delete_view_manager_.push_back(view_manager_.back());
            view_manager_.pop_back();
            if (!view_args_.empty())
            {
                view_args_.pop_back();
            }
        }

        view_manager_.push_back(view);
        view_args_.push_back(std::move(arg));

        view_manager_.back()->onCreate(view_args_.back());
        view_manager_.back()->onResume();
    });
}

StackView::~StackView()
{
    shutdown();
}

void StackView::cancelAnimations(Activity *activity)
{
    if (activity == nullptr || activity->getRoot() == nullptr)
    {
        return;
    }
    if (activity->getRoot()->isAttached())
    {
        lv_anim_delete(activity->getRoot()->getLvglItem(), nullptr);
    }
}

void StackView::shutdown()
{
    working_   = false;
    animating_ = false;
    pending_ops_.clear();

    // A pending pop animation would otherwise fire after the root is destroyed.
    for (auto &activity : view_manager_)
    {
        cancelAnimations(activity.get());
    }
    for (auto &activity : delete_view_manager_)
    {
        cancelAnimations(activity.get());
    }

    // Call onDestroy on pending deleted views
    while (!delete_view_manager_.empty())
    {
        delete_view_manager_.back()->onDestroy();
        delete_view_manager_.pop_back();
    }

    // Call onPause + onDestroy on all active views (bottom-up)
    while (!view_manager_.empty())
    {
        auto act = view_manager_.back();
        view_manager_.pop_back();
        act->onPause();
        act->onDestroy();
    }

    view_args_.clear();
}

void StackView::notifyAllUi(const Event &e)
{
    for (auto &it : view_manager_)
    {
        it->onNotifyUI(e);
    }
}

bool StackView::isAnimating() const
{
    return animating_ || !pending_ops_.empty();
}

void StackView::cancelRootAnimations()
{
    for (auto &activity : view_manager_)
    {
        if (activity && activity->getRoot() != nullptr && activity->getRoot()->getLvglItem() != nullptr)
        {
            lv_anim_delete(activity->getRoot()->getLvglItem(), nullptr);
        }
    }
}

void StackView::notifyConfigurationChanged(const UiConfiguration &configuration)
{
    for (auto it = view_manager_.rbegin(); it != view_manager_.rend(); ++it)
    {
        if (*it)
        {
            (*it)->onConfigurationChanged(configuration);
        }
    }

    if (Activity *current = getCurrentActivity())
    {
        current->refreshFocus();
    }
}

void StackView::clearDeleteVec()
{
    if (delete_view_manager_.empty())
    {
        return;
    }

    while (!delete_view_manager_.empty())
    {
        LOG_DEBUG() << "clearDeleteVec()";
        delete_view_manager_.back()->onDestroy();
        delete_view_manager_.pop_back();
    }
}

} // namespace lvglpp::sys
