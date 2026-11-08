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

#ifdef __cplusplus
}
#endif

#endif // LVGLPP_FONTPROFILE_H
