#ifndef LVGLPP_FONTPROFILE_H
#define LVGLPP_FONTPROFILE_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * One size/style entry of a generated font profile.
 *
 * gen_fonts_profile.py --emit-table emits arrays of this type; pass them to
 * LvFontManager::setProfile() at startup or when the language changes.
 */
typedef struct LvglppFontFace {
    int              size;
    const lv_font_t *normal;
    const lv_font_t *bold;
} LvglppFontFace;

/**
 * Semantic typography roles. Keep in sync with Label::Role and with the role
 * names accepted by gen_role_fonts.py ("Title", "Title2", "Body1", "Body2").
 */
typedef enum {
    LVGLPP_FONT_ROLE_NONE  = -1,
    LVGLPP_FONT_ROLE_TITLE = 0,
    LVGLPP_FONT_ROLE_TITLE2,
    LVGLPP_FONT_ROLE_BODY1,
    LVGLPP_FONT_ROLE_BODY2,
    LVGLPP_FONT_ROLE_COUNT
} LvglppFontRole;

/**
 * One role and the font generated for it in a specific language.
 *
 * gen_role_fonts.py emits arrays of this type plus a per-language registry
 * (<prefix>_role_font(locale, role)).
 */
typedef struct LvglppRoleFont {
    LvglppFontRole   role;
    int              size;
    const lv_font_t *font;
} LvglppRoleFont;

#ifdef __cplusplus
}
#endif

#endif // LVGLPP_FONTPROFILE_H
