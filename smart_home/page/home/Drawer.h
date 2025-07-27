#ifndef LVGLPP_DRAWER_H
#define LVGLPP_DRAWER_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/MouseArea.h"
#include <memory>

namespace lvglpp::ui::page {

using namespace lvglpp::widgets;

class Drawer : public BaseItem
{
public:
    enum Edge {
        TopEdge,
        LeftEdge,
        // BottomEdge,
        // RightEdge,
    };

    explicit Drawer(Drawer::Edge edge = TopEdge, BaseItem *parent = nullptr);

private:
    void changePagePos(int ypos);

    void pageEnterAnim();

    void pagePopAnim();

private:
    Edge                      _edge;
    std::shared_ptr<BaseItem> _root_container;

    std::shared_ptr<MouseArea> touch_area_;
};

} // namespace lvglpp::ui::page

#endif // LVGLPP_DRAWER_H
