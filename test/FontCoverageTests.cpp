#include "TestFramework.h"
#include "core/tools/fonts/LvFontManager.h"
#include <lvgl.h>

using namespace lvglpp::test;
using lvglpp::tools::LvFontManager;

namespace {

bool hasGlyph(const lv_font_t *font, uint32_t codepoint)
{
    if (font == nullptr)
    {
        return false;
    }
    lv_font_glyph_dsc_t dsc{};
    return lv_font_get_glyph_dsc(font, &dsc, codepoint, 0);
}

} // namespace

TEST(Fonts_bitmap_sizes_cover_scripts)
{
    // Representative glyphs from every supported language.
    const uint32_t codepoints[] = {
        'A',      // Latin
        0x4E2D,   // 中
        0x3042,   // あ
        0x0416,   // Ж
        0x0627,   // ا
    };

    for (int size : {20, 22, 24, 28, 32, 36})
    {
        const lv_font_t *normal = LvFontManager::getFontInfo(LvFontManager::SansSC, size, LvFontManager::Normal);
        const lv_font_t *bold   = LvFontManager::getFontInfo(LvFontManager::SansSC, size, LvFontManager::Bold);
        CHECK_TRUE(normal != nullptr);
        CHECK_TRUE(bold != nullptr);
        for (uint32_t codepoint : codepoints)
        {
            CHECK_TRUE(hasGlyph(normal, codepoint));
            CHECK_TRUE(hasGlyph(bold, codepoint));
        }
    }
    return true;
}

TEST(Fonts_bold_arabic_fallback)
{
    // LVGL's Arabic shaping looks up presentation forms (U+FE70..FEFF), which
    // only exist in the Arabic fallback attached to the bold fonts.
    const lv_font_t *bold = LvFontManager::getFontInfo(LvFontManager::SansSC, 22, LvFontManager::Bold);
    CHECK_TRUE(bold != nullptr);
    CHECK_TRUE(hasGlyph(bold, 0xFE8E)); // ARABIC LETTER ALEF FINAL FORM
    return true;
}

TEST(Fonts_large_styles_map_to_36_bold)
{
    const lv_font_t *font48 = LvFontManager::getFontInfo(LvFontManager::SansSC, 48, LvFontManager::Bold);
    const lv_font_t *font64 = LvFontManager::getFontInfo(LvFontManager::SansSC, 64, LvFontManager::Bold);
    const lv_font_t *font36 = LvFontManager::getFontInfo(LvFontManager::SansSC, 36, LvFontManager::Bold);
    CHECK_TRUE(font48 != nullptr);
    CHECK_TRUE(font64 != nullptr);
    CHECK_TRUE(font48 == font36);
    CHECK_TRUE(font64 == font36);
    return true;
}
