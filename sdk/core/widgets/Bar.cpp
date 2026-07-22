#include "Bar.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

ProgressBar::ProgressBar(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglBar, parent)
{}

void ProgressBar::setValue(int32_t value, bool animate)
{
    lv_bar_set_value(getLvglItem(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t ProgressBar::value() const
{
    return lv_bar_get_value(getLvglItem());
}

void ProgressBar::setStartValue(int32_t value, bool animate)
{
    lv_bar_set_start_value(getLvglItem(), value, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

int32_t ProgressBar::startValue() const
{
    return lv_bar_get_start_value(getLvglItem());
}

void ProgressBar::setRange(int32_t min, int32_t max)
{
    lv_bar_set_range(getLvglItem(), min, max);
}

int32_t ProgressBar::minValue() const
{
    return lv_bar_get_min_value(getLvglItem());
}

int32_t ProgressBar::maxValue() const
{
    return lv_bar_get_max_value(getLvglItem());
}

void ProgressBar::setMode(lv_bar_mode_t mode)
{
    lv_bar_set_mode(getLvglItem(), mode);
}

lv_bar_mode_t ProgressBar::mode() const
{
    return lv_bar_get_mode(getLvglItem());
}

void ProgressBar::setOrientation(lv_bar_orientation_t orient)
{
    lv_bar_set_orientation(getLvglItem(), orient);
}

lv_bar_orientation_t ProgressBar::orientation() const
{
    return lv_bar_get_orientation(getLvglItem());
}

} // namespace lvglpp::widgets
