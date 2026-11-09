#include "StackView.h"
#include "Activity.h"
#include "InputManager.h"
#include "UiConfig.h"
#include <algorithm>
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

void StackView::finishTransition()
{
    animating_ = false;
    processPendingOps();
}

void StackView::pushEntry(const std::shared_ptr<Activity> &view, std::any arg, ResultHandler onResult)
{
    view_manager_.push_back(view);
    view_args_.push_back(std::move(arg));
    view_result_handlers_.push_back(std::move(onResult));
}

void StackView::popEntry()
{
    if (!view_manager_.empty())
    {
        view_manager_.pop_back();
    }
    if (!view_args_.empty())
    {
        view_args_.pop_back();
    }
    if (!view_result_handlers_.empty())
    {
        view_result_handlers_.pop_back();
    }
}

bool StackView::pushView(const std::shared_ptr<Activity> &view, std::any arg, ResultHandler onResult, LaunchMode mode)
{
    return pushViewNow(view, std::move(arg), std::move(onResult), mode, true);
}

bool StackView::pushViewImmediately(const std::shared_ptr<Activity> &view, std::any arg, ResultHandler onResult, LaunchMode mode)
{
    return pushViewNow(view, std::move(arg), std::move(onResult), mode, false);
}

bool StackView::pushViewNow(const std::shared_ptr<Activity> &view, std::any arg, ResultHandler onResult, LaunchMode mode, bool animate)
{
    if (!view)
    {
        return false;
    }

    if (!working_)
    {
        if (mode == LaunchMode::SingleTop && !view_manager_.empty() && view_manager_.back()->routeId() == view->routeId())
        {
            view_args_.back() = std::move(arg);
            return true;
        }

        if (mode == LaunchMode::ClearTop)
        {
            for (int i = static_cast<int>(view_manager_.size()) - 1; i >= 0; --i)
            {
                if (view_manager_[static_cast<std::size_t>(i)]->routeId() == view->routeId())
                {
                    view_manager_.resize(static_cast<std::size_t>(i) + 1);
                    view_args_.resize(static_cast<std::size_t>(i) + 1);
                    view_result_handlers_.resize(static_cast<std::size_t>(i) + 1);
                    view_args_[static_cast<std::size_t>(i)] = std::move(arg);
                    return true;
                }
            }
        }

        if (mode == LaunchMode::Replace && !view_manager_.empty())
        {
            popEntry();
        }

        pushEntry(view, std::move(arg), std::move(onResult));
        return true;
    }

    if (animating_)
    {
        pending_ops_.push_back(
            [this, view, arg = std::move(arg), onResult = std::move(onResult), mode, animate]() mutable { pushOrApply(view, std::move(arg), std::move(onResult), mode, animate); });
        return true;
    }

    return pushOrApply(view, std::move(arg), std::move(onResult), mode, animate);
}

bool StackView::pushOrApply(const std::shared_ptr<Activity> &view, std::any arg, ResultHandler onResult, LaunchMode mode, bool animate)
{
    const bool topMatches = !view_manager_.empty() && view_manager_.back()->routeId() == view->routeId();

    if (mode == LaunchMode::SingleTop && topMatches)
    {
        animating_ = true;
        view_manager_.back()->onNewArguments(std::move(arg));
        finishTransition();
        return true;
    }

    if (mode == LaunchMode::ClearTop)
    {
        int target = -1;
        for (int i = static_cast<int>(view_manager_.size()) - 1; i >= 0; --i)
        {
            if (view_manager_[static_cast<std::size_t>(i)]->routeId() == view->routeId())
            {
                target = i;
                break;
            }
        }

        if (target >= 0)
        {
            animating_              = true;
            const bool resumeTarget = target != static_cast<int>(view_manager_.size()) - 1;

            // Only the current top is resumed; entries below it are already
            // paused and must not receive a second onPause.
            bool first_removed = true;
            while (static_cast<int>(view_manager_.size()) - 1 > target)
            {
                auto activity = view_manager_.back();
                if (first_removed)
                {
                    activity->onPause();
                    first_removed = false;
                }
                delete_view_manager_.push_back(activity);
                popEntry();
            }
            view_manager_[static_cast<std::size_t>(target)]->onNewArguments(std::move(arg));
            if (resumeTarget)
            {
                view_manager_[static_cast<std::size_t>(target)]->onResume();
            }
            finishTransition();
            return true;
        }
        // No matching route: behave like Standard.
    }

    animating_ = true;

    if (mode == LaunchMode::Replace && !view_manager_.empty())
    {
        view_manager_.back()->onPause();
        delete_view_manager_.push_back(view_manager_.back());
        popEntry();
    } else if (!view_manager_.empty())
    {
        view_manager_.back()->onPause();
    }

    pushEntry(view, std::move(arg), std::move(onResult));
    view_manager_.back()->onCreate(view_args_.back());
    view_manager_.back()->onResume();

    if (animate)
    {
        enterAnim(view_manager_.back().get());
    } else
    {
        finishTransition();
    }
    return true;
}

bool StackView::popView(std::any result)
{
    if (view_manager_.size() <= 1)
    {
        return false;
    }

    if (!working_)
    {
        popEntry();
        return true;
    }

    if (animating_)
    {
        pending_ops_.push_back([this, result = std::move(result)]() mutable { popOrApply(std::move(result)); });
        return true;
    }

    return popOrApply(std::move(result));
}

bool StackView::popOrApply(std::any result)
{
    if (view_manager_.size() <= 1)
    {
        finishTransition();
        return false;
    }

    animating_          = true;
    pending_pop_result_ = std::move(result);
    popAnim(view_manager_.back().get());
    return true;
}

