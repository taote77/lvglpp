#include "Slider.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Slider::Slider(Widget *parent)
    : Widget(tools::LvObjFactory::createLvglSlider, parent)
{
    setFocusable(true);
}

void Slider::setValue(int32_t value, bool animate)
{
    lv_slider_set_value(lvObj(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t Slider::value() const
{
    return lv_slider_get_value(lvObj());
}

void Slider::setRange(int32_t min, int32_t max)
{
    lv_slider_set_range(lvObj(), min, max);
}

int32_t Slider::minValue() const
{
    return lv_slider_get_min_value(lvObj());
}

int32_t Slider::maxValue() const
{
    return lv_slider_get_max_value(lvObj());
}

void Slider::setOrientation(lv_dir_t orientation)
{
    lv_slider_set_orientation(lvObj(),
        static_cast<lv_slider_orientation_t>(orientation));
}

lv_dir_t Slider::orientation() const
{
    return static_cast<lv_dir_t>(lv_slider_get_orientation(lvObj()));
}

void Slider::setMode(lv_slider_mode_t mode)
{
    lv_slider_set_mode(lvObj(), mode);
}

lv_slider_mode_t Slider::mode() const
{
    return lv_slider_get_mode(lvObj());
}

void Slider::setLeftValue(int32_t value, bool animate)
{
    lv_slider_set_start_value(lvObj(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t Slider::leftValue() const
{
    return lv_slider_get_left_value(lvObj());
}

} // namespace lvglpp::widgets
