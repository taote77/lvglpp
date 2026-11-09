#include "Arc.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Arc::Arc(Widget *parent)
    : Widget(tools::LvObjFactory::createLvglArc, parent)
{
    setFocusable(true);
}

void Arc::setValue(int32_t value)
{
    lv_arc_set_value(lvObj(), value);
}

int32_t Arc::value() const
{
    return lv_arc_get_value(lvObj());
}

void Arc::setRange(int32_t min, int32_t max)
{
    lv_arc_set_range(lvObj(), min, max);
}

int32_t Arc::minValue() const
{
    return lv_arc_get_min_value(lvObj());
}

int32_t Arc::maxValue() const
{
    return lv_arc_get_max_value(lvObj());
}

void Arc::setStartAngle(uint32_t startAngle)
{
    lv_arc_set_start_angle(lvObj(), startAngle);
}

void Arc::setEndAngle(uint32_t endAngle)
{
    lv_arc_set_end_angle(lvObj(), endAngle);
}

void Arc::setRotation(uint32_t rotation)
{
    lv_arc_set_rotation(lvObj(), rotation);
}

void Arc::setMode(lv_arc_mode_t mode)
{
    lv_arc_set_mode(lvObj(), mode);
}

lv_arc_mode_t Arc::mode() const
{
    return lv_arc_get_mode(lvObj());
}

} // namespace lvglpp::widgets
