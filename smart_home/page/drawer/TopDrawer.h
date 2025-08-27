
#ifndef ULTRACORE_REFLEX_PRO_DRAWER_H
#define ULTRACORE_REFLEX_PRO_DRAWER_H

#include "MessageCenterControl.h"
#include "ModuleCollector.h"
#include "SwipeView.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/MouseArea.h"
#include "core/widgets/PageIndicator.h"
#include <boost/shared_ptr.hpp>

namespace lvglpp::ui::page {
using namespace widgets;

class TopDrawer : public BaseItem
{
public:
    explicit TopDrawer(BaseItem *parent = nullptr);

    void updateDeviceInfo(void *data);

    // type 0-wifi，1-cloud, 2-udisk
    void conStateChange(int type, bool on);

    void hide();

    void show();

private:
    void changePagePos(int ypos);

    void pageEnterAnim();

    void pagePopAnim();

private:
    std::shared_ptr<BaseItem>  bg_item_;
    std::shared_ptr<MouseArea> touch_area_;

    std::shared_ptr<SwipeView> _page_container;

    std::shared_ptr<widgets::BaseItem> _ctrl_panel;

    std::shared_ptr<ModuleCollector> _mod_panel;

    std::shared_ptr<MessageCenterControl> _notice_panel;

    std::shared_ptr<widgets::PageIndicator> _page_indicator;
};

} // namespace lvglpp::ui::page

#endif // ULTRACORE_REFLEX_PRO_DRAWER_H
