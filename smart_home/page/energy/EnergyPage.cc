#include "EnergyPage.h"
#include "core/log/log.h"
#include "theme/form_design.h"
#include <format>

namespace lvglpp::ui::page {

using namespace form::design;

EnergyPage::EnergyPage() = default;
EnergyPage::~EnergyPage() = default;

void EnergyPage::onCreate(std::any arg) {
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    _title = std::make_unique<w::LvText>("Energy", CLR_ON_SURFACE, getRoot(), w::LvText::FontSize28Bold);
    _title->setAligment(LV_ALIGN_TOP_LEFT, 20, 10);

    buildTodaySummary();
    buildWeeklyChart();
    buildRoomBreakdown();
    buildTips();

    LOG_INFO() << "EnergyPage created";
}

void EnergyPage::buildTodaySummary() {
    _todayUsage = std::make_unique<w::LvText>("Today: 12.4 kWh", CLR_PRIMARY, getRoot(), w::LvText::FontSize36Bold);
    _todayUsage->setAligment(LV_ALIGN_TOP_MID, 0, 45);

    auto subLabel = std::make_unique<w::LvText>("Estimated cost: $1.86", CLR_ON_SURFACE_VARIANT_2, getRoot(), w::LvText::FontSize20);
    subLabel->setAligment(LV_ALIGN_TOP_MID, 0, 85);
}

void EnergyPage::buildWeeklyChart() {
    _weeklyChart = std::make_unique<ui::widgets::EnergyChart>("Weekly Usage", getRoot());
    _weeklyChart->setAligment(LV_ALIGN_TOP_MID, 0, 115);
    _weeklyChart->setTotal(86.3f);

    std::vector<int32_t> data = {12, 15, 10, 18, 11, 8, 14};
    std::vector<std::string> labels = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    _weeklyChart->setData(data, labels);
}

void EnergyPage::buildRoomBreakdown() {
    auto breakdownLabel = std::make_unique<w::LvText>("Per Room", CLR_ON_SURFACE_VARIANT_0, getRoot(), w::LvText::FontSize22Bold);
    breakdownLabel->setAligment(LV_ALIGN_TOP_LEFT, 20, 350);

    struct RoomUsage { const char *name; float kWh; uint32_t color; };
    RoomUsage rooms[] = {
        {"Living Room", 4.2f, CLR_PRIMARY},
        {"Kitchen",     3.8f, CLR_WARNING},
        {"Bedroom",     2.1f, CLR_INFO},
        {"Bathroom",    1.5f, CLR_SUCCESS},
    };

    int y = 378;
    for (auto &r : rooms) {
        auto label = std::make_unique<w::LvText>(std::format("{} - {:.1f} kWh", r.name, r.kWh), CLR_ON_SURFACE_VARIANT_1, getRoot(), w::LvText::FontSize20);
        label->setAligment(LV_ALIGN_TOP_LEFT, 30, y);

        auto bar = std::make_unique<w::ProgressBar>(getRoot());
        bar->setSize(200, 12);
        bar->setRange(0, 50);
        bar->setValue(static_cast<int32_t>(r.kWh * 10));
        bar->setAligment(LV_ALIGN_TOP_LEFT, 240, y + 2);
        y += 28;
    }
}

void EnergyPage::buildTips() {
    auto tipsLabel = std::make_unique<w::LvText>("Tip: Use LED bulbs to save up to 80% energy", CLR_SUCCESS, getRoot(), w::LvText::FontSize20);
    tipsLabel->setAligment(LV_ALIGN_BOTTOM_MID, 0, -15);
}

void EnergyPage::onNotifyUI(const sys::Event &) {}
void EnergyPage::onDestroy() {}

} // namespace lvglpp::ui::page