bool StackView::popViewImmediately(std::any result)
{
    if (view_manager_.size() <= 1)
    {
        return false;
    }

    if (!working_)
    {
        popEntry();
        return true;
    }

    if (animating_)
    {
        pending_ops_.push_back([this, result = std::move(result)]() mutable { popTop(false, std::move(result)); });
        return true;
    }

    popTop(false, std::move(result));
    return true;
}

bool StackView::popTo(int32_t depth)
{
    const int32_t target = std::max<int32_t>(1, depth);
    if (static_cast<int32_t>(view_manager_.size()) <= target)
    {
        return false;
    }

    if (!working_)
    {
        while (static_cast<int32_t>(view_manager_.size()) > target)
        {
            popEntry();
        }
        return true;
    }

    if (animating_)
    {
        pending_ops_.push_back([this, target]() { popTo(target); });
        return true;
    }

    animating_         = true;
    bool first_removed = true;
    while (static_cast<int32_t>(view_manager_.size()) > target)
    {
        auto activity = view_manager_.back();
        if (first_removed)
        {
            activity->onPause();
            first_removed = false;
        }
        delete_view_manager_.push_back(activity);
        popEntry();
    }
    view_manager_.back()->onResume();
    finishTransition();
    return true;
}

bool StackView::popToRoot()
{
    return popTo(1);
}

void StackView::start()
{
    if (working_)
    {
        return;
    }

    working_   = true;
    animating_ = true;

    // Views pushed before exec() were only queued. Create them bottom-up and
    // leave only the top one resumed.
    view_args_.resize(view_manager_.size());
    view_result_handlers_.resize(view_manager_.size());
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

    back_token_ = InputManager::getInstance().addBackHandler(BackPriority::Activity, [this]() {
        Activity *current = getCurrentActivity();
        if (current != nullptr && current->onBackPressed())
        {
            return true;
        }
        if (depth() > 1)
        {
            popView();
            return true;
        }
        return false;
    });

    // 屏幕切换时不显示滚动条
    lv_obj_set_scrollbar_mode(lv_scr_act(), LV_SCROLLBAR_MODE_OFF);

    animating_ = false;
    processPendingOps();
}

StackView::StackView()
{
    view_manager_.reserve(10);
    view_args_.reserve(10);
    view_result_handlers_.reserve(10);
}

void StackView::enterAnim(const Activity *act)
{
    auto root_item = act->getRoot();
    if (root_item == nullptr || root_item->lvObj() == nullptr)
    {
        finishTransition();
        return;
    }
    auto d = lv_obj_get_disp(root_item->lvObj());
    if (d == nullptr)
    {
        finishTransition();
        return;
    }

    lv_anim_t a_enter;
    lv_anim_init(&a_enter);
    lv_anim_set_var(&a_enter, root_item->lvObj());
    lv_anim_set_user_data(&a_enter, this);
    lv_anim_set_ready_cb(&a_enter, [](struct _lv_anim_t *anim) -> void {
        if (anim != nullptr && anim->user_data != nullptr)
        {
            static_cast<StackView *>(anim->user_data)->finishTransition();
        }
    });
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
    if (root_item == nullptr || root_item->lvObj() == nullptr)
    {
        popViewAndRunCb();
        return;
    }
    auto d = lv_obj_get_disp(root_item->lvObj());
    if (d == nullptr)
    {
        popViewAndRunCb();
        return;
    }
    lv_anim_t a_pop;
    lv_anim_init(&a_pop);
    lv_anim_set_var(&a_pop, root_item->lvObj());
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

void StackView::popTop(bool animate, std::any result)
{
    (void)animate; // animation decision is made by the caller

    if (view_manager_.size() <= 1)
    {
        finishTransition();
        return;
    }

    // Keep the transition guard for the whole unwind. Navigation from
    // onPause/onResume or the result handler is queued and executed after the
    // stack is consistent.
    animating_ = true;

    auto top = view_manager_.back();
    top->onPause();
    delete_view_manager_.push_back(top);

    ResultHandler handler = view_result_handlers_.back();
    popEntry();

    view_manager_.back()->onResume();

    if (handler)
    {
        handler(std::move(result));
    }

    pending_pop_result_.reset();
    finishTransition();
}

void StackView::popViewAndRunCb()
{
    std::any result = std::move(pending_pop_result_);
    pending_pop_result_.reset();
    popTop(false, std::move(result));
}

bool StackView::pushViewAndReplaced(const std::shared_ptr<Activity> &view, std::any arg)
{
    return pushViewNow(view, std::move(arg), {}, LaunchMode::Replace, true);
}

bool StackView::pushViewAndReplacedImmediately(const std::shared_ptr<Activity> &view, std::any arg)
{
    return pushViewNow(view, std::move(arg), {}, LaunchMode::Replace, false);
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
        lv_anim_delete(activity->getRoot()->lvObj(), nullptr);
    }
}

void StackView::shutdown()
{
    working_   = false;
    animating_ = false;
    pending_ops_.clear();
    pending_pop_result_.reset();

    if (back_token_ != 0)
    {
        InputManager::getInstance().removeBackHandler(back_token_);
        back_token_ = 0;
    }

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
        popEntry();
        act->onPause();
        act->onDestroy();
    }

    view_args_.clear();
    view_result_handlers_.clear();
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
        if (activity && activity->getRoot() != nullptr && activity->getRoot()->lvObj() != nullptr)
        {
            lv_anim_delete(activity->getRoot()->lvObj(), nullptr);
        }
    }

    // A canceled transition will never run its ready callback. Drop queued
    // navigation as well: the configuration rebuild invalidates those targets.
    pending_ops_.clear();
    pending_pop_result_.reset();
    animating_ = false;
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
