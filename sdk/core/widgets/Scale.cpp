#include "Scale.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Scale::Scale(BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglScale, parent)
{
    setFocusable(true);
}

Scale &Scale::setRange(int32_t min, int32_t max)
{
    if (getLvglItem() != nullptr)
    {
        lv_scale_set_range(getLvglItem(), min, max);
    }
    return *this;
}

Scale &Scale::setTotalTickCount(uint32_t count)
{
    if (getLvglItem() != nullptr)
    {
        lv_scale_set_total_tick_count(getLvglItem(), count);
    }
    return *this;
}

Scale &Scale::setMajorTickEvery(uint32_t every)
{
    if (getLvglItem() != nullptr)
    {
        lv_scale_set_major_tick_every(getLvglItem(), every);
    }
    return *this;
}

Scale &Scale::setLabelShow(bool show)
{
    if (getLvglItem() != nullptr)
    {
        lv_scale_set_label_show(getLvglItem(), show);
    }
    return *this;
}

Scale &Scale::setAngleRange(uint32_t degrees)
{
    if (getLvglItem() != nullptr)
    {
        lv_scale_set_angle_range(getLvglItem(), degrees);
    }
    return *this;
}

Scale &Scale::setRotation(int32_t degrees)
{
    if (getLvglItem() != nullptr)
    {
        lv_scale_set_rotation(getLvglItem(), degrees);
    }
    return *this;
}

Scale &Scale::setMode(lv_scale_mode_t mode)
{
    if (getLvglItem() != nullptr)
    {
        lv_scale_set_mode(getLvglItem(), mode);
    }
    return *this;
}

} // namespace lvglpp::widgets
