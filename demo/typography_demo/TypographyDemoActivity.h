#ifndef TYPOGRAPHY_DEMO_ACTIVITY_H
#define TYPOGRAPHY_DEMO_ACTIVITY_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/WidgetStore.h"

namespace typography_demo {

/**
 * Demonstrates semantic typography roles (Title / Title2 / Body1 / Body2)
 * whose bitmap fonts are generated per language from strings.xlsx + fonts.json.
 */
class TypographyDemoActivity : public lvglpp::sys::BaseActivity
{
public:
    TypographyDemoActivity()           = default;
    ~TypographyDemoActivity() override = default;

    void onCreate(std::any arg) override;

    void onConfigurationChanged(const lvglpp::sys::UiConfiguration &configuration) override;

    void onDestroy() override;

private:
    void buildUi();

    lvglpp::widgets::WidgetStore store_;
};

}    // namespace typography_demo

#endif    // TYPOGRAPHY_DEMO_ACTIVITY_H
