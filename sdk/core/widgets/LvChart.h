#ifndef LVGLPP_LVCHART_H
#define LVGLPP_LVCHART_H

#include "BaseItem.h"

namespace lvglpp::widgets {

class LvChart : public BaseItem
{
public:
    explicit LvChart(BaseItem *parent = nullptr);
};

// Modern alias
using Chart = LvChart;

} // namespace lvglpp::widgets

#endif // LV_CHART_H
