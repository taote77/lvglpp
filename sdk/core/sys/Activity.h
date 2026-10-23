/**************************************************************************

Description:定义不同活动的窗口。类似Android
                用Navigators压入新页面

**************************************************************************/

#ifndef LVGLPP_ACTIVITY_H
#define LVGLPP_ACTIVITY_H

#include "Event.h"
#include "UiTypes.h"
#include "core/widgets/Widget.h"
#include <any>

namespace lvglpp {
namespace sys {
class Activity
{
public:
    virtual ~Activity() = default;

    // protected:
    virtual void onCreate(std::any arg) = 0;

    virtual void onResume() = 0;

    virtual void onPause() = 0;

    virtual void onDestroy() = 0;

    virtual widgets::Widget *getRoot() const = 0;

    virtual void onNotifyUI(const Event &e) = 0;

    /**
     * Called after a display orientation or layout-direction change.
     * The activity root has already been resized; rebuild content here.
     */
    virtual void onConfigurationChanged(const UiConfiguration &configuration)
    {
        (void)configuration;
    }

    /// Rebuild the keyboard focus group after a rebuild/visibility change.
    virtual void refreshFocus() {}
};
} // namespace sys
} // namespace lvglpp

#endif // LVGLPP_ACTIVITY_H
