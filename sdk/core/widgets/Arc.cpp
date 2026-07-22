#include "Arc.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Arc::Arc(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglArc, parent)
{}

void Arc::setValue(int32_t value)
{
    lv_arc_set_value(getLvglItem(), value);
}

int32_t Arc::value() const
{
    return lv_arc_get_value(getLvglItem());
}

void Arc::setRange(int32_t min, int32_t max)
{
    lv_arc_set_range(getLvglItem(), min, max);
}

int32_t Arc::minValue() const
{
    return lv_arc_get_min_value(getLvglItem());
}

int32_t Arc::maxValue() const
{
    return lv_arc_get_max_value(getLvglItem());
}

void Arc::setStartAngle(uint32_t startAngle)
{
    lv_arc_set_start_angle(getLvglItem(), startAngle);
}

void Arc::setEndAngle(uint32_t endAngle)
{
    lv_arc_set_end_angle(getLvglItem(), endAngle);
}

void Arc::setRotation(uint32_t rotation)
{
    lv_arc_set_rotation(getLvglItem(), rotation);
}

void Arc::setMode(lv_arc_mode_t mode)
{
    lv_arc_set_mode(getLvglItem(), mode);
}

lv_arc_mode_t Arc::mode() const
{
    return lv_arc_get_mode(getLvglItem());
}

} // namespace lvglpp::widgets
