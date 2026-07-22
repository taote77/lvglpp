#ifndef SMART_HOME_SCENES_PAGE_H
#define SMART_HOME_SCENES_PAGE_H

#include "components/SceneButton.h"
#include "core/sys/BaseActivity.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include <memory>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::page {

/// Scene automation page with preset scene buttons
class ScenesPage : public sys::BaseActivity
{
public:
    ScenesPage();
    ~ScenesPage() override;

protected:
    void onCreate(std::any arg) override;
    void onNotifyUI(const sys::Event &evt) override;
    void onDestroy() override;

private:
    std::unique_ptr<w::LvText> _title;
    std::unique_ptr<w::LvText> _activeSceneLabel;
};

} // namespace lvglpp::ui::page

#endif
