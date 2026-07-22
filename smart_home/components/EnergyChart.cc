#include "EnergyChart.h"
#include "theme/form_design.h"
#include <format>

namespace lvglpp::ui::widgets {

using namespace form::design;

EnergyChart::EnergyChart(const std::string &title, w::BaseItem *parent)
    : w::BaseItem(parent)
{
    setSize(380, 220);
    setBgColor(CLR_SURFACE_CONTAINER);
    setRadius(RADIUS_12);

    // Title
    _titleLabel = std::make_unique<w::LvText>(title, CLR_ON_SURFACE_VARIANT_0, this, w::LvText::FontSize22Bold);
    _titleLabel->setAligment(LV_ALIGN_TOP_LEFT, 12, 8);

    // Total kWh
    _totalLabel = std::make_unique<w::LvText>("0 kWh", CLR_PRIMARY, this, w::LvText::FontSize28Bold);
    _totalLabel->setAligment(LV_ALIGN_TOP_RIGHT, -12, 8);

    // Chart
    _chart = std::make_unique<w::LvChart>(this);
    _chart->setSize(350, 120);
    _chart->setAligment(LV_ALIGN_TOP_MID, 0, 40);
    _chart->setType(w::LvChart::Bar);
    _chart->setPointCount(7);
    _chart->setAxisRange(w::LvChart::PrimaryY, 0, 50);
    _chart->setDivLineCount(3, 0);

    _series = _chart->addSeries(lv_color_hex(CLR_PRIMARY), w::LvChart::PrimaryY);

    // Day/Week toggle buttons
    _dayBtn = std::make_unique<w::PushButton>(50, 28, w::PushButton::Blue, "Day", this);
    _dayBtn->setAligment(LV_ALIGN_BOTTOM_LEFT, 12, -8);

    _weekBtn = std::make_unique<w::PushButton>(50, 28, w::PushButton::Gray, "Week", this);
    _weekBtn->setAligment(LV_ALIGN_BOTTOM_LEFT, 68, -8);
}

void EnergyChart::setData(const std::vector<int32_t> &values, const std::vector<std::string> &) {
    if (!_series) return;
    _chart->setPointCount(static_cast<uint32_t>(values.size()));
    for (auto v : values) {
        _chart->setNextValue(_series, v);
    }
    _chart->refresh();
}

void EnergyChart::setTotal(float kWh) {
    _totalLabel->setText(std::format("{:.1f} kWh", kWh));
}

} // namespace lvglpp::ui::widgets
