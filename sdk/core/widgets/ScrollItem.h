
#ifndef LV_SCROLL_ITEM_H
#define LV_SCROLL_ITEM_H

#include "BaseItem.h"

namespace heygears {
namespace widgets {

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
} // namespace widgets
} // namespace heygears

#endif // LV_SCROLL_ITEM_H
