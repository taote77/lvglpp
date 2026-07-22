/**
 * Smart Home UI — LVGL++ Application
 *
 * Bottom navigation bar with 5 tabs. Content pages are built directly
 * inside a container and swapped via visibility toggling.
 */

#include "core/sys/app/SdlApplication.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/StackView.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/Dropdown.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/Roller.h"
#include "core/widgets/ScrollItem.h"
#include "core/widgets/Slider.h"
#include "core/widgets/SwitchButton.h"
#include "core/widgets/Toast.h"
#include "components/DeviceCard.h"
#include "components/EnergyChart.h"
#include "components/LightWidget.h"
#include "components/RoomCard.h"
#include "components/SceneButton.h"
#include "components/StatusBadge.h"
#include "components/ThermostatWidget.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <memory>
#include <vector>

using namespace lvglpp;
using namespace form::design;

namespace w = ::lvglpp::widgets;
namespace c = ::lvglpp::ui::widgets;

// ============================================================
// Page builders — each returns a BaseItem* container
// ============================================================

static void buildDashboard(w::BaseItem *root)
{
    // Title
    auto title = std::make_unique<w::LvText>("Smart Home", CLR_ON_SURFACE, root, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    // Weather
    auto weather = std::make_unique<w::LvText>("Sunny  72F  45%", CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize22);
    weather->setAligment(LV_ALIGN_TOP_RIGHT, -16, 12);

    // Quick actions row
    auto lightsOff = std::make_unique<w::PushButton>(110, 32, w::PushButton::Dark, "Lights Off", root);
    lightsOff->setAligment(LV_ALIGN_TOP_LEFT, 16, 42);
    lightsOff->setOnClickedListener([]() { w::Toast::success("All lights off"); });

    auto lockAll = std::make_unique<w::PushButton>(110, 32, w::PushButton::Dark, "Lock All", root);
    lockAll->setAligment(LV_ALIGN_TOP_LEFT, 134, 42);
    lockAll->setOnClickedListener([]() { w::Toast::success("All doors locked"); });

    auto awayBtn = std::make_unique<w::PushButton>(100, 32, w::PushButton::Red, "Away", root);
    awayBtn->setAligment(LV_ALIGN_TOP_LEFT, 252, 42);
    awayBtn->setOnClickedListener([]() { w::Toast::warn("Away mode"); });

    // Room cards (2x2 grid)
    struct { const char *n; const char *i; int d; float t; float h; } rooms[] = {
        {"Living Room","Sofa",4,22.5f,48},{"Bedroom","Bed",3,20,52},
        {"Kitchen","Cook",5,23,55},{"Bathroom","Bath",2,24,60},
    };
    int pos[4][2] = {{16,85},{208,85},{16,225},{208,225}};
    for (int i = 0; i < 4; i++) {
        auto card = std::make_unique<c::RoomCard>(rooms[i].n, rooms[i].i, rooms[i].d, rooms[i].t, rooms[i].h, root);
        card->setAligment(LV_ALIGN_TOP_LEFT, pos[i][0], pos[i][1]);
    }

    // Energy chart
    auto chart = std::make_unique<c::EnergyChart>("Today", root);
    chart->setSize(380, 200);
    chart->setAligment(LV_ALIGN_TOP_RIGHT, -16, 85);
    chart->setTotal(12.4f);
    chart->setData({8,12,15,20,25,30,18}, {});

    // Security status
    auto secTitle = std::make_unique<w::LvText>("Security", CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize22Bold);
    secTitle->setAligment(LV_ALIGN_TOP_LEFT, 16, 370);

    auto doorBadge = std::make_unique<c::StatusBadge>(c::StatusBadge::Online, root);
    doorBadge->setLabel("Doors Locked");
    doorBadge->setAligment(LV_ALIGN_TOP_LEFT, 16, 400);

    auto camBadge = std::make_unique<c::StatusBadge>(c::StatusBadge::Online, root);
    camBadge->setLabel("Cameras Active");
    camBadge->setAligment(LV_ALIGN_TOP_LEFT, 120, 400);

    auto alarmBadge = std::make_unique<c::StatusBadge>(c::StatusBadge::Offline, root);
    alarmBadge->setLabel("Alarm Disarmed");
    alarmBadge->setAligment(LV_ALIGN_TOP_LEFT, 250, 400);
}

static void buildRoomsPage(w::BaseItem *root)
{
    // Scrollable container
    auto scroll = std::make_unique<w::ScrollItem>(root);
    scroll->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);

    auto title = std::make_unique<w::LvText>("Rooms", CLR_ON_SURFACE, scroll.get(), w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    struct { const char *n; const char *i; int d; float t; float h; } rooms[] = {
        {"Living Room","Sofa",4,22.5f,48},{"Bedroom","Bed",3,20,52},
        {"Kitchen","Cook",5,23,55},{"Bathroom","Bath",2,24,60},
        {"Hallway","Door",1,21,50},{"Garage","Car",2,19,45},
    };

    for (int i = 0; i < 6; i++) {
        auto card = std::make_unique<c::RoomCard>(rooms[i].n, rooms[i].i, rooms[i].d, rooms[i].t, rooms[i].h, scroll.get());
        card->setAligment(LV_ALIGN_TOP_LEFT, 16, 45 + i * 145);
        card->setOnTap([name = std::string(rooms[i].n)]() {
            w::Toast::success(("Opening " + name).c_str());
        });
    }
}

static void buildDevicesPage(w::BaseItem *root)
{
    auto scroll = std::make_unique<w::ScrollItem>(root);
    scroll->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);
    scroll->setScrollable(true);

    auto title = std::make_unique<w::LvText>("Devices", CLR_ON_SURFACE, scroll.get(), w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    int y = 42;
    // Lights section
    auto lightsHdr = std::make_unique<w::LvText>("Lights", CLR_ON_SURFACE_VARIANT_1, scroll.get(), w::LvText::FontSize22Bold);
    lightsHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto ceilingLight = std::make_unique<c::LightWidget>("Ceiling Light", scroll.get());
    ceilingLight->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 170;

    auto floorLamp = std::make_unique<c::DeviceCard>("Floor Lamp", "L", true, scroll.get());
    floorLamp->setSize(360, 56);
    floorLamp->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    floorLamp->setSubtitle("Living Room"); y += 64;

    // Thermostats
    auto thermHdr = std::make_unique<w::LvText>("Climate", CLR_ON_SURFACE_VARIANT_1, scroll.get(), w::LvText::FontSize22Bold);
    thermHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto thermostat = std::make_unique<c::ThermostatWidget>(22.0f, 21.0f, scroll.get());
    thermostat->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 210;

    // Security
    auto secHdr = std::make_unique<w::LvText>("Security", CLR_ON_SURFACE_VARIANT_1, scroll.get(), w::LvText::FontSize22Bold);
    secHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto frontDoor = std::make_unique<c::DeviceCard>("Front Door", "D", true, scroll.get());
    frontDoor->setSize(360, 56);
    frontDoor->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    frontDoor->setSubtitle("Locked"); y += 64;

    auto garageDoor = std::make_unique<c::DeviceCard>("Garage Door", "G", false, scroll.get());
    garageDoor->setSize(360, 56);
    garageDoor->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    garageDoor->setSubtitle("Open"); y += 64;

    auto motionSensor = std::make_unique<c::DeviceCard>("Motion Sensor", "M", true, scroll.get());
    motionSensor->setSize(360, 56);
    motionSensor->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    motionSensor->setSubtitle("Hallway - No motion"); y += 64;
}

static void buildScenesPage(w::BaseItem *root)
{
    auto title = std::make_unique<w::LvText>("Scenes", CLR_ON_SURFACE, root, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    auto activeLabel = std::make_unique<w::LvText>("Active: None", CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize20);
    activeLabel->setAligment(LV_ALIGN_TOP_RIGHT, -16, 12);

    struct { const char *icon; const char *name; const char *desc; } scenes[] = {
        {"Sun",  "Good Morning", "Lights on, blinds open"},
        {"Moon", "Good Night",   "All off, doors locked"},
        {"Car",  "Away Mode",    "All off, cameras on"},
        {"Film", "Movie Time",   "Lights dim, TV on"},
    };

    int x = 16, y = 45;
    for (int i = 0; i < 4; i++) {
        auto btn = std::make_unique<c::SceneButton>(scenes[i].icon, scenes[i].name, scenes[i].desc, root);
        btn->setAligment(LV_ALIGN_TOP_LEFT, x, y);
        btn->setOnActivate([&al = activeLabel, name = std::string(scenes[i].name)]() {
            al->setText("Active: " + name);
            w::Toast::success((name + " activated").c_str());
        });
        x += 190;
    }
}

static void buildSettingsPage(w::BaseItem *root)
{
    auto scroll = std::make_unique<w::ScrollItem>(root);
    scroll->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);
    scroll->setScrollable(true);

    auto title = std::make_unique<w::LvText>("Settings", CLR_ON_SURFACE, scroll.get(), w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    int y = 45;

    // ---- Appearance ----
    auto appHdr = std::make_unique<w::LvText>("Appearance", CLR_PRIMARY, scroll.get(), w::LvText::FontSize22Bold);
    appHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto themeLbl = std::make_unique<w::LvText>("Theme", CLR_ON_SURFACE_VARIANT_0, scroll.get(), w::LvText::FontSize20);
    themeLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto themeDrop = std::make_unique<w::Dropdown>(scroll.get());
    themeDrop->setSize(180, 36);
    themeDrop->setOptions("Dark\nLight\nAuto");
    themeDrop->setAligment(LV_ALIGN_TOP_LEFT, 200, y - 3); y += 44;

    auto brightLbl = std::make_unique<w::LvText>("Brightness", CLR_ON_SURFACE_VARIANT_0, scroll.get(), w::LvText::FontSize20);
    brightLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto brightSlider = std::make_unique<w::Slider>(scroll.get());
    brightSlider->setSize(180, 20);
    brightSlider->setRange(10, 100);
    brightSlider->setValue(80);
    brightSlider->setAligment(LV_ALIGN_TOP_LEFT, 200, y + 3); y += 30;

    // ---- Language ----
    auto langHdr = std::make_unique<w::LvText>("Language", CLR_PRIMARY, scroll.get(), w::LvText::FontSize22Bold);
    langHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto langLbl = std::make_unique<w::LvText>("Language", CLR_ON_SURFACE_VARIANT_0, scroll.get(), w::LvText::FontSize20);
    langLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto langRoller = std::make_unique<w::Roller>(scroll.get());
    langRoller->setSize(150, 100);
    langRoller->setOptions("English\nChinese\nJapanese\nRussian\nArabic", LV_ROLLER_MODE_INFINITE);
    langRoller->setVisibleRowCount(3);
    langRoller->setAligment(LV_ALIGN_TOP_LEFT, 200, y - 5); y += 110;

    // ---- Network ----
    auto netHdr = std::make_unique<w::LvText>("Network", CLR_PRIMARY, scroll.get(), w::LvText::FontSize22Bold);
    netHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto wifiLbl = std::make_unique<w::LvText>("WiFi", CLR_ON_SURFACE_VARIANT_0, scroll.get(), w::LvText::FontSize20);
    wifiLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto wifiSwitch = std::make_unique<w::SwitchButton>(48, 24, scroll.get());
    wifiSwitch->setCheck(true);
    wifiSwitch->setAligment(LV_ALIGN_TOP_LEFT, 200, y); y += 30;

    auto wifiInfo = std::make_unique<w::LvText>("SSID: MyHomeWiFi  IP: 192.168.1.100", CLR_ON_SURFACE_VARIANT_2, scroll.get(), w::LvText::FontSize20);
    wifiInfo->setAligment(LV_ALIGN_TOP_LEFT, 32, y); y += 28;

    // ---- Notifications ----
    auto notifHdr = std::make_unique<w::LvText>("Notifications", CLR_PRIMARY, scroll.get(), w::LvText::FontSize22Bold);
    notifHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto pushLbl = std::make_unique<w::LvText>("Push Notifications", CLR_ON_SURFACE_VARIANT_0, scroll.get(), w::LvText::FontSize20);
    pushLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto pushSwitch = std::make_unique<w::SwitchButton>(48, 24, scroll.get());
    pushSwitch->setCheck(true);
    pushSwitch->setAligment(LV_ALIGN_TOP_LEFT, 200, y); y += 30;

    // ---- About ----
    auto aboutHdr = std::make_unique<w::LvText>("About", CLR_PRIMARY, scroll.get(), w::LvText::FontSize22Bold);
    aboutHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y); y += 30;

    auto ver = std::make_unique<w::LvText>("LVGL++ Smart Home v2.0", CLR_ON_SURFACE_VARIANT_1, scroll.get(), w::LvText::FontSize20);
    ver->setAligment(LV_ALIGN_TOP_LEFT, 32, y); y += 24;

    auto build = std::make_unique<w::LvText>("Build: 2026-07-23  |  SDK: LVGL 9.x", CLR_ON_SURFACE_VARIANT_2, scroll.get(), w::LvText::FontSize20);
    build->setAligment(LV_ALIGN_TOP_LEFT, 32, y); y += 24;

    auto copy = std::make_unique<w::LvText>("(c) 2024-2026 LVGL++ Team", CLR_PRIMARY_DIS_BG, scroll.get(), w::LvText::FontSize20);
    copy->setAligment(LV_ALIGN_TOP_LEFT, 32, y); y += 28;

    auto aboutBtn = std::make_unique<w::PushButton>(140, 36, w::PushButton::Blue, "View License", scroll.get());
    aboutBtn->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
}

// ============================================================
// Main shell with bottom navigation
// ============================================================

class SmartHomeShell : public lvglpp::sys::BaseActivity
{
public:
    SmartHomeShell() = default;

protected:
    void onCreate(std::any arg) override
    {
        lvglpp::sys::BaseActivity::onCreate(arg);
        auto *root = lvglpp::sys::BaseActivity::getRoot();
        root->setBgColor(CLR_SURFACE);
        root->setSize(WIN_WIDTH, WIN_HEIGHT);

        // Build all 5 page containers
        _pages.push_back(buildPageContainer(root, buildDashboard));
        _pages.push_back(buildPageContainer(root, buildRoomsPage));
        _pages.push_back(buildPageContainer(root, buildDevicesPage));
        _pages.push_back(buildPageContainer(root, buildScenesPage));
        _pages.push_back(buildPageContainer(root, buildSettingsPage));

        // Bottom nav bar
        _navBar = std::make_unique<w::BaseItem>(root);
        _navBar->setSize(WIN_WIDTH, 60);
        _navBar->setBgColor(CLR_SURFACE_CONTAINER);
        _navBar->setAligment(LV_ALIGN_BOTTOM_MID, 0, 0);

        const char *tabLabels[] = {"Home", "Rooms", "Devices", "Scenes", "Settings"};
        int tabW = WIN_WIDTH / 5;
        for (int i = 0; i < 5; i++) {
            auto btn = std::make_unique<w::PushButton>(
                tabW - 4, 54,
                i == 0 ? w::PushButton::Blue : w::PushButton::Dark,
                tabLabels[i], _navBar.get());
            btn->setAligment(LV_ALIGN_LEFT_MID, i * tabW + 2, 0);

            int idx = i;
            btn->setOnClickedListener([this, idx]() { showPage(idx); });
            _navBtns.push_back(std::move(btn));
        }

        showPage(0);
    }

    void onNotifyUI(const lvglpp::sys::Event &) override {}
    void onDestroy() override {}

private:
    using PageBuilder = void(*)(w::BaseItem *);

    w::BaseItem *buildPageContainer(w::BaseItem *root, PageBuilder builder)
    {
        auto *container = new w::BaseItem(root);
        container->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
        container->setAligment(LV_ALIGN_TOP_MID, 0, 0);
        builder(container);
        container->setVisible(false);
        return container;
    }

    void showPage(int index)
    {
        for (size_t i = 0; i < _pages.size(); i++) {
            _pages[i]->setVisible(static_cast<int>(i) == index);
        }
        for (size_t i = 0; i < _navBtns.size(); i++) {
            _navBtns[i]->setColorType(
                static_cast<int>(i) == index
                    ? w::PushButton::Blue : w::PushButton::Dark);
        }
    }

    std::unique_ptr<w::BaseItem> _navBar;
    std::vector<w::BaseItem *> _pages;
    std::vector<std::unique_ptr<w::PushButton>> _navBtns;
};

int main(int argc, char *argv[])
{
    lvglpp::sys::SdlApplication app(argc, argv);

    if (!app.initApp()) {
        return -1;
    }

    LvTranslator::Instance().init();
    LvTranslator::Instance().load(LANGUAGE::ENGLISH);

    auto shell = std::make_shared<SmartHomeShell>();
    lvglpp::sys::StackView::getInstance().pushView(shell);

    return app.exec();
}
