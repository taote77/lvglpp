#ifndef SMART_HOME_ENERGY_PAGE_H
#define SMART_HOME_ENERGY_PAGE_H

#include "components/EnergyChart.h"
#include "core/sys/BaseActivity.h"
#include "core/widgets/Bar.h"
#include "core/widgets/LvText.h"
#include <memory>
#include <vector>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::page {

/// Energy monitoring page with charts and per-room breakdown
class EnergyPage : public sys::BaseActivity
{
public:
    EnergyPage();
    ~EnergyPage() override;

protected:
    void onCreate(std::any arg) override;
    void onNotifyUI(const sys::Event &evt) override;
    void onDestroy() override;

private:
    void buildTodaySummary();
    void buildWeeklyChart();
    void buildRoomBreakdown();
    void buildTips();

    std::unique_ptr<w::LvText> _title;
    std::unique_ptr<w::LvText> _todayUsage;
    std::unique_ptr<ui::widgets::EnergyChart> _weeklyChart;
};

} // namespace lvglpp::ui::page

#endif
