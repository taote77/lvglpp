#include "Slider.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Slider::Slider(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglSlider, parent)
{}

void Slider::setValue(int32_t value, bool animate)
{
    lv_slider_set_value(getLvglItem(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t Slider::value() const
{
    return lv_slider_get_value(getLvglItem());
}

void Slider::setRange(int32_t min, int32_t max)
{
    lv_slider_set_range(getLvglItem(), min, max);
}

int32_t Slider::minValue() const
{
    return lv_slider_get_min_value(getLvglItem());
}

int32_t Slider::maxValue() const
{
    return lv_slider_get_max_value(getLvglItem());
}

void Slider::setOrientation(lv_dir_t orientation)
{
    lv_slider_set_orientation(getLvglItem(),
        static_cast<lv_slider_orientation_t>(orientation));
}

lv_dir_t Slider::orientation() const
{
    return static_cast<lv_dir_t>(lv_slider_get_orientation(getLvglItem()));
}

void Slider::setMode(lv_slider_mode_t mode)
{
    lv_slider_set_mode(getLvglItem(), mode);
}

lv_slider_mode_t Slider::mode() const
{
    return lv_slider_get_mode(getLvglItem());
}

void Slider::setLeftValue(int32_t value, bool animate)
{
    lv_slider_set_start_value(getLvglItem(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t Slider::leftValue() const
{
    return lv_slider_get_left_value(getLvglItem());
}

} // namespace lvglpp::widgets
