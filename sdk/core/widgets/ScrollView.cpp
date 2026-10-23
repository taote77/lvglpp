
#include "ScrollView.h"

namespace lvglpp::widgets {

ScrollView::ScrollView(Widget *parent) : Widget(parent)
{}

void ScrollView::setScrollable(bool b)
{
    if (b)
    {
        lv_obj_add_flag(lvObj(), LV_OBJ_FLAG_SCROLLABLE);
    } else
    {
        lv_obj_clear_flag(lvObj(), LV_OBJ_FLAG_SCROLLABLE);
    }
}

bool ScrollView::isScrollable() const
{
    return lv_obj_has_flag(lvObj(), LV_OBJ_FLAG_SCROLLABLE);
}

void ScrollView::setFlickable(bool b)
{
    if (b)
    {
        lv_obj_add_flag(lvObj(), LV_OBJ_FLAG_SCROLL_ELASTIC);
    } else
    {
        lv_obj_clear_flag(lvObj(), LV_OBJ_FLAG_SCROLL_ELASTIC);
    }
}

bool ScrollView::isFlickable() const
{
    return lv_obj_has_flag(lvObj(), LV_OBJ_FLAG_SCROLL_ELASTIC);
}

void ScrollView::setScrollbarVisible(bool b)
{
    if (b)
    {
        lv_obj_set_scrollbar_mode(lvObj(), LV_SCROLLBAR_MODE_AUTO);
    } else
    {
        lv_obj_set_scrollbar_mode(lvObj(), LV_SCROLLBAR_MODE_OFF);
    }
}

void ScrollView::setScrollSnapping(lv_scroll_snap_t l)
{
    lv_obj_set_scroll_snap_x(lvObj(), l);
}

lv_coord_t ScrollView::getScrollSnapX() const
{
    return lv_obj_get_scroll_x(lvObj());
}

void ScrollView::setScrollToX(lv_coord_t x, bool animEnable)
{
    lv_obj_scroll_to_x(lvObj(), x, animEnable ? LV_ANIM_ON : LV_ANIM_OFF);
}

lv_coord_t ScrollView::getScrollSnapY() const
{
    return lv_obj_get_scroll_y(lvObj());
}

void ScrollView::setScrollToY(lv_coord_t y, bool animEnable)
{
    lv_obj_scroll_to_y(lvObj(), y, animEnable ? LV_ANIM_ON : LV_ANIM_OFF);
}
} // namespace lvglpp::widgets