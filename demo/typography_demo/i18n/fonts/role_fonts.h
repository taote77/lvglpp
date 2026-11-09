#ifndef TYPOGRAPHY_DEMO_ROLE_FONTS_H
#define TYPOGRAPHY_DEMO_ROLE_FONTS_H

#include "core/tools/fonts/FontProfile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LvglppLanguageRoleFonts {
    const char           *locale;
    const LvglppRoleFont *fonts;
    unsigned              count;
} LvglppLanguageRoleFonts;

extern const LvglppLanguageRoleFonts typography_demo_language_role_fonts[];
extern const unsigned typography_demo_language_role_fonts_count;

/** Resolve a role font for a locale. Falls back to en-US and then to
 *  the default role. Returns NULL only when no fonts were generated. */
const lv_font_t *typography_demo_role_font(const char *locale, LvglppFontRole role);

#ifdef __cplusplus
}
#endif

#endif // TYPOGRAPHY_DEMO_ROLE_FONTS_H
