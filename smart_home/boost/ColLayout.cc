#include "ColLayout.h"
#include "core/widgets/BaseItem.h"
#include <theme/form_design.h>

namespace wrapper::widgets {
using namespace form::design;

ColLayout::ColLayout(uint32_t width, uint32_t height, widgets::BaseItem *parent) : BaseItem(parent)
{
    this->setSize(width, height);

    this->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER));

    lv_obj_set_style_pad_hor(getLvglItem(), 28, LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(getLvglItem(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(getLvglItem(), 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(getLvglItem(), 26, LV_STATE_DEFAULT);
}

void ColLayout::setPadHor(int lenth)
{
    lv_obj_set_style_pad_hor(getLvglItem(), lenth, LV_STATE_DEFAULT);
}

void ColLayout::setPadRow(int lenth)
{
    lv_obj_set_style_pad_row(getLvglItem(), lenth, LV_STATE_DEFAULT);
}

void ColLayout::setPadBottom(int lenth)
{
    lv_obj_set_style_pad_bottom(getLvglItem(), lenth, LV_STATE_DEFAULT);
}

} // namespace wrapper::widgets
