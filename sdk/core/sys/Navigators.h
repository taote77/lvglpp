
#ifndef LVGLPP_NAVIGATORS_H
#define LVGLPP_NAVIGATORS_H

#include <memory>
#include <vector>
#include "Activity.h"
#include "Event.h"
#include "Application.h"

namespace lvglpp {
namespace sys {
class Navigators
{
public:
    Navigators(const Navigators &) = delete;

    Navigators &operator=(const Navigators &) = delete;

    ~Navigators();

    static Navigators *getInstance();

    void start();

    void pushView(const std::shared_ptr<Activity> &view, void *arg = nullptr);

    void pushViewAndReplaced(const std::shared_ptr<Activity> &view, void *arg = nullptr);

    void pushViewAndReplacedImmediately(const std::shared_ptr<Activity> &view, void *arg = nullptr);

    void popView();

    /**
     * 直接退出（没有动画）
     */
    void popViewImmediately();

    void pushViewImmediately(const std::shared_ptr<Activity> &view, void *arg = nullptr);

    int32_t depth() const { return view_manager_.size(); }

    Activity *getCurrentActivity() const
    {
        if (view_manager_.empty()) {
            return nullptr;
        } else {
            return view_manager_[view_manager_.size() - 1].get();
        }
    }

    void notifyAllUi(const Event &e);

    void setApplication(Application *app) { app_ = app; }

    Application *getApplication() const { return app_; }

    void clearDeleteVec();

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
    static Navigators *instance_;
    bool working_ = false;
    const int enter_anim_ms_ = 250;
    const int pop_anim_ms_ = 250;
    std::vector<std::shared_ptr<Activity>> view_manager_;
    std::vector<std::shared_ptr<Activity>> delete_view_manager_;
    Application *app_ = nullptr;

    Navigators();

    void popViewAndRunCb();
};
} // namespace sys
} // namespace lvglpp

#endif // LVGLPP_NAVIGATORS_H
