#ifndef LVGLPP_ARC_H
#define LVGLPP_ARC_H

#include "BaseItem.h"

namespace lvglpp::widgets {

class Arc : public BaseItem
{
public:
    explicit Arc(BaseItem *parent = nullptr);

    /// Set the current value (angle)
    void setValue(int32_t value);

    /// Get the current value
    int32_t value() const;

    /// Set range (min angle, max angle in degrees * 10)
    void setRange(int32_t min, int32_t max);

    /// Get min
    int32_t minValue() const;

    /// Get max
    int32_t maxValue() const;

    /// Set the start angle (0-360 degrees)
    void setStartAngle(uint32_t startAngle);

    /// Set the end angle (0-360 degrees)
    void setEndAngle(uint32_t endAngle);

    /// Set rotation offset
    void setRotation(uint32_t rotation);

    /// Set mode: LV_ARC_MODE_NORMAL, LV_ARC_MODE_REVERSE, LV_ARC_MODE_SYMMETRICAL
    void setMode(lv_arc_mode_t mode);

    /// Get mode
    lv_arc_mode_t mode() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_ARC_H
