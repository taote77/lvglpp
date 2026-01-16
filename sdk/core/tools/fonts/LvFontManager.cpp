
#include "LvFontManager.h"
#include <iostream>

LV_FONT_DECLARE(NotoSC_20_Normal)
LV_FONT_DECLARE(NotoSC_20_Bold)
LV_FONT_DECLARE(NotoSC_24_Normal)
LV_FONT_DECLARE(NotoSC_24_Bold)
LV_FONT_DECLARE(NotoSC_28_Normal)
LV_FONT_DECLARE(NotoSC_28_Bold)
LV_FONT_DECLARE(NotoSC_32_Normal)
LV_FONT_DECLARE(NotoSC_32_Bold)
LV_FONT_DECLARE(NotoSC_36_Normal)
LV_FONT_DECLARE(NotoSC_36_Bold)

#define PATH_PREFIX "./asset/fonts/"

namespace lvglpp::tools {

bool LvFontManager::kEnableFreetype = false;

void LvFontManager::enableFreetype(bool enable)
{
    LvFontManager::kEnableFreetype = enable;
}

const lv_font_t *LvFontManager::getFontInfo(LvFontManager::FontName name, int size, FontStyle style)
{
    static bool init{false};

    static lv_font_t *font22{nullptr};
    static lv_font_t *font24{nullptr};

    if (LvFontManager::kEnableFreetype)
    {
        if (!init)
        {
            // font22 = lv_freetype_font_create(PATH_PREFIX "NotoSansSC-AR-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 22, LV_FREETYPE_FONT_STYLE_NORMAL);
            // font24 = lv_freetype_font_create(PATH_PREFIX "NotoSansSC-AR-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 24, LV_FREETYPE_FONT_STYLE_NORMAL);

            // font22 = lv_freetype_font_create(PATH_PREFIX "NotoSansArabic-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 22, LV_FREETYPE_FONT_STYLE_NORMAL);
            // font24 = lv_freetype_font_create(PATH_PREFIX "NotoSansArabic-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 24, LV_FREETYPE_FONT_STYLE_NORMAL);

            font22 = lv_freetype_font_create(PATH_PREFIX "NotoSansSC-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 22, LV_FREETYPE_FONT_STYLE_NORMAL);
            font24 = lv_freetype_font_create(PATH_PREFIX "NotoSansSC-Regular.ttf", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 24, LV_FREETYPE_FONT_STYLE_NORMAL);

            if (!font22 || !font24)
            {
                LV_LOG_ERROR("fail");

                std::cout << "fail" << std::endl;
            } else
            {
                LV_LOG_INFO("success");
                std::cout << "success " << std::endl;
            }
            init = true;
        }

        switch (size)
        {
        case 22:
            return font22;
        case 24:
            return font24;
        default:
            return font24;
        }
    }

    if (name == FontName::SansSC)
    {
        switch (size)
        {
        case 20:
            return style == FontStyle::Normal ? &NotoSC_20_Normal : &NotoSC_20_Bold;
        case 24:
            return style == FontStyle::Normal ? &NotoSC_24_Normal : &NotoSC_24_Bold;
        case 28:
            return style == FontStyle::Normal ? &NotoSC_28_Normal : &NotoSC_28_Bold;
        case 32:
            return style == FontStyle::Normal ? &NotoSC_32_Normal : &NotoSC_32_Bold;
        case 36:
            return style == FontStyle::Normal ? &NotoSC_36_Normal : &NotoSC_36_Bold;
        case 64:
            return &NotoSC_36_Normal;
        default:
            return style == FontStyle::Normal ? &NotoSC_20_Normal : &NotoSC_20_Bold;
        }
    }
    return &NotoSC_20_Bold;
}
} // namespace lvglpp::tools
  // tools