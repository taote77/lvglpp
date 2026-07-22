#ifndef SMART_HOME_DASHBOARD_PAGE_H
#define SMART_HOME_DASHBOARD_PAGE_H

#include "components/EnergyChart.h"
#include "components/RoomCard.h"
#include "components/SceneButton.h"
#include "components/StatusBadge.h"
#include "core/sys/BaseActivity.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include <memory>
#include <vector>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::page {

/// Main dashboard page — smart home overview
class DashboardPage : public sys::BaseActivity
{
public:
    DashboardPage();
    ~DashboardPage() override;

protected:
    void onCreate(std::any arg) override;
    void onNotifyUI(const sys::Event &evt) override;
    void onDestroy() override;

private:
    void buildWeatherBar();
    void buildQuickActions();
    void buildRoomCards();
    void buildEnergySummary();
    void buildSecurityStatus();

    // Weather
    std::unique_ptr<w::LvText> _weatherLabel;
    std::unique_ptr<w::LvText> _tempLabel;

    // Quick actions
    std::unique_ptr<w::PushButton> _lightsOffBtn;
    std::unique_ptr<w::PushButton> _lockAllBtn;
    std::unique_ptr<w::PushButton> _awayBtn;

    // Room cards
    std::vector<std::unique_ptr<ui::widgets::RoomCard>> _roomCards;

    // Energy
    std::unique_ptr<ui::widgets::EnergyChart> _energyChart;

    // Security
    std::unique_ptr<ui::widgets::StatusBadge> _doorBadge;
    std::unique_ptr<ui::widgets::StatusBadge> _cameraBadge;
    std::unique_ptr<ui::widgets::StatusBadge> _alarmBadge;
};

} // namespace lvglpp::ui::page

#endif
