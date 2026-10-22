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
#include "core/sys/InputManager.h"
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
#include "core/sys/UiConfig.h"
#include "core/widgets/WidgetStore.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <algorithm>
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
    auto *title = store.emplace<w::LvText>(lvTr("Home.Title"), CLR_ON_SURFACE, root, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    // Weather
    auto *weather = store.emplace<w::LvText>(lvTr("Home.Weather"), CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize22);
    weather->setAligment(LV_ALIGN_TOP_RIGHT, -16, 12);

    // Quick actions row
    auto *lightsOff = store.emplace<w::PushButton>(110, 32, w::PushButton::Dark, lvTr("Home.LightsOff"), root);
    lightsOff->setAligment(LV_ALIGN_TOP_LEFT, 16, 42);
    lightsOff->setOnClickedListener([]() { w::Toast::success(lvTr("Home.AllLightsOff")); });

    auto *lockAll = store.emplace<w::PushButton>(110, 32, w::PushButton::Dark, lvTr("Home.LockAll"), root);
    lockAll->setAligment(LV_ALIGN_TOP_LEFT, 134, 42);
    lockAll->setOnClickedListener([]() { w::Toast::success(lvTr("Home.AllDoorsLocked")); });

    auto *awayBtn = store.emplace<w::PushButton>(100, 32, w::PushButton::Red, lvTr("Home.Away"), root);
    awayBtn->setAligment(LV_ALIGN_TOP_LEFT, 252, 42);
    awayBtn->setOnClickedListener([]() { w::Toast::warn(lvTr("Home.AwayMode")); });

    // Room cards (2x2 grid)
    struct {
        const char *n;
        const char *i;
        int         d;
        float       t;
        float       h;
    } rooms[] = {
        {lvTr("Room.LivingRoom"), lvTr("Room.Icon.Sofa"), 4, 22.5f, 48},
        {lvTr("Room.Bedroom"), lvTr("Room.Icon.Bed"), 3, 20, 52},
        {lvTr("Room.Kitchen"), lvTr("Room.Icon.Cook"), 5, 23, 55},
        {lvTr("Room.Bathroom"), lvTr("Room.Icon.Bath"), 2, 24, 60},
    };
    int pos[4][2] = {{16, 85}, {208, 85}, {16, 225}, {208, 225}};
    for (int i = 0; i < 4; i++)
    {
        auto *card = store.emplace<c::RoomCard>(rooms[i].n, rooms[i].i, rooms[i].d, rooms[i].t, rooms[i].h, root);
        card->setAligment(LV_ALIGN_TOP_LEFT, pos[i][0], pos[i][1]);
    }

    const auto &cfg = sys::UiConfig::instance();
#if LVGLPP_ENABLE_ROTATION
    const bool portrait = cfg.isPortrait();
#else
    constexpr bool portrait = false;
#endif

    // Energy chart: right of the cards in landscape, below them in portrait.
    auto *chart = store.emplace<c::EnergyChart>(lvTr("Home.Today"), root);
    chart->setSize(portrait ? std::min(380, cfg.width() - 32) : 380, 200);
    if (portrait)
    {
        chart->setAligment(LV_ALIGN_TOP_MID, 0, 370);
    } else
    {
        chart->setAligment(LV_ALIGN_TOP_RIGHT, -16, 85);
    }
    chart->setTotal(12.4f);
    chart->setData({8, 12, 15, 20, 25, 30, 18}, {});

    // Security status
    const int securityY = portrait ? 580 : 370;
    auto *secTitle = store.emplace<w::LvText>(lvTr("Home.Security"), CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize22Bold);
    secTitle->setAligment(LV_ALIGN_TOP_LEFT, 16, securityY);

    auto *doorBadge = store.emplace<c::StatusBadge>(c::StatusBadge::Online, root);
    doorBadge->setLabel(lvTr("Home.DoorsLocked"));
    doorBadge->setAligment(LV_ALIGN_TOP_LEFT, 16, securityY + 30);

    auto *camBadge = store.emplace<c::StatusBadge>(c::StatusBadge::Online, root);
    camBadge->setLabel(lvTr("Home.CamerasActive"));
    camBadge->setAligment(LV_ALIGN_TOP_LEFT, 120, securityY + 30);

    auto *alarmBadge = store.emplace<c::StatusBadge>(c::StatusBadge::Offline, root);
    alarmBadge->setLabel(lvTr("Home.AlarmDisarmed"));
    alarmBadge->setAligment(LV_ALIGN_TOP_LEFT, 250, securityY + 30);
}

