#ifndef SMART_HOME_ENERGY_CHART_H
#define SMART_HOME_ENERGY_CHART_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/LvChart.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include <memory>
#include <string>
#include <vector>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::widgets {

/// Energy usage chart with day/week toggle
class EnergyChart : public w::BaseItem
{
public:
    EnergyChart(const std::string &title, w::BaseItem *parent = nullptr);
    void setData(const std::vector<int32_t> &values, const std::vector<std::string> &labels);
    void setTotal(float kWh);

private:
    std::unique_ptr<w::LvText>      _titleLabel;
    std::unique_ptr<w::LvText>      _totalLabel;
    std::unique_ptr<w::LvChart>     _chart;
    std::unique_ptr<w::PushButton>  _dayBtn;
    std::unique_ptr<w::PushButton>  _weekBtn;
    w::ChartSeries                 *_series = nullptr; // owned by LvChart
};

} // namespace lvglpp::ui::widgets

#endif
