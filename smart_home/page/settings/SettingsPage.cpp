#include "SettingsPage.h"
#include "core/log/log.h"
#include "core/widgets/Toast.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {

using namespace form::design;

SettingsPage::SettingsPage() = default;
SettingsPage::~SettingsPage() = default;

void SettingsPage::onCreate(std::any arg) {
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    _title = std::make_unique<w::LvText>("Settings", CLR_ON_SURFACE, getRoot(), w::LvText::FontSize28Bold);
    _title->setAligment(LV_ALIGN_TOP_LEFT, 20, 10);

    buildAppearanceSection();
    buildLanguageSection();
    buildNetworkSection();
    buildAboutSection();

    LOG_INFO() << "SettingsPage created";
}

void SettingsPage::buildAppearanceSection() {
    auto sectionLabel = std::make_unique<w::LvText>("Appearance", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::LvText::FontSize22Bold);
    sectionLabel->setAligment(LV_ALIGN_TOP_LEFT, 20, 50);

    // Theme dropdown
    auto themeLabel = std::make_unique<w::LvText>("Theme:", CLR_ON_SURFACE_VARIANT_0, getRoot(), w::LvText::FontSize20);
    themeLabel->setAligment(LV_ALIGN_TOP_LEFT, 40, 85);

    _themeDropdown = std::make_unique<w::Dropdown>(getRoot());
    _themeDropdown->setSize(180, 36);
    _themeDropdown->setOptions("Dark\nLight\nAuto (System)");
    _themeDropdown->setSelected(0);
    _themeDropdown->setAligment(LV_ALIGN_TOP_LEFT, 200, 85);

    // Brightness slider
    auto brightLabel = std::make_unique<w::LvText>("Brightness:", CLR_ON_SURFACE_VARIANT_0, getRoot(), w::LvText::FontSize20);
    brightLabel->setAligment(LV_ALIGN_TOP_LEFT, 40, 130);

    _brightnessSlider = std::make_unique<w::Slider>(getRoot());
    _brightnessSlider->setSize(200, 20);
    _brightnessSlider->setRange(10, 100);
    _brightnessSlider->setValue(80);
    _brightnessSlider->setAligment(LV_ALIGN_TOP_LEFT, 200, 135);
}

void SettingsPage::buildLanguageSection() {
    auto sectionLabel = std::make_unique<w::LvText>("Language", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::LvText::FontSize22Bold);
    sectionLabel->setAligment(LV_ALIGN_TOP_LEFT, 20, 175);

    _langRoller = std::make_unique<w::Roller>(getRoot());
    _langRoller->setSize(160, 100);
    _langRoller->setOptions("English\nChinese\nJapanese\nRussian\nArabic", LV_ROLLER_MODE_INFINITE);
    _langRoller->setSelected(0);
    _langRoller->setVisibleRowCount(3);
    _langRoller->setAligment(LV_ALIGN_TOP_LEFT, 200, 180);
}

void SettingsPage::buildNetworkSection() {
    auto sectionLabel = std::make_unique<w::LvText>("Network", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::LvText::FontSize22Bold);
    sectionLabel->setAligment(LV_ALIGN_TOP_LEFT, 20, 295);

    auto wifiLabel = std::make_unique<w::LvText>("WiFi:", CLR_ON_SURFACE_VARIANT_0, getRoot(), w::LvText::FontSize20);
    wifiLabel->setAligment(LV_ALIGN_TOP_LEFT, 40, 325);

    _wifiSwitch = std::make_unique<w::SwitchButton>(48, 24, getRoot());
    _wifiSwitch->setCheck(true);
    _wifiSwitch->setAligment(LV_ALIGN_TOP_LEFT, 200, 325);

    auto wifiInfo = std::make_unique<w::LvText>("SSID: MyHomeWiFi  |  IP: 192.168.1.100", CLR_ON_SURFACE_VARIANT_2, getRoot(), w::LvText::FontSize20);
    wifiInfo->setAligment(LV_ALIGN_TOP_LEFT, 260, 327);
}

void SettingsPage::buildAboutSection() {
    auto sectionLabel = std::make_unique<w::LvText>("About", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::LvText::FontSize22Bold);
    sectionLabel->setAligment(LV_ALIGN_TOP_LEFT, 20, 370);

    auto versionLabel = std::make_unique<w::LvText>("LVGL++ Smart Home v2.0", CLR_ON_SURFACE_VARIANT_2, getRoot(), w::LvText::FontSize20);
    versionLabel->setAligment(LV_ALIGN_TOP_LEFT, 40, 400);

    auto copyright = std::make_unique<w::LvText>("(c) 2024-2026 LVGL++ Team", CLR_PRIMARY_DIS_BG, getRoot(), w::LvText::FontSize20);
    copyright->setAligment(LV_ALIGN_TOP_LEFT, 40, 425);
}

void SettingsPage::onNotifyUI(const sys::Event &) {}
void SettingsPage::onDestroy() {}

} // namespace lvglpp::ui::page
