/**
 * Smart Home UI — LVGL++ Application
 *
 * Bottom navigation bar with 5 tabs. Content pages are built directly
 * inside a container and swapped via visibility toggling.
 */

#include "components/DeviceCard.h"
#include "components/EnergyChart.h"
#include "components/LightWidget.h"
#include "components/RoomCard.h"
#include "components/SceneButton.h"
#include "components/StatusBadge.h"
#include "components/ThermostatWidget.h"
#include "core/sys/Application.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/StackView.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/Dropdown.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PageHost.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/Roller.h"
#include "core/widgets/ScrollItem.h"
#include "core/widgets/Slider.h"
#include "core/widgets/SwitchButton.h"
#include "core/widgets/Toast.h"
#include "core/widgets/WidgetStore.h"
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

static void buildDashboard(w::BaseItem *root, w::WidgetStore &store)
{
    // Title
    auto *title = store.emplace<w::LvText>("Smart Home", CLR_ON_SURFACE, root, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    // Weather
    auto *weather = store.emplace<w::LvText>("Sunny  72F  45%", CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize22);
    weather->setAligment(LV_ALIGN_TOP_RIGHT, -16, 12);

    // Quick actions row
    auto *lightsOff = store.emplace<w::PushButton>(110, 32, w::PushButton::Dark, "Lights Off", root);
    lightsOff->setAligment(LV_ALIGN_TOP_LEFT, 16, 42);
    lightsOff->setOnClickedListener([]() { w::Toast::success("All lights off"); });

    auto *lockAll = store.emplace<w::PushButton>(110, 32, w::PushButton::Dark, "Lock All", root);
    lockAll->setAligment(LV_ALIGN_TOP_LEFT, 134, 42);
    lockAll->setOnClickedListener([]() { w::Toast::success("All doors locked"); });

    auto *awayBtn = store.emplace<w::PushButton>(100, 32, w::PushButton::Red, "Away", root);
    awayBtn->setAligment(LV_ALIGN_TOP_LEFT, 252, 42);
    awayBtn->setOnClickedListener([]() { w::Toast::warn("Away mode"); });

    // Room cards (2x2 grid)
    struct {
        const char *n;
        const char *i;
        int         d;
        float       t;
        float       h;
    } rooms[] = {
        {"Living Room", "Sofa", 4, 22.5f, 48},
        {"Bedroom", "Bed", 3, 20, 52},
        {"Kitchen", "Cook", 5, 23, 55},
        {"Bathroom", "Bath", 2, 24, 60},
    };
    int pos[4][2] = {{16, 85}, {208, 85}, {16, 225}, {208, 225}};
    for (int i = 0; i < 4; i++)
    {
        auto *card = store.emplace<c::RoomCard>(rooms[i].n, rooms[i].i, rooms[i].d, rooms[i].t, rooms[i].h, root);
        card->setAligment(LV_ALIGN_TOP_LEFT, pos[i][0], pos[i][1]);
    }

    // Energy chart
    auto *chart = store.emplace<c::EnergyChart>("Today", root);
    chart->setSize(380, 200);
    chart->setAligment(LV_ALIGN_TOP_RIGHT, -16, 85);
    chart->setTotal(12.4f);
    chart->setData({8, 12, 15, 20, 25, 30, 18}, {});

    // Security status
    auto *secTitle = store.emplace<w::LvText>("Security", CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize22Bold);
    secTitle->setAligment(LV_ALIGN_TOP_LEFT, 16, 370);

    auto *doorBadge = store.emplace<c::StatusBadge>(c::StatusBadge::Online, root);
    doorBadge->setLabel("Doors Locked");
    doorBadge->setAligment(LV_ALIGN_TOP_LEFT, 16, 400);

    auto *camBadge = store.emplace<c::StatusBadge>(c::StatusBadge::Online, root);
    camBadge->setLabel("Cameras Active");
    camBadge->setAligment(LV_ALIGN_TOP_LEFT, 120, 400);

    auto *alarmBadge = store.emplace<c::StatusBadge>(c::StatusBadge::Offline, root);
    alarmBadge->setLabel("Alarm Disarmed");
    alarmBadge->setAligment(LV_ALIGN_TOP_LEFT, 250, 400);
}

static void buildRoomsPage(w::BaseItem *root, w::WidgetStore &store)
{
    // Scrollable container
    auto *scroll = store.emplace<w::ScrollItem>(root);
    scroll->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);

    auto *title = store.emplace<w::LvText>("Rooms", CLR_ON_SURFACE, scroll, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    struct {
        const char *n;
        const char *i;
        int         d;
        float       t;
        float       h;
    } rooms[] = {
        {"Living Room", "Sofa", 4, 22.5f, 48}, {"Bedroom", "Bed", 3, 20, 52},  {"Kitchen", "Cook", 5, 23, 55},
        {"Bathroom", "Bath", 2, 24, 60},       {"Hallway", "Door", 1, 21, 50}, {"Garage", "Car", 2, 19, 45},
    };

    for (int i = 0; i < 6; i++)
    {
        auto *card = store.emplace<c::RoomCard>(rooms[i].n, rooms[i].i, rooms[i].d, rooms[i].t, rooms[i].h, scroll);
        card->setAligment(LV_ALIGN_TOP_LEFT, 16, 45 + i * 145);
        card->setOnTap([name = std::string(rooms[i].n)]() { w::Toast::success(("Opening " + name).c_str()); });
    }
}

static void buildDevicesPage(w::BaseItem *root, w::WidgetStore &store)
{
    auto *scroll = store.emplace<w::ScrollItem>(root);
    scroll->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);
    scroll->setScrollable(true);

    auto *title = store.emplace<w::LvText>("Devices", CLR_ON_SURFACE, scroll, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    int y = 42;
    // Lights section
    auto *lightsHdr = store.emplace<w::LvText>("Lights", CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize22Bold);
    lightsHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *ceilingLight = store.emplace<c::LightWidget>("Ceiling Light", scroll);
    ceilingLight->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 170;

    auto *floorLamp = store.emplace<c::DeviceCard>("Floor Lamp", "L", true, scroll);
    floorLamp->setSize(360, 56);
    floorLamp->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    floorLamp->setSubtitle("Living Room");
    y += 64;

    // Thermostats
    auto *thermHdr = store.emplace<w::LvText>("Climate", CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize22Bold);
    thermHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *thermostat = store.emplace<c::ThermostatWidget>(22.0f, 21.0f, scroll);
    thermostat->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 210;

    // Security
    auto *secHdr = store.emplace<w::LvText>("Security", CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize22Bold);
    secHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *frontDoor = store.emplace<c::DeviceCard>("Front Door", "D", true, scroll);
    frontDoor->setSize(360, 56);
    frontDoor->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    frontDoor->setSubtitle("Locked");
    y += 64;

    auto *garageDoor = store.emplace<c::DeviceCard>("Garage Door", "G", false, scroll);
    garageDoor->setSize(360, 56);
    garageDoor->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    garageDoor->setSubtitle("Open");
    y += 64;

    auto *motionSensor = store.emplace<c::DeviceCard>("Motion Sensor", "M", true, scroll);
    motionSensor->setSize(360, 56);
    motionSensor->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    motionSensor->setSubtitle("Hallway - No motion");
    y += 64;
}

static void buildScenesPage(w::BaseItem *root, w::WidgetStore &store)
{
    auto *title = store.emplace<w::LvText>("Scenes", CLR_ON_SURFACE, root, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    auto *activeLabel = store.emplace<w::LvText>("Active: None", CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize20);
    activeLabel->setAligment(LV_ALIGN_TOP_RIGHT, -16, 12);

    struct {
        const char *icon;
        const char *name;
        const char *desc;
    } scenes[] = {
        {"Sun", "Good Morning", "Lights on, blinds open"},
        {"Moon", "Good Night", "All off, doors locked"},
        {"Car", "Away Mode", "All off, cameras on"},
        {"Film", "Movie Time", "Lights dim, TV on"},
    };

    int x = 16, y = 45;
    for (int i = 0; i < 4; i++)
    {
        auto *btn = store.emplace<c::SceneButton>(scenes[i].icon, scenes[i].name, scenes[i].desc, root);
        btn->setAligment(LV_ALIGN_TOP_LEFT, x, y);
        btn->setOnActivate([activeLabel, name = std::string(scenes[i].name)]() {
            activeLabel->setText("Active: " + name);
            w::Toast::success((name + " activated").c_str());
        });
        x += 190;
    }
}

static void buildSettingsPage(w::BaseItem *root, w::WidgetStore &store)
{
    auto *scroll = store.emplace<w::ScrollItem>(root);
    scroll->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);
    scroll->setScrollable(true);

    auto *title = store.emplace<w::LvText>("Settings", CLR_ON_SURFACE, scroll, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    int y = 45;

    // ---- Appearance ----
    auto *appHdr = store.emplace<w::LvText>("Appearance", CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    appHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *themeLbl = store.emplace<w::LvText>("Theme", CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    themeLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *themeDrop = store.emplace<w::Dropdown>(scroll);
    themeDrop->setSize(180, 36);
    themeDrop->setOptions("Dark\nLight\nAuto");
    themeDrop->setAligment(LV_ALIGN_TOP_LEFT, 200, y - 3);
    y += 44;

    auto *brightLbl = store.emplace<w::LvText>("Brightness", CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    brightLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *brightSlider = store.emplace<w::Slider>(scroll);
    brightSlider->setSize(180, 20);
    brightSlider->setRange(10, 100);
    brightSlider->setValue(80);
    brightSlider->setAligment(LV_ALIGN_TOP_LEFT, 200, y + 3);
    y += 30;

    // ---- Language ----
    auto *langHdr = store.emplace<w::LvText>("Language", CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    langHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *langLbl = store.emplace<w::LvText>("Language", CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    langLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *langRoller = store.emplace<w::Roller>(scroll);
    langRoller->setSize(150, 100);
    langRoller->setOptions("English\nChinese\nJapanese\nRussian\nArabic", LV_ROLLER_MODE_INFINITE);
    langRoller->setVisibleRowCount(3);
    langRoller->setAligment(LV_ALIGN_TOP_LEFT, 200, y - 5);
    y += 110;

    // ---- Network ----
    auto *netHdr = store.emplace<w::LvText>("Network", CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    netHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *wifiLbl = store.emplace<w::LvText>("WiFi", CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    wifiLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *wifiSwitch = store.emplace<w::SwitchButton>(48, 24, scroll);
    wifiSwitch->setCheck(true);
    wifiSwitch->setAligment(LV_ALIGN_TOP_LEFT, 200, y);
    y += 30;

    auto *wifiInfo = store.emplace<w::LvText>("SSID: MyHomeWiFi  IP: 192.168.1.100", CLR_ON_SURFACE_VARIANT_2, scroll, w::LvText::FontSize20);
    wifiInfo->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 28;

    // ---- Notifications ----
    auto *notifHdr = store.emplace<w::LvText>("Notifications", CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    notifHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *pushLbl = store.emplace<w::LvText>("Push Notifications", CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    pushLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *pushSwitch = store.emplace<w::SwitchButton>(48, 24, scroll);
    pushSwitch->setCheck(true);
    pushSwitch->setAligment(LV_ALIGN_TOP_LEFT, 200, y);
    y += 30;

    // ---- About ----
    auto *aboutHdr = store.emplace<w::LvText>("About", CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    aboutHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *ver = store.emplace<w::LvText>("LVGL++ Smart Home v2.0", CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize20);
    ver->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 24;

    auto *build = store.emplace<w::LvText>("Build: 2026-07-23  |  SDK: LVGL 9.x", CLR_ON_SURFACE_VARIANT_2, scroll, w::LvText::FontSize20);
    build->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 24;

    auto *copy = store.emplace<w::LvText>("(c) 2024-2026 LVGL++ Team", CLR_PRIMARY_DIS_BG, scroll, w::LvText::FontSize20);
    copy->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 28;

    auto *aboutBtn = store.emplace<w::PushButton>(140, 36, w::PushButton::Blue, "View License", scroll);
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

        // Pages are built lazily on first show; visited pages are kept alive.
        _pageHost = std::make_unique<w::PageHost>(root);
        _pageHost->setSize(WIN_WIDTH, WIN_HEIGHT - 60);
        _pageHost->setAligment(LV_ALIGN_TOP_MID, 0, 0);

        _pageHost->addPage("Home", [](w::PageContext &ctx) { buildDashboard(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Rooms", [](w::PageContext &ctx) { buildRoomsPage(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Devices", [](w::PageContext &ctx) { buildDevicesPage(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Scenes", [](w::PageContext &ctx) { buildScenesPage(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Settings", [](w::PageContext &ctx) { buildSettingsPage(ctx.root(), ctx.widgets()); });

        // Bottom nav bar
        _navBar = std::make_unique<w::BaseItem>(root);
        _navBar->setSize(WIN_WIDTH, 60);
        _navBar->setBgColor(CLR_SURFACE_CONTAINER);
        _navBar->setAligment(LV_ALIGN_BOTTOM_MID, 0, 0);

        const char *tabLabels[] = {"Home", "Rooms", "Devices", "Scenes", "Settings"};
        int         tabW        = WIN_WIDTH / 5;
        for (int i = 0; i < 5; i++)
        {
            auto btn = std::make_unique<w::PushButton>(tabW - 4, 54, i == 0 ? w::PushButton::Blue : w::PushButton::Dark, tabLabels[i], _navBar.get());
            btn->setAligment(LV_ALIGN_LEFT_MID, i * tabW + 2, 0);

            int idx = i;
            btn->setOnClickedListener([this, idx]() { showPage(idx); });
            _navBtns.push_back(std::move(btn));
        }

        showPage(0);
    }

    void onNotifyUI(const lvglpp::sys::Event &) override
    {}
    void onDestroy() override
    {}

private:
    void showPage(int index)
    {
        if (!_pageHost)
        {
            return;
        }

        _pageHost->show(index);

        for (size_t i = 0; i < _navBtns.size(); i++)
        {
            _navBtns[i]->setColorType(static_cast<int>(i) == index ? w::PushButton::Blue : w::PushButton::Dark);
        }
    }

    std::unique_ptr<w::BaseItem>                _navBar;
    std::vector<std::unique_ptr<w::PushButton>> _navBtns;
    // Declared last: destroys page widgets before the nav wrappers.
    std::unique_ptr<w::PageHost> _pageHost;
};

int main(int argc, char *argv[])
{
    auto app = lvglpp::sys::createApplication(argc, argv);

    if (!app || !app->initialize())
    {
        return -1;
    }

    LvTranslator::Instance().init();
    LvTranslator::Instance().load(LANGUAGE::ENGLISH);

    auto shell = std::make_shared<SmartHomeShell>();
    lvglpp::sys::StackView::getInstance().pushView(shell);

    return app->exec();
}
