#include "LvTranslator.h"
#include "translations/lv_i18n.h"
#include <iostream>
#include <lvgl.h>

static lv_subject_t g_subject_language;

namespace lvglpp {

static void language_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);

    static uint32_t lang_current = 0;
    uint32_t        lang_new     = lv_subject_get_int(subject);
    if (lang_new != lang_current)
    {
        lang_current = lang_new;

        switch (LANGUAGE(lang_current))
        {
        case LANGUAGE::LOCAL:
        case LANGUAGE::CHINESE:
            lv_i18n_set_locale("zh-CN");
            break;
        case LANGUAGE::ENGLISH:
            lv_i18n_set_locale("en-US");
            break;
        case LANGUAGE::JAPANESE:
            lv_i18n_set_locale("ja");
            break;
        case LANGUAGE::RUSSIAN:
            lv_i18n_set_locale("ru-RU");
            break;
        case LANGUAGE::AR:
            lv_i18n_set_locale("ar");
            break;
        default:
            lv_i18n_set_locale("zh-CN");
            break;
        }
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
    std::cout << __FUNCTION__ << lang;

    switch (lang)
    {
    case LANGUAGE::LOCAL:
    case LANGUAGE::CHINESE:
        lv_i18n_set_locale("zh-CN");
        break;
    case LANGUAGE::ENGLISH:
        lv_i18n_set_locale("en-US");
        break;
    case LANGUAGE::JAPANESE:
        lv_i18n_set_locale("ja");
        break;
    case LANGUAGE::RUSSIAN:
        lv_i18n_set_locale("ru-RU");
        break;
    case LANGUAGE::AR:
        lv_i18n_set_locale("ar");
        break;
    default:
        lv_i18n_set_locale("zh-CN");
        break;
    }
}

const char *LvTranslator::tr(const char *text)
{
    return _(text);
}

} // namespace lvglpp