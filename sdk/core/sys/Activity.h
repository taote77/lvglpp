/**************************************************************************

Description:定义不同活动的窗口。类似Android
                用Navigators压入新页面

**************************************************************************/

#ifndef LVGLPP_ACTIVITY_H
#define LVGLPP_ACTIVITY_H

#include "Event.h"
#include "core/widgets/BaseItem.h"
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

    virtual widgets::BaseItem *getRoot() const = 0;

    virtual void onNotifyUI(const Event &e) = 0;
};
} // namespace sys
} // namespace lvglpp

#endif // LVGLPP_ACTIVITY_H
