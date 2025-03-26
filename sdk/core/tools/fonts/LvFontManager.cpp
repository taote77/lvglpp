
#include "LvFontManager.h"

LV_FONT_DECLARE(Noto_20_Normal)
LV_FONT_DECLARE(Noto_20_Bold)
LV_FONT_DECLARE(Noto_24_Normal)
LV_FONT_DECLARE(Noto_24_Bold)
LV_FONT_DECLARE(Noto_28_Normal)
LV_FONT_DECLARE(Noto_28_Bold)
LV_FONT_DECLARE(Noto_32_Normal)
LV_FONT_DECLARE(Noto_32_Bold)
LV_FONT_DECLARE(Noto_36_Normal)
LV_FONT_DECLARE(Noto_36_Bold)
LV_FONT_DECLARE(Noto_64_Bold)
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

namespace lvglpp::tools {
const lv_font_t *LvFontManager::getFontInfo(LvFontManager::FontName name, int size, FontStyle style)
{
    if (name == FontName::DMSans) {
        switch (size) {
        case 20:
            return style == FontStyle::Normal ? &Noto_20_Normal : &Noto_20_Bold;
        case 24:
            return style == FontStyle::Normal ? &Noto_24_Normal : &Noto_24_Bold;
        case 28:
            return style == FontStyle::Normal ? &Noto_28_Normal : &Noto_28_Bold;
        case 32:
            return style == FontStyle::Normal ? &Noto_32_Normal : &Noto_32_Bold;
        case 36:
            return style == FontStyle::Normal ? &Noto_36_Normal : &Noto_36_Bold;
        case 64:
            return &Noto_64_Bold;
        default:
            return style == FontStyle::Normal ? &Noto_20_Normal : &Noto_20_Bold;
        }
    } else if (name == FontName::SansSC) {
        switch (size) {
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
            return &Noto_64_Bold;
        default:
            return style == FontStyle::Normal ? &NotoSC_20_Normal : &NotoSC_20_Bold;
        }
    }
    return &Noto_20_Normal;
}
} // namespace lvglpp::tools
  // tools