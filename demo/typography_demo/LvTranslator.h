#ifndef TYPOGRAPHY_DEMO_LV_TRANSLATOR_H
#define TYPOGRAPHY_DEMO_LV_TRANSLATOR_H

namespace typography_demo {

enum class Language {
    English = 0,
    Chinese,
    Japanese,
    Russian,
    Arabic,
};

/// Initialise lv_i18n and select the default locale.
void initTranslations();

/// Switch the locale and rebuild the current page so role fonts resolve for
/// the new language.
void setLanguage(Language language);

Language currentLanguage();

const char *localeTag(Language language);
const char *localeTag();

const char *tr(const char *key);

}    // namespace typography_demo

#endif    // TYPOGRAPHY_DEMO_LV_TRANSLATOR_H
