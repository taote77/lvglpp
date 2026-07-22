#include "LvChart.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

LvChart::LvChart(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglChart, parent)
{}

void LvChart::setType(Type type)
{
    lv_chart_set_type(getLvglItem(), static_cast<lv_chart_type_t>(type));
}

LvChart::Type LvChart::type() const
{
    return static_cast<Type>(lv_chart_get_type(getLvglItem()));
}

void LvChart::setPointCount(uint32_t count)
{
    lv_chart_set_point_count(getLvglItem(), count);
}

uint32_t LvChart::pointCount() const
{
    return lv_chart_get_point_count(getLvglItem());
}

void LvChart::setAxisRange(Axis axis, int32_t min, int32_t max)
{
    lv_chart_set_axis_range(getLvglItem(), static_cast<lv_chart_axis_t>(axis), min, max);
}

void LvChart::setUpdateMode(UpdateMode mode)
{
    lv_chart_set_update_mode(getLvglItem(), static_cast<lv_chart_update_mode_t>(mode));
}

void LvChart::setDivLineCount(uint8_t horizontal, uint8_t vertical)
{
    lv_chart_set_div_line_count(getLvglItem(), horizontal, vertical);
}

ChartSeries *LvChart::addSeries(lv_color_t color, Axis axis)
{
    auto *ser = lv_chart_add_series(getLvglItem(), color,
        static_cast<lv_chart_axis_t>(axis));
    return ser ? new ChartSeries(ser) : nullptr;
}

void LvChart::removeSeries(ChartSeries *series)
{
    if (series) {
        lv_chart_remove_series(getLvglItem(), series->handle());
        delete series;
    }
}

void LvChart::setSeriesHidden(ChartSeries *series, bool hidden)
{
    if (series) {
        lv_chart_hide_series(getLvglItem(), series->handle(), hidden);
    }
}

void LvChart::setSeriesColor(ChartSeries *series, lv_color_t color)
{
    if (series) {
        lv_chart_set_series_color(getLvglItem(), series->handle(), color);
    }
}

void LvChart::setNextValue(ChartSeries *series, int32_t value)
{
    if (series) {
        lv_chart_set_next_value(getLvglItem(), series->handle(), value);
    }
}

void LvChart::setNextValue2(ChartSeries *series, int32_t xValue, int32_t yValue)
{
    if (series) {
        lv_chart_set_next_value2(getLvglItem(), series->handle(), xValue, yValue);
    }
}

void LvChart::setXStartPoint(ChartSeries *series, uint32_t id)
{
    if (series) {
        lv_chart_set_x_start_point(getLvglItem(), series->handle(), id);
    }
}

void LvChart::getPointPosition(ChartSeries *series, uint32_t index,
                                lv_coord_t &x, lv_coord_t &y) const
{
    lv_point_t p;
    lv_chart_get_point_pos_by_id(getLvglItem(), series->handle(), index, &p);
    x = p.x;
    y = p.y;
}

ChartCursor *LvChart::addCursor(lv_color_t color, lv_dir_t direction)
{
    auto *cursor = lv_chart_add_cursor(getLvglItem(), color, direction);
    return cursor ? new ChartCursor(cursor) : nullptr;
}

void LvChart::setCursorPoint(ChartCursor *cursor, ChartSeries *series, uint32_t pointIndex)
{
    if (cursor && series) {
        lv_chart_set_cursor_point(getLvglItem(), cursor->handle(), series->handle(), pointIndex);
    }
}

void LvChart::refresh()
{
    lv_chart_refresh(getLvglItem());
}

std::vector<ChartSeries *> LvChart::series() const
{
    std::vector<ChartSeries *> result;
    auto *s = lv_chart_get_series_next(getLvglItem(), nullptr);
    while (s) {
        result.push_back(new ChartSeries(s));
        s = lv_chart_get_series_next(getLvglItem(), s);
    }
    return result;
}

} // namespace lvglpp::widgets
