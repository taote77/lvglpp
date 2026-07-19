
#ifndef LVGLPP_SCROLLITEM_H
#define LVGLPP_SCROLLITEM_H

#include "BaseItem.h"

namespace lvglpp::widgets {

class ScrollItem : public BaseItem
{
public:
    ScrollItem(BaseItem *parent);

    void setScrollable(bool b);

    bool getScrollable();

    void setFlickable(bool b);

    bool getFlickable();

    void setScrollBarVisible(bool b);

    void setScrollSnapping(lv_scroll_snap_t l);

    lv_coord_t getScrollSnapX();

    void setScrollToX(lv_coord_t x, bool animEnable);

    lv_coord_t getScrollSnapY();

    void setScrollToY(lv_coord_t y, bool animEnable);
};
} // namespace lvglpp::widgets

#endif // LVGLPP_SCROLLITEM_H
