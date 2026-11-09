#include "role_fonts.h"

#include "font_table_en_us.h"
#include "font_table_ar.h"
#include "font_table_ja_jp.h"
#include "font_table_ru_ru.h"
#include "font_table_zh_cn.h"

#include <string.h>

const LvglppLanguageRoleFonts typography_demo_language_role_fonts[] = {
    {"en-US", typography_demo_role_fonts_en_us, typography_demo_role_fonts_en_us_count},
    {"ar", typography_demo_role_fonts_ar, typography_demo_role_fonts_ar_count},
    {"ja-JP", typography_demo_role_fonts_ja_jp, typography_demo_role_fonts_ja_jp_count},
    {"ru-RU", typography_demo_role_fonts_ru_ru, typography_demo_role_fonts_ru_ru_count},
    {"zh-CN", typography_demo_role_fonts_zh_cn, typography_demo_role_fonts_zh_cn_count},
};
const unsigned typography_demo_language_role_fonts_count =
    sizeof(typography_demo_language_role_fonts) / sizeof(typography_demo_language_role_fonts[0]);

static const LvglppLanguageRoleFonts *find_table(const char *locale)
{
    if (locale != NULL)
    {
        for (unsigned i = 0; i < typography_demo_language_role_fonts_count; ++i)
        {
            if (strcmp(typography_demo_language_role_fonts[i].locale, locale) == 0)
            {
                return &typography_demo_language_role_fonts[i];
            }
        }
    }
    return NULL;
}

static const lv_font_t *find_role(const LvglppRoleFont *fonts, unsigned count,
                                  LvglppFontRole role)
{
    if (fonts == NULL)
    {
        return NULL;
    }
    for (unsigned i = 0; i < count; ++i)
    {
        if (fonts[i].role == role)
        {
            return fonts[i].font;
        }
    }
    return NULL;
}

const lv_font_t *typography_demo_role_font(const char *locale, LvglppFontRole role)
{
    const LvglppLanguageRoleFonts *table = find_table(locale);
    if (table == NULL)
    {
        table = find_table("en-US");
    }
    if (table == NULL)
    {
        return NULL;
    }

    const lv_font_t *font = find_role(table->fonts, table->count, role);
    if (font == NULL)
    {
        font = find_role(table->fonts, table->count, LVGLPP_FONT_ROLE_BODY1);
    }
    if (font == NULL && table->count > 0)
    {
        font = table->fonts[0].font;
    }
    return font;
}
