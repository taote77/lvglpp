
#include "ScrollItem.h"

namespace heygears {
namespace widgets {

ScrollItem::ScrollItem(BaseItem *parent) : BaseItem(parent) { }

void ScrollItem::setScrollable(bool b)
{
    if (b) {
        lv_obj_add_flag(getLvglItem(), LV_OBJ_FLAG_SCROLLABLE);
    } else {
        lv_obj_clear_flag(getLvglItem(), LV_OBJ_FLAG_SCROLLABLE);
    }
}

bool ScrollItem::getScrollable()
{
    return lv_obj_has_flag(getLvglItem(), LV_OBJ_FLAG_SCROLLABLE);
}

void ScrollItem::setFlickable(bool b)
{
    if (b) {
        lv_obj_add_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ELASTIC);
    } else {
        lv_obj_clear_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ELASTIC);
    }
}

bool ScrollItem::getFlickable()
{
    return lv_obj_has_flag(getLvglItem(), LV_OBJ_FLAG_SCROLL_ELASTIC);
}

void ScrollItem::setScrollBarVisible(bool b)
{
    if (b) {
        lv_obj_set_scrollbar_mode(getLvglItem(), LV_SCROLLBAR_MODE_AUTO);
    } else {
        lv_obj_set_scrollbar_mode(getLvglItem(), LV_SCROLLBAR_MODE_OFF);
    }
}

void ScrollItem::setScrollSnapping(lv_scroll_snap_t l)
{
    lv_obj_set_scroll_snap_x(getLvglItem(), l);
}

lv_coord_t ScrollItem::getScrollSnapX()
{
    return lv_obj_get_scroll_x(getLvglItem());
}

void ScrollItem::setScrollToX(lv_coord_t x, bool animEnable)
{
    lv_obj_scroll_to_x(getLvglItem(), x, animEnable ? LV_ANIM_ON : LV_ANIM_OFF);
}

lv_coord_t ScrollItem::getScrollSnapY()
{
    return lv_obj_get_scroll_y(getLvglItem());
}

void ScrollItem::setScrollToY(lv_coord_t y, bool animEnable)
{
    lv_obj_scroll_to_y(getLvglItem(), y, animEnable ? LV_ANIM_ON : LV_ANIM_OFF);
}
} // namespace widgets
} // namespace heygears