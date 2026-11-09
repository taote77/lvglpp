#ifndef SMART_HOME_ROLE_FONTS_H
#define SMART_HOME_ROLE_FONTS_H

#include "core/tools/fonts/FontProfile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LvglppLanguageRoleFonts {
    const char           *locale;
    const LvglppRoleFont *fonts;
    unsigned              count;
} LvglppLanguageRoleFonts;

extern const LvglppLanguageRoleFonts smart_home_language_role_fonts[];
extern const unsigned smart_home_language_role_fonts_count;

/** Resolve a role font for a locale. Falls back to en-US and then to
 *  the default role. Returns NULL only when no fonts were generated. */
const lv_font_t *smart_home_role_font(const char *locale, LvglppFontRole role);

#ifdef __cplusplus
}
#endif

#endif // SMART_HOME_ROLE_FONTS_H
