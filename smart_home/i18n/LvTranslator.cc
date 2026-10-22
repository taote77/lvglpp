#include "LvTranslator.h"
#include "core/sys/Application.h"
#include "core/sys/UiConfig.h"
#include "translations/lv_i18n.h"
#include <lvgl.h>

static lv_subject_t g_subject_language;

namespace lvglpp {

namespace {

const char *localeForLanguage(LANGUAGE lang)
{
    switch (lang)
    {
    case LANGUAGE::LOCAL:
    case LANGUAGE::CHINESE:
        return "zh-CN";
    case LANGUAGE::ENGLISH:
        return "en-US";
    case LANGUAGE::JAPANESE:
        return "ja-JP";
    case LANGUAGE::RUSSIAN:
        return "ru-RU";
    case LANGUAGE::AR:
        return "ar";
    default:
        return "zh-CN";
    }
}

} // namespace

static void language_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);

    static uint32_t lang_current = 0;
    uint32_t        lang_new     = lv_subject_get_int(subject);
    if (lang_new != lang_current)
    {
        lang_current = lang_new;
        lv_i18n_set_locale(localeForLanguage(LANGUAGE(lang_current)));
    }
}

void LvTranslator::init()
{
    lv_i18n_init(lv_i18n_language_pack);

    lv_subject_init_int(&g_subject_language, 1);
    lv_subject_add_observer_obj(&g_subject_language, language_observer_cb, lv_screen_active(), nullptr);
}

LvTranslator &LvTranslator::Instance()
{
    static LvTranslator lv_translator;
    return lv_translator;
}

LvTranslator::LvTranslator()
{}

void LvTranslator::load(const LANGUAGE lang)
{
    lv_i18n_set_locale(localeForLanguage(lang));

    switch (lang)
    {
    case LANGUAGE::CHINESE:
    case LANGUAGE::LOCAL:
        sys::Application::setLanguageType(sys::Application::Chinese);
        break;
    case LANGUAGE::ENGLISH:
        sys::Application::setLanguageType(sys::Application::English);
        break;
    case LANGUAGE::JAPANESE:
        sys::Application::setLanguageType(sys::Application::Japanese);
        break;
    case LANGUAGE::RUSSIAN:
        sys::Application::setLanguageType(sys::Application::Russian);
        break;
    case LANGUAGE::AR:
        sys::Application::setLanguageType(sys::Application::Arabic);
        break;
    }

#if LVGLPP_ENABLE_RTL
    sys::UiConfig::instance().requestLayoutDirection(lang == LANGUAGE::AR ? sys::LayoutDirection::Rtl
                                                                          : sys::LayoutDirection::Ltr);
#endif
#if LVGLPP_DYNAMIC_UI
    // Re-translate existing pages and re-run their builders.
    sys::UiConfig::instance().requestRebuild();
#endif
}

const char *LvTranslator::tr(const char *text)
{
    return _(text);
}

const char *LvTranslator::getI18nText(const char *text)
{
    return lv_i18n_get_text(text);
}

const char *lvTr(const char *text)
{
    return _(text);
}

const std::string lvTrStr(const char *text)
{
    return _(text);
}

} // namespace lvglpp