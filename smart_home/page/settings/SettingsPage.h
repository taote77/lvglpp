#ifndef SMART_HOME_SETTINGS_PAGE_H
#define SMART_HOME_SETTINGS_PAGE_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/SwitchButton.h"
#include <memory>
#include <vector>

namespace lvglpp::ui::page {

/// Settings page with theme switching, language selection, and about info
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
    void buildThemeSection();
    void buildLanguageSection();
    void buildAboutSection();
    void onThemeChanged(int themeIndex);
    void onLanguageChanged(int langIndex);

    std::unique_ptr<widgets::LvText>       _title;
    std::unique_ptr<widgets::LvText>       _themeLabel;
    std::unique_ptr<widgets::LvText>       _langLabel;
    std::unique_ptr<widgets::LvText>       _versionLabel;
    std::unique_ptr<widgets::PushButton>   _backBtn;
    std::vector<std::unique_ptr<widgets::SwitchButton>> _switches;

    int _currentTheme = 0;
    int _currentLang  = 0;
};

} // namespace lvglpp::ui::page

#endif // SMART_HOME_SETTINGS_PAGE_H
