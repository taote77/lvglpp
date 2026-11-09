#include "Scale.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Scale::Scale(Widget *parent) : Widget(tools::LvObjFactory::createLvglScale, parent)
{
    setFocusable(true);
}

Scale &Scale::setRange(int32_t min, int32_t max)
{
    if (lvObj() != nullptr)
    {
        lv_scale_set_range(lvObj(), min, max);
    }
    return *this;
}

Scale &Scale::setTotalTickCount(uint32_t count)
{
    if (lvObj() != nullptr)
    {
        lv_scale_set_total_tick_count(lvObj(), count);
    }
    return *this;
}

Scale &Scale::setMajorTickEvery(uint32_t every)
{
    if (lvObj() != nullptr)
    {
        lv_scale_set_major_tick_every(lvObj(), every);
    }
    return *this;
}

Scale &Scale::setLabelShow(bool show)
{
    if (lvObj() != nullptr)
    {
        lv_scale_set_label_show(lvObj(), show);
    }
    return *this;
}

Scale &Scale::setAngleRange(uint32_t degrees)
{
    if (lvObj() != nullptr)
    {
        lv_scale_set_angle_range(lvObj(), degrees);
    }
    return *this;
}

Scale &Scale::setRotation(int32_t degrees)
{
    if (lvObj() != nullptr)
    {
        lv_scale_set_rotation(lvObj(), degrees);
    }
    return *this;
}

Scale &Scale::setMode(lv_scale_mode_t mode)
{
    if (lvObj() != nullptr)
    {
        lv_scale_set_mode(lvObj(), mode);
    }
    return *this;
}

} // namespace lvglpp::widgets
