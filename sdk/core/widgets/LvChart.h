#ifndef LVGLPP_LVCHART_H
#define LVGLPP_LVCHART_H

#include "BaseItem.h"
#include <vector>

namespace lvglpp::widgets {

class ChartSeries {
public:
    ChartSeries(lv_chart_series_t *ser) : _series(ser) {}
    lv_chart_series_t *handle() const { return _series; }
private:
    lv_chart_series_t *_series;
};

class ChartCursor {
public:
    ChartCursor(lv_chart_cursor_t *cursor) : _cursor(cursor) {}
    lv_chart_cursor_t *handle() const { return _cursor; }
private:
    lv_chart_cursor_t *_cursor;
};

class LvChart : public BaseItem
{
public:
    /// Chart type
    enum Type {
        None  = LV_CHART_TYPE_NONE,
        Line  = LV_CHART_TYPE_LINE,
        Bar   = LV_CHART_TYPE_BAR,
        Scatter = LV_CHART_TYPE_SCATTER,
    };

    /// Axis
    enum Axis {
        PrimaryY   = LV_CHART_AXIS_PRIMARY_Y,
        SecondaryY = LV_CHART_AXIS_SECONDARY_Y,
        PrimaryX   = LV_CHART_AXIS_PRIMARY_X,
        SecondaryX = LV_CHART_AXIS_SECONDARY_X,
    };

    /// Update mode
    enum UpdateMode {
        Shift  = LV_CHART_UPDATE_MODE_SHIFT,
        Circular = LV_CHART_UPDATE_MODE_CIRCULAR,
    };

    explicit LvChart(BaseItem *parent = nullptr);

    /// Set chart type
    void setType(Type type);

    /// Get chart type
    Type type() const;

    /// Set the number of data points each series can hold
    void setPointCount(uint32_t count);

    /// Get point count
    uint32_t pointCount() const;

    /// Set the range for an axis
    void setAxisRange(Axis axis, int32_t min, int32_t max);

    /// Set update mode
    void setUpdateMode(UpdateMode mode);

    /// Set divider line count (horizontal, vertical)
    void setDivLineCount(uint8_t horizontal, uint8_t vertical);

    /// Add a data series. Returns a handle for setting data.
    ChartSeries *addSeries(lv_color_t color, Axis axis = Axis::PrimaryY);

    /// Remove a series
    void removeSeries(ChartSeries *series);

    /// Hide or show a series
    void setSeriesHidden(ChartSeries *series, bool hidden);

    /// Set series color
    void setSeriesColor(ChartSeries *series, lv_color_t color);

    /// Set the next value(s) for a series. Pass one value for line, two for bar (prev, next)
    void setNextValue(ChartSeries *series, int32_t value);

    /// Set next value with X and Y (for scatter charts)
    void setNextValue2(ChartSeries *series, int32_t xValue, int32_t yValue);

    /// Set the starting x-index for a series
    void setXStartPoint(ChartSeries *series, uint32_t id);

    /// Get the screen position of a data point
    void getPointPosition(ChartSeries *series, uint32_t index, lv_coord_t &x, lv_coord_t &y) const;

    /// Add a cursor
    ChartCursor *addCursor(lv_color_t color, lv_dir_t direction);

    /// Set cursor position by point
    void setCursorPoint(ChartCursor *cursor, ChartSeries *series, uint32_t pointIndex);

    /// Force a chart refresh
    void refresh();

    /// Get all series
    std::vector<ChartSeries *> series() const;
};

// Modern alias
using Chart = LvChart;

} // namespace lvglpp::widgets

#endif // LV_CHART_H
