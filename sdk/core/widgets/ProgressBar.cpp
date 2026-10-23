#include "ProgressBar.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

ProgressBar::ProgressBar(Widget *parent) : Widget(tools::LvObjFactory::createLvglBar, parent)
{}

void ProgressBar::setValue(int32_t value, bool animate)
{
    lv_bar_set_value(lvObj(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t ProgressBar::value() const
{
    return lv_bar_get_value(lvObj());
}

void ProgressBar::setStartValue(int32_t value, bool animate)
{
    lv_bar_set_start_value(lvObj(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t ProgressBar::startValue() const
{
    return lv_bar_get_start_value(lvObj());
}

void ProgressBar::setRange(int32_t min, int32_t max)
{
    lv_bar_set_range(lvObj(), min, max);
}

int32_t ProgressBar::minValue() const
{
    return lv_bar_get_min_value(lvObj());
}

int32_t ProgressBar::maxValue() const
{
    return lv_bar_get_max_value(lvObj());
}

void ProgressBar::setMode(lv_bar_mode_t mode)
{
    lv_bar_set_mode(lvObj(), mode);
}

lv_bar_mode_t ProgressBar::mode() const
{
    return lv_bar_get_mode(lvObj());
}

void ProgressBar::setOrientation(lv_bar_orientation_t orient)
{
    lv_bar_set_orientation(lvObj(), orient);
}

lv_bar_orientation_t ProgressBar::orientation() const
{
    return lv_bar_get_orientation(lvObj());
}

} // namespace lvglpp::widgets
