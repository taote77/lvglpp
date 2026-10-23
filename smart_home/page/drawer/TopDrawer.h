
#ifndef TOP_DRAWER_H
#define TOP_DRAWER_H

#include "MessageCenterControl.h"
#include "ModuleCollector.h"
#include "SwipeView.h"
#include "core/widgets/Widget.h"
#include "core/widgets/MouseArea.h"
#include "core/widgets/PageIndicator.h"

namespace lvglpp::ui::page {
using namespace widgets;

class TopDrawer : public Widget
{
public:
    explicit TopDrawer(Widget *parent = nullptr);

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
    int content_height_ = 480;

    std::shared_ptr<Widget>  bg_item_;
    std::shared_ptr<MouseArea> touch_area_;

    std::shared_ptr<SwipeView> _page_container;

    std::shared_ptr<widgets::Widget> _ctrl_panel;

    std::shared_ptr<ModuleCollector> _mod_panel;

    std::shared_ptr<MessageCenterControl> _notice_panel;

    std::shared_ptr<widgets::PageIndicator> _page_indicator;
};

} // namespace lvglpp::ui::page

#endif // TOP_DRAWER_H
