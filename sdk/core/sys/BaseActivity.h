/**************************************************************************

Class: BaseActivity

Description: Base implementation of an Activity with a root window

**************************************************************************/

#ifndef BASE_ACTIVITY_H
#define BASE_ACTIVITY_H

#include "Activity.h"
#include <memory>

namespace lvglpp::sys {

class BaseActivity : public Activity
{
public:
    BaseActivity() = default;

    ~BaseActivity() override;

    // Disable copy: copying would cause the window to be freed multiple times
    BaseActivity(const BaseActivity &) = delete;

    BaseActivity &operator=(const BaseActivity &) = delete;

    /**
     * @brief Called on first creation to build the UI
     * @param arg optional argument passed from the caller
     */
    void onCreate(std::any arg) override;

    /**
     * @brief Called when the activity becomes visible
     */
    void onResume() override;

    /**
     * @brief Called when the activity is no longer visible
     */
    void onPause() override;

    /**
     * @brief Called when the activity is being destroyed
     */
    void onDestroy() override;

    /// Receive notification events
    void onNotifyUI(const Event &e) override;

    /**
     * @brief Returns the root widget of this activity
     * @return pointer to the root BaseItem
     */
    widgets::BaseItem *getRoot() const override;

private:
    std::unique_ptr<widgets::BaseItem> _root_win;
};
} // namespace lvglpp::sys

#endif // BASE_ACTIVITY_H
