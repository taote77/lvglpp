#include "Chart.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Chart::Chart(Widget *parent) : Widget(tools::LvObjFactory::createLvglChart, parent)
{
    // Quantitative plots keep a left-to-right axis even in an RTL UI.
    setBaseDir(LV_BASE_DIR_LTR);
}

void Chart::setType(Type type)
{
    lv_chart_set_type(lvObj(), static_cast<lv_chart_type_t>(type));
}

Chart::Type Chart::type() const
{
    return static_cast<Type>(lv_chart_get_type(lvObj()));
}

void Chart::setPointCount(uint32_t count)
{
    lv_chart_set_point_count(lvObj(), count);
}

uint32_t Chart::pointCount() const
{
    return lv_chart_get_point_count(lvObj());
}

void Chart::setAxisRange(Axis axis, int32_t min, int32_t max)
{
    lv_chart_set_axis_range(lvObj(), static_cast<lv_chart_axis_t>(axis), min, max);
}

void Chart::setUpdateMode(UpdateMode mode)
{
    lv_chart_set_update_mode(lvObj(), static_cast<lv_chart_update_mode_t>(mode));
}

void Chart::setDivisionLineCount(uint8_t horizontal, uint8_t vertical)
{
    lv_chart_set_div_line_count(lvObj(), horizontal, vertical);
}

ChartSeries *Chart::addSeries(lv_color_t color, Axis axis)
{
    auto *ser = lv_chart_add_series(lvObj(), color, static_cast<lv_chart_axis_t>(axis));
    return ser ? new ChartSeries(ser) : nullptr;
}

void Chart::removeSeries(ChartSeries *series)
{
    if (series)
    {
        lv_chart_remove_series(lvObj(), series->handle());
        delete series;
    }
}

void Chart::setSeriesHidden(ChartSeries *series, bool hidden)
{
    if (series)
    {
        lv_chart_hide_series(lvObj(), series->handle(), hidden);
    }
}

void Chart::setSeriesColor(ChartSeries *series, lv_color_t color)
{
    if (series)
    {
        lv_chart_set_series_color(lvObj(), series->handle(), color);
    }
}

void Chart::setNextValue(ChartSeries *series, int32_t value)
{
    if (series)
    {
        lv_chart_set_next_value(lvObj(), series->handle(), value);
    }
}

void Chart::setNextPoint(ChartSeries *series, int32_t xValue, int32_t yValue)
{
    if (series)
    {
        lv_chart_set_next_value2(lvObj(), series->handle(), xValue, yValue);
    }
}

void Chart::setXStartPoint(ChartSeries *series, uint32_t id)
{
    if (series)
    {
        lv_chart_set_x_start_point(lvObj(), series->handle(), id);
    }
}

void Chart::getPointPosition(ChartSeries *series, uint32_t index, lv_coord_t &x, lv_coord_t &y) const
{
    lv_point_t p;
    lv_chart_get_point_pos_by_id(lvObj(), series->handle(), index, &p);
    x = p.x;
    y = p.y;
}

ChartCursor *Chart::addCursor(lv_color_t color, lv_dir_t direction)
{
    auto *cursor = lv_chart_add_cursor(lvObj(), color, direction);
    return cursor ? new ChartCursor(cursor) : nullptr;
}

void Chart::setCursorPoint(ChartCursor *cursor, ChartSeries *series, uint32_t pointIndex)
{
    if (cursor && series)
    {
        lv_chart_set_cursor_point(lvObj(), cursor->handle(), series->handle(), pointIndex);
    }
}

void Chart::refresh()
{
    lv_chart_refresh(lvObj());
}

std::vector<ChartSeries *> Chart::series() const
{
    std::vector<ChartSeries *> result;
    auto                      *s = lv_chart_get_series_next(lvObj(), nullptr);
    while (s)
    {
        result.push_back(new ChartSeries(s));
        s = lv_chart_get_series_next(lvObj(), s);
    }
    return result;
}

} // namespace lvglpp::widgets
