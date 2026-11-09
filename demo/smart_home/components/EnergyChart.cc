#include "EnergyChart.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <format>

namespace lvglpp::ui::widgets {

using namespace form::design;

EnergyChart::EnergyChart(const std::string &title, w::Widget *parent)
    : w::Widget(parent)
{
    setSize(380, 220);
    setBgColor(CLR_SURFACE_CONTAINER);
    setRadius(RADIUS_12);

    // Title
    _titleLabel = std::make_unique<w::Label>(title, CLR_ON_SURFACE_VARIANT_0, this, w::Label::FontSize22Bold);
    _titleLabel->setAlignment(LV_ALIGN_TOP_LEFT, 12, 8);

    // Total kWh
    _totalLabel = std::make_unique<w::Label>(lvTrFmt("common_energy_kwh", 0.0f), CLR_PRIMARY, this, w::Label::FontSize28Bold);
    _totalLabel->setAlignment(LV_ALIGN_TOP_RIGHT, -12, 8);

    // Chart
    _chart = std::make_unique<w::Chart>(this);
    _chart->setSize(350, 120);
    _chart->setAlignment(LV_ALIGN_TOP_MID, 0, 40);
    _chart->setType(w::Chart::Bar);
    _chart->setPointCount(7);
    _chart->setAxisRange(w::Chart::PrimaryY, 0, 50);
    _chart->setDivisionLineCount(3, 0);

    _series = _chart->addSeries(lv_color_hex(CLR_PRIMARY), w::Chart::PrimaryY);

    // Day/Week toggle buttons
    _dayBtn = std::make_unique<w::PushButton>(50, 28, w::PushButton::Blue, lvTr("common_day"), this);
    _dayBtn->setAlignment(LV_ALIGN_BOTTOM_LEFT, 12, -8);

    _weekBtn = std::make_unique<w::PushButton>(50, 28, w::PushButton::Gray, lvTr("common_week"), this);
    _weekBtn->setAlignment(LV_ALIGN_BOTTOM_LEFT, 68, -8);
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
    _totalLabel->setText(lvTrFmt("common_energy_kwh", kWh));
}

} // namespace lvglpp::ui::widgets
