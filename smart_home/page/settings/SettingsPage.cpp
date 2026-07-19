#include "SettingsPage.h"
#include "core/log/log.h"
#include "core/sys/StackView.h"
#include "core/widgets/BaseItem.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {

using namespace lvglpp;
using namespace lvglpp::widgets;
using namespace form::design;

SettingsPage::SettingsPage() : sys::BaseActivity() {}
SettingsPage::~SettingsPage() = default;

void SettingsPage::onCreate(std::any arg)
{
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    // Title
    _title = std::make_unique<LvText>("Settings", CLR_ON_SURFACE, getRoot(), LvText::FontSize28Bold);
    _title->setAligment(LV_ALIGN_TOP_MID, 0, 20);

    // Back button
    _backBtn = std::make_unique<PushButton>(120, 40, PushButton::Gray, "Back", getRoot());
    _backBtn->setAligment(LV_ALIGN_TOP_LEFT, 20, 20);
    _backBtn->setOnClickedListener([]() {
        sys::StackView::getInstance().popView();
    });

    buildThemeSection();
    buildLanguageSection();
    buildAboutSection();

    LOG_INFO() << "SettingsPage created";
}

void SettingsPage::buildThemeSection()
{
    _themeLabel = std::make_unique<LvText>("Theme", CLR_PRIMARY_BLACK, getRoot(), LvText::FontSize24Bold);
    _themeLabel->setAligment(LV_ALIGN_TOP_LEFT, 40, 80);

    for (int i = 0; i < 3; i++) {
        const char *names[] = {"Light", "Dark", "Auto"};
        auto sw = std::make_unique<SwitchButton>(200, 40, getRoot());
        sw->setAligment(LV_ALIGN_TOP_LEFT, 200, (lv_coord_t)(80 + i * 50));
        sw->setEnable(i == _currentTheme); // highlight current
        _switches.push_back(std::move(sw));

        auto label = std::make_unique<LvText>(names[i], CLR_PRIMARY_BLACK, getRoot(), LvText::FontSize20);
        label->setAligment(LV_ALIGN_TOP_LEFT, 210, (lv_coord_t)(80 + i * 50));
    }
}

void SettingsPage::buildLanguageSection()
{
    _langLabel = std::make_unique<LvText>("Language", CLR_PRIMARY_BLACK, getRoot(), LvText::FontSize24Bold);
    _langLabel->setAligment(LV_ALIGN_TOP_LEFT, 40, 250);

    const char *langs[] = {"English", "Chinese", "Japanese", "Russian"};
    for (int i = 0; i < 4; i++) {
        auto label = std::make_unique<LvText>(langs[i], CLR_PRIMARY_BLACK, getRoot(), LvText::FontSize20);
        label->setAligment(LV_ALIGN_TOP_LEFT, 200, (lv_coord_t)(250 + i * 40));
        if (i == _currentLang) {
            label->setTextClr(CLR_PRIMARY_BLUE);
        }
    }
}

void SettingsPage::buildAboutSection()
{
    _versionLabel = std::make_unique<LvText>("LVGL++ Framework v2.0.0", CLR_PRIMARY_DIS_BG, getRoot(), LvText::FontSize20);
    _versionLabel->setAligment(LV_ALIGN_BOTTOM_MID, 0, -40);

    auto copyright = std::make_unique<LvText>("(c) 2024-2026 LVGL++ Team", CLR_PRIMARY_DIS_BG, getRoot(), LvText::FontSize20);
    copyright->setAligment(LV_ALIGN_BOTTOM_MID, 0, -10);
}

void SettingsPage::onThemeChanged(int themeIndex)
{
    _currentTheme = themeIndex;
    LOG_INFO() << "Theme changed to " << themeIndex;
}

void SettingsPage::onLanguageChanged(int langIndex)
{
    _currentLang = langIndex;
    LOG_INFO() << "Language changed to " << langIndex;
}

void SettingsPage::onNotifyUI(const sys::Event &evt) {}
void SettingsPage::onDestroy() {}

} // namespace lvglpp::ui::page
