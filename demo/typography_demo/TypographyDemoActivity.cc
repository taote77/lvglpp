#include "TypographyDemoActivity.h"

#include "LvTranslator.h"
#include "config/color_sheet.h"
#include "core/widgets/Label.h"
#include "core/widgets/PushButton.h"

#include <cstddef>
#include <string>

using namespace lvglpp;
using namespace form::design;

namespace typography_demo {
namespace {

struct LanguageChoice {
    const char *label;
    Language    language;
};

constexpr LanguageChoice kLanguages[] = {
    {"English", Language::English},
    {"中文", Language::Chinese},
    {"日本語", Language::Japanese},
    {"Русский", Language::Russian},
    {"العربية", Language::Arabic},
};

}    // namespace

void TypographyDemoActivity::onCreate(std::any arg)
{
    sys::BaseActivity::onCreate(arg);
    buildUi();
}

void TypographyDemoActivity::onConfigurationChanged(const sys::UiConfiguration &configuration)
{
    // Language changes arrive here through UiConfig::requestRebuild(); drop the
    // old wrappers before LVGL deletes their objects, then rebuild from scratch.
    store_.clear();
    if (auto *root = getRoot(); root != nullptr && root->lvObj() != nullptr)
    {
        lv_obj_clean(root->lvObj());
    }

    sys::BaseActivity::onConfigurationChanged(configuration);
    buildUi();
    refreshFocus();
}

void TypographyDemoActivity::onDestroy()
{
    store_.clear();
    sys::BaseActivity::onDestroy();
}

void TypographyDemoActivity::buildUi()
{
    auto *root = getRoot();
    if (root == nullptr || root->lvObj() == nullptr)
    {
        return;
    }

    root->setBgColor(CLR_SURFACE);
    root->setBgOpacity(LV_OPA_COVER);

    const char *locale = localeTag();

    // The call site only names the semantic role; the installed resolver picks
    // the font for the current language.
    auto addRoleLabel = [&](const char *key, widgets::Label::Role role, uint32_t color, int y) {
        auto *label = store_.emplace<widgets::Label>(tr(key), color, root, role);
        label->setWidth(752);
        label->setLongMode(LV_LABEL_LONG_WRAP);
        label->setAlignment(LV_ALIGN_TOP_LEFT, 24, y);
        return label;
    };

    addRoleLabel("typo_title", widgets::Label::Role::Title, CLR_ON_SURFACE, 18);
    addRoleLabel("typo_subtitle", widgets::Label::Role::Title2, CLR_PRIMARY, 66);
    addRoleLabel("typo_body1", widgets::Label::Role::Body1, CLR_ON_SURFACE, 112);
    addRoleLabel("typo_body2", widgets::Label::Role::Body2, CLR_ON_SURFACE_VARIANT_1, 178);
    addRoleLabel("typo_caption", widgets::Label::Role::Body2, CLR_ON_SURFACE_VARIANT_2, 244);

    auto *languageLabel = store_.emplace<widgets::Label>(tr("typo_language"), CLR_PRIMARY, root, widgets::Label::Role::Body1);
    languageLabel->setAlignment(LV_ALIGN_TOP_LEFT, 24, 292);

    auto *localeLabel = store_.emplace<widgets::Label>(std::string("locale: ") + locale,
                                                       CLR_ON_SURFACE_VARIANT_2,
                                                       root,
                                                       widgets::Label::Role::Body2);
    localeLabel->setAlignment(LV_ALIGN_TOP_LEFT, 120, 296);

    for (std::size_t i = 0; i < sizeof(kLanguages) / sizeof(kLanguages[0]); ++i)
    {
        const LanguageChoice &choice = kLanguages[i];
        auto                 *button = store_.emplace<widgets::PushButton>(
            140, 38, widgets::PushButton::Blue, choice.label, root);
        button->setAlignment(LV_ALIGN_TOP_LEFT, 24 + static_cast<int>(i) * 148, 322);
        button->onClicked([language = choice.language]() { setLanguage(language); });
    }

    auto *hint = store_.emplace<widgets::Label>(tr("typo_switch_hint"), CLR_ON_SURFACE_VARIANT_1, root, widgets::Label::Role::Body2);
    hint->setWidth(752);
    hint->setLongMode(LV_LABEL_LONG_WRAP);
    hint->setAlignment(LV_ALIGN_TOP_LEFT, 24, 386);
}

}    // namespace typography_demo
