
#ifndef LVGLPP_TASK_STACK_H
#define LVGLPP_TASK_STACK_H

#include "Activity.h"
#include "Application.h"
#include "Event.h"
#include "UiTypes.h"
#include <any>
#include <deque>
#include <functional>
#include <memory>
#include <vector>

namespace lvglpp::sys {

class StackView
{
public:
    StackView(const StackView &) = delete;

    StackView &operator=(const StackView &) = delete;

    ~StackView();

    static StackView &getInstance();

    void start();

    void pushView(const std::shared_ptr<Activity> &view, std::any arg = {});

    void pushViewAndReplaced(const std::shared_ptr<Activity> &view, std::any arg = {});

    void pushViewAndReplacedImmediately(const std::shared_ptr<Activity> &view, std::any arg = {});

    void popView();

    /**
     * 直接退出（没有动画）
     */
    void popViewImmediately();

    void pushViewImmediately(const std::shared_ptr<Activity> &view, std::any arg = {});

    int32_t depth() const
    {
        return view_manager_.size();
    }

    Activity *getCurrentActivity() const
    {
        if (view_manager_.empty())
        {
            return nullptr;
        } else
        {
            return view_manager_[view_manager_.size() - 1].get();
        }
    }

    void notifyAllUi(const Event &e);

    /// True while a push/pop transition or queued navigation is in flight.
    bool isAnimating() const;

    /// Stop any root-position animations (used before a configuration rebuild).
    void cancelRootAnimations();

    /// Deliver a configuration change to every activity, then refresh focus.
    void notifyConfigurationChanged(const UiConfiguration &configuration);

    void setApplication(Application *app)
    {
        app_ = app;
    }

    Application *getApplication() const
    {
        return app_;
    }

    void clearDeleteVec();

    /// Gracefully shut down all views: call onDestroy on every activity
    void shutdown();

protected:
    /**
     * 页面进入动画
     * @param act
     */
    virtual void enterAnim(const Activity *act);

    /**
     *
     * @param act
     */
    virtual void popAnim(const Activity *act);

private:
    bool                                   working_       = false;
    bool                                   animating_     = false;
    const int                              enter_anim_ms_ = 250;
    const int                              pop_anim_ms_   = 250;
    std::vector<std::shared_ptr<Activity>> view_manager_;
    std::vector<std::any>                  view_args_;
    std::vector<std::shared_ptr<Activity>> delete_view_manager_;
    std::deque<std::function<void()>>      pending_ops_;
    Application                           *app_ = nullptr;

    StackView();

    /// Run immediately, or queue while a pop transition is in flight.
    void runOrQueue(std::function<void()> op);

    void processPendingOps();

    void popTop();

    void cancelAnimations(Activity *activity);

    void popViewAndRunCb();
};

} // namespace lvglpp::sys

#endif // LVGLPP_STACK_VIEW_H
