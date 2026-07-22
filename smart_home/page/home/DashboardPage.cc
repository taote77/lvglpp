#include "DashboardPage.h"
#include "core/log/log.h"
#include "core/widgets/Toast.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {

using namespace form::design;

DashboardPage::DashboardPage() = default;
DashboardPage::~DashboardPage() = default;

void DashboardPage::onCreate(std::any arg) {
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    // Title
    auto title = std::make_unique<w::LvText>("Smart Home", CLR_ON_SURFACE, getRoot(), w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 20, 10);

    buildWeatherBar();
    buildQuickActions();
    buildRoomCards();
    buildEnergySummary();
    buildSecurityStatus();

    LOG_INFO() << "DashboardPage created";
}

void DashboardPage::buildWeatherBar() {
    _weatherLabel = std::make_unique<w::LvText>("Sunny  72F  45%", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::LvText::FontSize22);
    _weatherLabel->setAligment(LV_ALIGN_TOP_RIGHT, -20, 15);
}

void DashboardPage::buildQuickActions() {
    _lightsOffBtn = std::make_unique<w::PushButton>(120, 36, w::PushButton::Dark, "All Lights Off", getRoot());
    _lightsOffBtn->setAligment(LV_ALIGN_TOP_LEFT, 20, 45);
    _lightsOffBtn->setOnClickedListener([]() {
        w::Toast::success("All lights turned off");
    });

    _lockAllBtn = std::make_unique<w::PushButton>(120, 36, w::PushButton::Dark, "Lock All", getRoot());
    _lockAllBtn->setAligment(LV_ALIGN_TOP_LEFT, 150, 45);
    _lockAllBtn->setOnClickedListener([]() {
        w::Toast::success("All doors locked");
    });

    _awayBtn = std::make_unique<w::PushButton>(100, 36, w::PushButton::Red, "Away Mode", getRoot());
    _awayBtn->setAligment(LV_ALIGN_TOP_LEFT, 280, 45);
    _awayBtn->setOnClickedListener([]() {
        w::Toast::warn("Away mode activated");
    });
}

void DashboardPage::buildRoomCards() {
    struct RoomInfo { const char *name; const char *icon; int devices; float temp; float humidity; };
    RoomInfo rooms[] = {
        {"Living Room", "Sofa", 4, 22.5f, 48.0f},
        {"Bedroom", "Bed", 3, 20.0f, 52.0f},
        {"Kitchen", "Cook", 5, 23.0f, 55.0f},
        {"Bathroom", "Bath", 2, 24.0f, 60.0f},
    };

    int x = 20, y = 100;
    for (auto &r : rooms) {
        auto card = std::make_unique<ui::widgets::RoomCard>(r.name, r.icon, r.devices, r.temp, r.humidity, getRoot());
        card->setAligment(LV_ALIGN_TOP_LEFT, x, y);
        _roomCards.push_back(std::move(card));
        x += 190;
    }
}

void DashboardPage::buildEnergySummary() {
    _energyChart = std::make_unique<ui::widgets::EnergyChart>("Today's Energy", getRoot());
    _energyChart->setAligment(LV_ALIGN_TOP_LEFT, 20, 245);
    _energyChart->setTotal(12.4f);

    std::vector<int32_t> data = {8, 12, 15, 20, 25, 30, 18};
    std::vector<std::string> labels = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
    _energyChart->setData(data, labels);
}

void DashboardPage::buildSecurityStatus() {
    _doorBadge = std::make_unique<ui::widgets::StatusBadge>(ui::widgets::StatusBadge::Online, getRoot());
    _doorBadge->setLabel("Doors: Locked");
    _doorBadge->setAligment(LV_ALIGN_TOP_RIGHT, -20, 100);

    _cameraBadge = std::make_unique<ui::widgets::StatusBadge>(ui::widgets::StatusBadge::Online, getRoot());
    _cameraBadge->setLabel("Cameras: Active");
    _cameraBadge->setAligment(LV_ALIGN_TOP_RIGHT, -20, 135);

    _alarmBadge = std::make_unique<ui::widgets::StatusBadge>(ui::widgets::StatusBadge::Offline, getRoot());
    _alarmBadge->setLabel("Alarm: Disarmed");
    _alarmBadge->setAligment(LV_ALIGN_TOP_RIGHT, -20, 170);
}

void DashboardPage::onNotifyUI(const sys::Event &) {}
void DashboardPage::onDestroy() {}

} // namespace lvglpp::ui::page
