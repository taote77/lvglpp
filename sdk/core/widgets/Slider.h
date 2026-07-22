#ifndef LVGLPP_SLIDER_H
#define LVGLPP_SLIDER_H

#include "BaseItem.h"

namespace lvglpp::widgets {

class Slider : public BaseItem
{
public:
    explicit Slider(BaseItem *parent = nullptr);

    /// Set the current value
    void setValue(int32_t value, bool animate = false);

    /// Get the current value
    int32_t value() const;

    /// Set the range (min, max)
    void setRange(int32_t min, int32_t max);

    /// Get min value
    int32_t minValue() const;

    /// Get max value
    int32_t maxValue() const;

    /// Set orientation: LV_DIR_HORIZ or LV_DIR_VERT
    void setOrientation(lv_dir_t orientation);

    /// Get orientation
    lv_dir_t orientation() const;

    /// Set mode: LV_SLIDER_MODE_NORMAL, LV_SLIDER_MODE_SYMMETRICAL, LV_SLIDER_MODE_RANGE
    void setMode(lv_slider_mode_t mode);

    /// Get mode
    lv_slider_mode_t mode() const;

    /// Set the left/start value (for range mode)
    void setLeftValue(int32_t value, bool animate = false);

    /// Get the left/start value
    int32_t leftValue() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_SLIDER_H
