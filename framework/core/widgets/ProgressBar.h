#ifndef LVGLPP_PROGRESSBAR_H
#define LVGLPP_PROGRESSBAR_H

#include "Widget.h"

namespace lvglpp::widgets {

class ProgressBar : public Widget
{
public:
    explicit ProgressBar(Widget *parent = nullptr);

    /// Set the current value
    void setValue(int32_t value, bool animate = false);

    /// Get the current value
    int32_t value() const;

    /// Set the start value (for range bars)
    void setStartValue(int32_t value, bool animate = false);

    /// Get the start value
    int32_t startValue() const;

    /// Set range (min, max)
    void setRange(int32_t min, int32_t max);

    /// Get min
    int32_t minValue() const;

    /// Get max
    int32_t maxValue() const;

    /// Set mode: LV_BAR_MODE_NORMAL, LV_BAR_MODE_SYMMETRICAL, LV_BAR_MODE_RANGE
    void setMode(lv_bar_mode_t mode);

    /// Get mode
    lv_bar_mode_t mode() const;

    /// Set orientation (LV_BAR_ORIENTATION_HORIZONTAL / VERTICAL)
    void setOrientation(lv_bar_orientation_t orient);

    /// Get orientation
    lv_bar_orientation_t orientation() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_PROGRESSBAR_H
