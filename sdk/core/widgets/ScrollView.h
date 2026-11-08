
#ifndef LVGLPP_SCROLLVIEW_H
#define LVGLPP_SCROLLVIEW_H

#include "Widget.h"

namespace lvglpp::widgets {

class ScrollView : public Widget
{
public:
    ScrollView(Widget *parent);

    void setScrollable(bool b);

    bool isScrollable() const;

    /// Scroll with momentum after a flick (LV_OBJ_FLAG_SCROLL_MOMENTUM).
    void setFlickable(bool b);

    bool isFlickable() const;

    void setScrollbarVisible(bool b);

    void setScrollSnapping(lv_scroll_snap_t l);

    lv_coord_t getScrollSnapX() const;

    void setScrollToX(lv_coord_t x, bool animEnable);

    lv_coord_t getScrollSnapY() const;

    void setScrollToY(lv_coord_t y, bool animEnable);
};
} // namespace lvglpp::widgets

#endif // LVGLPP_SCROLLVIEW_H
