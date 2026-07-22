#ifndef LVGLPP_LINE_H
#define LVGLPP_LINE_H

#include "BaseItem.h"
#include <vector>

namespace lvglpp::widgets {

class Line : public BaseItem
{
public:
    explicit Line(BaseItem *parent = nullptr);

    /// Set all points from an array of lv_point_precise_t
    /// Coordinates use LVGL's precise format (fixed-point with 8-bit fraction)
    void setPoints(const lv_point_precise_t *points, uint32_t count);

    /// Set points conveniently from x/y pairs
    void setPoints(const std::vector<lv_point_precise_t> &points);

    /// Get the number of points
    uint32_t pointCount() const;

    /// Set Y-inversion (flip vertically)
    void setYInvert(bool invert);

    /// Get Y-inversion state
    bool yInvert() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_LINE_H
