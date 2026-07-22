#ifndef SMART_HOME_SETTINGS_PAGE_H
#define SMART_HOME_SETTINGS_PAGE_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/Dropdown.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/Roller.h"
#include "core/widgets/Slider.h"
#include "core/widgets/SwitchButton.h"
#include <memory>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::page {

/// Settings page with working theme, language, brightness, and about info
class SettingsPage : public sys::BaseActivity
{
public:
    SettingsPage();
    ~SettingsPage() override;

protected:
    void onCreate(std::any arg) override;
    void onNotifyUI(const sys::Event &evt) override;
    void onDestroy() override;

private:
    void buildAppearanceSection();
    void buildLanguageSection();
    void buildNetworkSection();
    void buildAboutSection();

    std::unique_ptr<w::LvText>      _title;
    std::unique_ptr<w::Dropdown>    _themeDropdown;
    std::unique_ptr<w::Roller>      _langRoller;
    std::unique_ptr<w::Slider>      _brightnessSlider;
    std::unique_ptr<w::SwitchButton> _wifiSwitch;
};

} // namespace lvglpp::ui::page

#endif
