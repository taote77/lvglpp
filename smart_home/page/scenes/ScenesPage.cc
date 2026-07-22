#include "ScenesPage.h"
#include "core/log/log.h"
#include "core/widgets/Toast.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {

using namespace form::design;

ScenesPage::ScenesPage() = default;
ScenesPage::~ScenesPage() = default;

void ScenesPage::onCreate(std::any arg) {
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    _title = std::make_unique<w::LvText>("Scenes", CLR_ON_SURFACE, getRoot(), w::LvText::FontSize28Bold);
    _title->setAligment(LV_ALIGN_TOP_LEFT, 20, 10);

    _activeSceneLabel = std::make_unique<w::LvText>("Active: None", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::LvText::FontSize20);
    _activeSceneLabel->setAligment(LV_ALIGN_TOP_RIGHT, -20, 15);

    struct SceneDef { const char *icon; const char *name; const char *desc; };
    SceneDef scenes[] = {
        {"Sun",   "Good Morning", "Lights on, blinds open"},
        {"Moon",  "Good Night",   "All off, doors locked"},
        {"Car",   "Away Mode",    "All off, cameras on"},
        {"Film",  "Movie Time",   "Lights dim, TV on"},
    };

    int x = 20, y = 50;
    for (int i = 0; i < 4; i++) {
        auto btn = std::make_unique<ui::widgets::SceneButton>(scenes[i].icon, scenes[i].name, scenes[i].desc, getRoot());
        btn->setAligment(LV_ALIGN_TOP_LEFT, x, y);
        btn->setOnActivate([this, name = std::string(scenes[i].name)]() {
            _activeSceneLabel->setText("Active: " + name);
            w::Toast::success((name + " activated").c_str());
        });
        x += 190;
    }

    LOG_INFO() << "ScenesPage created";
}

void ScenesPage::onNotifyUI(const sys::Event &) {}
void ScenesPage::onDestroy() {}

} // namespace lvglpp::ui::page
