#include "ColLayout.h"
#include "core/widgets/Widget.h"
#include <theme/form_design.h>

namespace wrapper::widgets {
using namespace form::design;

ColLayout::ColLayout(uint32_t width, uint32_t height, widgets::Widget *parent) : Widget(parent)
{
    this->setSize(width, height);

    this->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER));

    lv_obj_set_style_pad_hor(lvObj(), 28, LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(lvObj(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(lvObj(), 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(lvObj(), 26, LV_STATE_DEFAULT);
}

void ColLayout::setPadHor(int lenth)
{
    lv_obj_set_style_pad_hor(lvObj(), lenth, LV_STATE_DEFAULT);
}

void ColLayout::setPadRow(int lenth)
{
    lv_obj_set_style_pad_row(lvObj(), lenth, LV_STATE_DEFAULT);
}

void ColLayout::setPadBottom(int lenth)
{
    lv_obj_set_style_pad_bottom(lvObj(), lenth, LV_STATE_DEFAULT);
}

} // namespace wrapper::widgets
