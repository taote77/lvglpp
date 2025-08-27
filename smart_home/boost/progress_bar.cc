#include "progress_bar.h"
#include "core/widgets/BaseItem.h"
#include <algorithm>

namespace wrapper::widgets {
ProgressBar::ProgressBar(widgets::BaseItem *parent, int w, int h, int min, int max, int val) : BaseItem(BaseItem::Progress, parent), _max_value(max), _min_value(min)
{
    setSize(w, h);
    updateValue(val);
    // lv_bar_set_value(this->getLvglItem(), val, LV_ANIM_OFF);

    lv_bar_set_range(this->getLvglItem(), min, max);
    updateValue(val);
}

void ProgressBar::updateValue(int val)
{
    val = std::clamp(val, _min_value, _max_value);
    lv_bar_set_value(this->getLvglItem(), val, LV_ANIM_OFF);
}

void ProgressBar::setBgRadius(int32_t radius)
{
    // 背景圆角（默认直角）
    lv_obj_set_style_radius(this->getLvglItem(), radius, LV_PART_MAIN);
}

void ProgressBar::setIndicatorRadius(int32_t radius)
{
    // 指示器圆角（与背景一致）
    lv_obj_set_style_radius(this->getLvglItem(), radius, LV_PART_INDICATOR);
}

void ProgressBar::setPadSpace(int32_t sapce)
{
    // 边距（让指示器与背景有间隔）
    lv_obj_set_style_pad_all(this->getLvglItem(), sapce, LV_PART_MAIN); // 背景内边距
}

void ProgressBar::setMainColor(int32_t clr)
{
    lv_obj_set_style_bg_color(this->getLvglItem(), lv_color_hex(clr), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(this->getLvglItem(), LV_OPA_COVER, LV_PART_MAIN); // 确保不透明度生效
}

void ProgressBar::setIndicatorColor(int32_t clr)
{
    lv_obj_set_style_bg_color(this->getLvglItem(), lv_color_hex(clr), LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(this->getLvglItem(), LV_OPA_COVER, LV_PART_INDICATOR);
}

} // namespace wrapper::widgets
