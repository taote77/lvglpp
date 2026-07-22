#include "Line.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Line::Line(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglLine, parent)
{}

void Line::setPoints(const lv_point_precise_t *points, uint32_t count)
{
    lv_line_set_points(getLvglItem(), points, count);
}

void Line::setPoints(const std::vector<lv_point_precise_t> &points)
{
    setPoints(points.data(), static_cast<uint32_t>(points.size()));
}

uint32_t Line::pointCount() const
{
    return lv_line_get_point_count(getLvglItem());
}

void Line::setYInvert(bool invert)
{
    lv_line_set_y_invert(getLvglItem(), invert);
}

bool Line::yInvert() const
{
    return lv_line_get_y_invert(getLvglItem());
}

} // namespace lvglpp::widgets