static void buildRoomsPage(w::BaseItem *root, w::WidgetStore &store)
{
    const auto &cfg = sys::UiConfig::instance();

    // Scrollable container
    auto *scroll = store.emplace<w::ScrollItem>(root);
    scroll->setSize(cfg.width(), cfg.height() - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);

    auto *title = store.emplace<w::LvText>(lvTr("Nav.Rooms"), CLR_ON_SURFACE, scroll, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    struct {
        const char *n;
        const char *i;
        int         d;
        float       t;
        float       h;
    } rooms[] = {
        {lvTr("Room.LivingRoom"), lvTr("Room.Icon.Sofa"), 4, 22.5f, 48},
        {lvTr("Room.Bedroom"), lvTr("Room.Icon.Bed"), 3, 20, 52},
        {lvTr("Room.Kitchen"), lvTr("Room.Icon.Cook"), 5, 23, 55},
        {lvTr("Room.Bathroom"), lvTr("Room.Icon.Bath"), 2, 24, 60},
        {lvTr("Room.Hallway"), lvTr("Room.Icon.Door"), 1, 21, 50},
        {lvTr("Room.Garage"), lvTr("Room.Icon.Car"), 2, 19, 45},
    };

#if LVGLPP_ENABLE_ROTATION
    const int columns = cfg.isPortrait() ? 2 : 3;
#else
    const int columns = 3;
#endif
    const int cardW   = 180;
    const int cardH   = 130;
    const int gap     = 16;
    const int gridW   = columns * cardW + (columns - 1) * gap;
    const int startX  = std::max(0, (cfg.width() - gridW) / 2);

    for (int i = 0; i < 6; i++)
    {
        auto *card = store.emplace<c::RoomCard>(rooms[i].n, rooms[i].i, rooms[i].d, rooms[i].t, rooms[i].h, scroll);
        const int col = i % columns;
        const int row = i / columns;
        card->setAligment(LV_ALIGN_TOP_LEFT, startX + col * (cardW + gap), 45 + row * (cardH + gap));
        card->setOnTap([name = std::string(rooms[i].n)]() {
            w::Toast::success(lvTrFmt("Common.Opening", name).c_str());
        });
    }
}

static void buildDevicesPage(w::BaseItem *root, w::WidgetStore &store)
{
    const auto &cfg = sys::UiConfig::instance();
#if LVGLPP_ENABLE_ROTATION
    const bool portrait = cfg.isPortrait();
#else
    constexpr bool portrait = false;
#endif
    const lv_align_t cardAlign = portrait ? LV_ALIGN_TOP_MID : LV_ALIGN_TOP_LEFT;
    const lv_coord_t cardX     = portrait ? 0 : 16;

    auto *scroll = store.emplace<w::ScrollItem>(root);
    scroll->setSize(cfg.width(), cfg.height() - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);
    scroll->setScrollable(true);

    auto *title = store.emplace<w::LvText>(lvTr("Nav.Devices"), CLR_ON_SURFACE, scroll, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    int y = 42;
    // Lights section
    auto *lightsHdr = store.emplace<w::LvText>(lvTr("Home.Lights"), CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize22Bold);
    lightsHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *ceilingLight = store.emplace<c::LightWidget>(lvTr("Home.CeilingLight"), scroll);
    ceilingLight->setAligment(cardAlign, cardX, y);
    y += 170;

    auto *floorLamp = store.emplace<c::DeviceCard>(lvTr("Home.FloorLamp"), "L", true, scroll);
    floorLamp->setSize(360, 56);
    floorLamp->setAligment(cardAlign, cardX, y);
    floorLamp->setSubtitle(lvTr("Room.LivingRoom"));
    y += 64;

    // Thermostats
    auto *thermHdr = store.emplace<w::LvText>(lvTr("Home.Climate"), CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize22Bold);
    thermHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *thermostat = store.emplace<c::ThermostatWidget>(22.0f, 21.0f, scroll);
    thermostat->setAligment(cardAlign, cardX, y);
    y += 210;

    // Security
    auto *secHdr = store.emplace<w::LvText>(lvTr("Home.Security"), CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize22Bold);
    secHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *frontDoor = store.emplace<c::DeviceCard>(lvTr("Home.FrontDoor"), "D", true, scroll);
    frontDoor->setSize(360, 56);
    frontDoor->setAligment(cardAlign, cardX, y);
    frontDoor->setSubtitle(lvTr("Common.Locked"));
    y += 64;

    auto *garageDoor = store.emplace<c::DeviceCard>(lvTr("Home.GarageDoor"), "G", false, scroll);
    garageDoor->setSize(360, 56);
    garageDoor->setAligment(cardAlign, cardX, y);
    garageDoor->setSubtitle(lvTr("Common.Open"));
    y += 64;

    auto *motionSensor = store.emplace<c::DeviceCard>(lvTr("Home.MotionSensor"), "M", true, scroll);
    motionSensor->setSize(360, 56);
    motionSensor->setAligment(cardAlign, cardX, y);
    motionSensor->setSubtitle(lvTr("Home.HallwayNoMotion"));
    y += 64;
}

static void buildScenesPage(w::BaseItem *root, w::WidgetStore &store)
{
    auto *title = store.emplace<w::LvText>(lvTr("Nav.Scenes"), CLR_ON_SURFACE, root, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    auto *activeLabel = store.emplace<w::LvText>(lvTr("Scenes.Active"), CLR_ON_SURFACE_VARIANT_1, root, w::LvText::FontSize20);
    activeLabel->setAligment(LV_ALIGN_TOP_RIGHT, -16, 12);

    struct {
        const char *icon;
        const char *name;
        const char *desc;
    } scenes[] = {
        // Icon strings are symbolic glyphs, not translatable copy.
        {"Sun", lvTr("Scene.GoodMorning"), lvTr("Scene.GoodMorning.Desc")},
        {"Moon", lvTr("Scene.GoodNight"), lvTr("Scene.GoodNight.Desc")},
        {"Car", lvTr("Scene.AwayMode"), lvTr("Scene.AwayMode.Desc")},
        {"Film", lvTr("Scene.MovieTime"), lvTr("Scene.MovieTime.Desc")},
    };

    const auto &cfg      = sys::UiConfig::instance();
#if LVGLPP_ENABLE_ROTATION
    const int   columns  = cfg.isPortrait() ? 2 : 4;
#else
    const int   columns  = 4;
#endif
    const int   cardW    = 170;
    const int   cardH    = 120;
    const int   gap      = 16;
    const int   gridW    = columns * cardW + (columns - 1) * gap;
    const int   startX   = std::max(0, (cfg.width() - gridW) / 2);

    for (int i = 0; i < 4; i++)
    {
        auto *btn = store.emplace<c::SceneButton>(scenes[i].icon, scenes[i].name, scenes[i].desc, root);
        const int col = i % columns;
        const int row = i / columns;
        btn->setAligment(LV_ALIGN_TOP_LEFT, startX + col * (cardW + gap), 45 + row * (cardH + gap));
        btn->setOnActivate([activeLabel, name = std::string(scenes[i].name)]() {
            activeLabel->setText(lvTrFmt("Common.Active", name));
            w::Toast::success(lvTrFmt("Common.Activated", name).c_str());
        });
    }
}

static int languageRollerIndex()
{
    switch (lvglpp::sys::Application::getLanguageType())
    {
    case lvglpp::sys::Application::English:
        return 0;
    case lvglpp::sys::Application::Chinese:
        return 1;
    case lvglpp::sys::Application::Japanese:
        return 2;
    case lvglpp::sys::Application::Russian:
        return 3;
    case lvglpp::sys::Application::Arabic:
        return 4;
    default:
        return 1;
    }
}

static void onLanguageRollerChanged(lv_event_t *event)
{
    auto *roller = static_cast<lv_obj_t *>(lv_event_get_target(event));
    if (roller == nullptr)
    {
        return;
    }

    static const LANGUAGE languages[] = {
        LANGUAGE::ENGLISH,
        LANGUAGE::CHINESE,
        LANGUAGE::JAPANESE,
        LANGUAGE::RUSSIAN,
        LANGUAGE::AR,
    };
    const uint32_t index = lv_roller_get_selected(roller);
    if (index < sizeof(languages) / sizeof(languages[0]))
    {
        LvTranslator::load(languages[index]);
    }
}

static void buildSettingsPage(w::BaseItem *root, w::WidgetStore &store)
{
    const auto &cfg = sys::UiConfig::instance();

    auto *scroll = store.emplace<w::ScrollItem>(root);
    scroll->setSize(cfg.width(), cfg.height() - 60);
    scroll->setAligment(LV_ALIGN_TOP_MID, 0, 0);
    scroll->setScrollable(true);

    auto *title = store.emplace<w::LvText>(lvTr("Nav.Settings"), CLR_ON_SURFACE, scroll, w::LvText::FontSize28Bold);
    title->setAligment(LV_ALIGN_TOP_LEFT, 16, 8);

    int y = 45;

    // ---- Appearance ----
    auto *appHdr = store.emplace<w::LvText>(lvTr("Settings.Appearance"), CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    appHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *themeLbl = store.emplace<w::LvText>(lvTr("Settings.Theme"), CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    themeLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *themeDrop = store.emplace<w::Dropdown>(scroll);
    themeDrop->setSize(180, 36);
    themeDrop->setOptions(std::string(lvTr("Settings.Theme.Dark")) + "\n" + lvTr("Settings.Theme.Light") + "\n" + lvTr("Settings.Theme.Auto"));
    themeDrop->setAligment(LV_ALIGN_TOP_LEFT, 200, y - 3);
    y += 44;

    auto *brightLbl = store.emplace<w::LvText>(lvTr("Settings.Brightness"), CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    brightLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *brightSlider = store.emplace<w::Slider>(scroll);
    brightSlider->setSize(180, 20);
    brightSlider->setRange(10, 100);
    brightSlider->setValue(80);
    brightSlider->setAligment(LV_ALIGN_TOP_LEFT, 200, y + 3);
    y += 30;

    // ---- Language ----
    auto *langHdr = store.emplace<w::LvText>(lvTr("Language"), CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    langHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *langLbl = store.emplace<w::LvText>(lvTr("Language"), CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    langLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *langRoller = store.emplace<w::Roller>(scroll);
    langRoller->setSize(150, 100);
    const std::string languageOptions = std::string(lvTr("LanguageEnglish")) + "\n" + lvTr("LanguageChinese") + "\n" + lvTr("LanguageJapanese")
                                      + "\n" + lvTr("LanguageRussian") + "\n" + lvTr("LanguageArabic");
    langRoller->setOptions(languageOptions, LV_ROLLER_MODE_INFINITE);
    langRoller->setSelected(static_cast<uint32_t>(languageRollerIndex()));
    langRoller->setVisibleRowCount(3);
    langRoller->setAligment(LV_ALIGN_TOP_LEFT, 200, y - 5);
    langRoller->addLvglEventHandler(onLanguageRollerChanged, LV_EVENT_VALUE_CHANGED, nullptr);
    y += 110;

    // ---- Network ----
    auto *netHdr = store.emplace<w::LvText>(lvTr("Settings.Network"), CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    netHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *wifiLbl = store.emplace<w::LvText>(lvTr("Settings.WiFi"), CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    wifiLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *wifiSwitch = store.emplace<w::SwitchButton>(48, 24, scroll);
    wifiSwitch->setCheck(true);
    wifiSwitch->setAligment(LV_ALIGN_TOP_LEFT, 200, y);
    y += 30;

    // Mock network data, not UI copy.
    auto *wifiInfo = store.emplace<w::LvText>("SSID: MyHomeWiFi  IP: 192.168.1.100", CLR_ON_SURFACE_VARIANT_2, scroll, w::LvText::FontSize20);
    wifiInfo->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 28;

    // ---- Notifications ----
    auto *notifHdr = store.emplace<w::LvText>(lvTr("Settings.Notifications"), CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    notifHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    auto *pushLbl = store.emplace<w::LvText>(lvTr("Settings.PushNotifications"), CLR_ON_SURFACE_VARIANT_0, scroll, w::LvText::FontSize20);
    pushLbl->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    auto *pushSwitch = store.emplace<w::SwitchButton>(48, 24, scroll);
    pushSwitch->setCheck(true);
    pushSwitch->setAligment(LV_ALIGN_TOP_LEFT, 200, y);
    y += 30;

    // ---- About ----
    auto *aboutHdr = store.emplace<w::LvText>(lvTr("Settings.About"), CLR_PRIMARY, scroll, w::LvText::FontSize22Bold);
    aboutHdr->setAligment(LV_ALIGN_TOP_LEFT, 16, y);
    y += 30;

    // Version/build/copyright are product data, not translatable copy.
    auto *ver = store.emplace<w::LvText>("LVGL++ Smart Home v2.0", CLR_ON_SURFACE_VARIANT_1, scroll, w::LvText::FontSize20);
    ver->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 24;

    auto *build = store.emplace<w::LvText>("Build: 2026-07-23  |  SDK: LVGL 9.x", CLR_ON_SURFACE_VARIANT_2, scroll, w::LvText::FontSize20);
    build->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 24;

    auto *copy = store.emplace<w::LvText>("(c) 2024-2026 LVGL++ Team", CLR_PRIMARY_DIS_BG, scroll, w::LvText::FontSize20);
    copy->setAligment(LV_ALIGN_TOP_LEFT, 32, y);
    y += 28;

    auto *aboutBtn = store.emplace<w::PushButton>(140, 36, w::PushButton::Blue, lvTr("Settings.ViewLicense"), scroll);
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
        buildShell(0);
    }

    void onConfigurationChanged(const sys::UiConfiguration &configuration) override
    {
        const int index = _pageHost ? std::max(0, _pageHost->currentIndex()) : 0;

        _navBtns.clear();
        _navBar.reset();
        _pageHost.reset();
        if (auto *root = getRoot(); root != nullptr && root->getLvglItem() != nullptr)
        {
            lv_obj_clean(root->getLvglItem());
        }

        lvglpp::sys::BaseActivity::onConfigurationChanged(configuration);
        buildShell(index);
        refreshFocus();
    }

    void onNotifyUI(const lvglpp::sys::Event &) override
    {}
    void onDestroy() override
    {}

private:
    void buildShell(int initialIndex)
    {
        const auto &cfg  = sys::UiConfig::instance();
        auto       *root = getRoot();
        if (root == nullptr)
        {
            return;
        }

        root->setBgColor(CLR_SURFACE);
        root->setSize(cfg.width(), cfg.height());

        // Pages are built lazily on first show; visited pages are kept alive.
        _pageHost = std::make_unique<w::PageHost>(root);
        _pageHost->setSize(cfg.width(), cfg.height() - 60);
        _pageHost->setAligment(LV_ALIGN_TOP_MID, 0, 0);

        _pageHost->addPage("Home", [](w::PageContext &ctx) { buildDashboard(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Rooms", [](w::PageContext &ctx) { buildRoomsPage(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Devices", [](w::PageContext &ctx) { buildDevicesPage(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Scenes", [](w::PageContext &ctx) { buildScenesPage(ctx.root(), ctx.widgets()); });
        _pageHost->addPage("Settings", [](w::PageContext &ctx) { buildSettingsPage(ctx.root(), ctx.widgets()); });

        // Bottom nav bar. Flex row mirrors automatically under RTL.
        _navBar = std::make_unique<w::BaseItem>(root);
        _navBar->setSize(cfg.width(), 60);
        _navBar->setBgColor(CLR_SURFACE_CONTAINER);
        _navBar->setAligment(LV_ALIGN_BOTTOM_MID, 0, 0);
        _navBar->setFlexFlow(LV_FLEX_FLOW_ROW);
        _navBar->setPaddingAll(2);

        const char *tabLabels[] = {lvTr("Nav.Home"), lvTr("Nav.Rooms"), lvTr("Nav.Devices"), lvTr("Nav.Scenes"), lvTr("Nav.Settings")};
        for (int i = 0; i < 5; i++)
        {
            auto btn = std::make_unique<w::PushButton>(0, 54, i == 0 ? w::PushButton::Blue : w::PushButton::Dark, tabLabels[i], _navBar.get());
            btn->setFlexGrow(1);

            int idx = i;
            btn->setOnClickedListener([this, idx]() { showPage(idx); });
            _navBtns.push_back(std::move(btn));
        }

        showPage(initialIndex);
    }

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

#if LVGLPP_DYNAMIC_UI
    // Debug controls: 1/2/3/4 = four orientations, R = toggle RTL.
    lvglpp::sys::InputManager::getInstance().addGlobalKeyHandler([](w::KeyEvent &event) {
        if (event.action != w::KeyAction::Press || event.key != w::Key::Character)
        {
            return;
        }
        switch (event.character())
        {
        case U'1':
            lvglpp::sys::UiConfig::instance().requestOrientation(lvglpp::sys::Orientation::Landscape);
            event.consume();
            break;
        case U'2':
            lvglpp::sys::UiConfig::instance().requestOrientation(lvglpp::sys::Orientation::Portrait);
            event.consume();
            break;
        case U'3':
            lvglpp::sys::UiConfig::instance().requestOrientation(lvglpp::sys::Orientation::ReverseLandscape);
            event.consume();
            break;
        case U'4':
            lvglpp::sys::UiConfig::instance().requestOrientation(lvglpp::sys::Orientation::ReversePortrait);
            event.consume();
            break;
        case U'r':
        case U'R':
            lvglpp::sys::UiConfig::instance().requestLayoutDirection(
                lvglpp::sys::UiConfig::instance().isRtl() ? lvglpp::sys::LayoutDirection::Ltr
                                                         : lvglpp::sys::LayoutDirection::Rtl);
            event.consume();
            break;
        default:
            break;
        }
    });
#endif

    auto shell = std::make_shared<SmartHomeShell>();
    lvglpp::sys::StackView::getInstance().pushView(shell);

    return app->exec();
}
