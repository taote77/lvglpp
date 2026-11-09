#include "LvTranslator.h"

#include "core/sys/Application.h"
#include "core/sys/UiConfig.h"
#include "core/widgets/Label.h"
#include "role_fonts.h"
#include "translations/lv_i18n.h"

#include <cstdlib>
#include <cstring>

namespace typography_demo {

using namespace lvglpp;

namespace {

Language g_language = Language::English;

bool languageFromTag(const char *tag, Language &language)
{
    static const struct {
        const char *tag;
        Language    language;
    } kTags[] = {
        {"en-US", Language::English},
        {"en", Language::English},
        {"zh-CN", Language::Chinese},
        {"zh", Language::Chinese},
        {"ja-JP", Language::Japanese},
        {"ja", Language::Japanese},
        {"ru-RU", Language::Russian},
        {"ru", Language::Russian},
        {"ar", Language::Arabic},
    };

    if (tag == nullptr)
    {
        return false;
    }
    for (const auto &entry : kTags)
    {
        if (std::strcmp(tag, entry.tag) == 0)
        {
            language = entry.language;
            return true;
        }
    }
    return false;
}

void applyLanguage(Language language, bool rebuild)
{
    g_language = language;
    lv_i18n_set_locale(localeTag(language));

    switch (language)
    {
    case Language::English:
        sys::Application::setLanguageType(sys::Application::English);
        break;
    case Language::Chinese:
        sys::Application::setLanguageType(sys::Application::Chinese);
        break;
    case Language::Japanese:
        sys::Application::setLanguageType(sys::Application::Japanese);
        break;
    case Language::Russian:
        sys::Application::setLanguageType(sys::Application::Russian);
        break;
    case Language::Arabic:
        sys::Application::setLanguageType(sys::Application::Arabic);
        break;
    }

    sys::UiConfig::instance().requestLayoutDirection(language == Language::Arabic ? sys::LayoutDirection::Rtl
                                                                                  : sys::LayoutDirection::Ltr);
    if (rebuild)
    {
        // Rebuild the current page; labels resolve their role font from the
        // new locale during the rebuild.
        sys::UiConfig::instance().requestRebuild();
    }
}

}    // namespace

const char *localeTag(Language language)
{
    switch (language)
    {
    case Language::English:
        return "en-US";
    case Language::Chinese:
        return "zh-CN";
    case Language::Japanese:
        return "ja-JP";
    case Language::Russian:
        return "ru-RU";
    case Language::Arabic:
        return "ar";
    }
    return "en-US";
}

const char *localeTag()
{
    return localeTag(g_language);
}

void initTranslations()
{
    if (const char *tag = std::getenv("TYPOGRAPHY_DEMO_LANG"))
    {
        Language language = g_language;
        if (languageFromTag(tag, language))
        {
            g_language = language;
        }
    }

    lv_i18n_init(lv_i18n_language_pack);

    widgets::Label::setRoleFontResolver([](LvglppFontRole role) {
        return typography_demo_role_font(localeTag(), role);
    });

    applyLanguage(g_language, false);
}

void setLanguage(Language language)
{
    if (language == g_language)
    {
        return;
    }
    applyLanguage(language, true);
}

Language currentLanguage()
{
    return g_language;
}

const char *tr(const char *key)
{
    return lv_i18n_get_text(key);
}

}    // namespace typography_demo
