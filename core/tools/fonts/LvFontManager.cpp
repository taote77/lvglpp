/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-08-22

Class:${CLASS}

Description:

**************************************************************************/

#include "LvFontManager.h"

LV_FONT_DECLARE(DMSans16_Normal)
LV_FONT_DECLARE(DMSans16_Bold)
LV_FONT_DECLARE(DMSans18_Normal)
LV_FONT_DECLARE(DMSans18_Bold)
LV_FONT_DECLARE(DMSans20_Normal)
LV_FONT_DECLARE(DMSans20_Bold)
LV_FONT_DECLARE(DMSans24_Normal)
LV_FONT_DECLARE(DMSans24_Bold)
LV_FONT_DECLARE(DMSans32_Normal)
LV_FONT_DECLARE(DMSans32_Bold)
LV_FONT_DECLARE(DMSans36_Normal)
LV_FONT_DECLARE(DMSans36_Bold)
LV_FONT_DECLARE(DMSans48_Normal)
LV_FONT_DECLARE(DMSans48_Bold)
LV_FONT_DECLARE(DMSans64_Bold)
LV_FONT_DECLARE(DMSans72_Normal)
LV_FONT_DECLARE(DMSans72_Bold)
LV_FONT_DECLARE(DMSans120_Bold)
LV_FONT_DECLARE(SYHT_24Normal)
LV_FONT_DECLARE(SYHT_36Normal)
LV_FONT_DECLARE(SYHT_64Bold)
LV_FONT_DECLARE(SYHT_20Normal)
LV_FONT_DECLARE(SYHT_16Normal)
LV_FONT_DECLARE(SYHT_32Bold)
namespace heygears {
namespace tools {
const lv_font_t *LvFontManager::getFontInfo(LvFontManager::FontName name, int size, FontStyle style)
{
    if (name == FontName::DMSans) {
        switch (size) {
        case 16:
            return style == FontStyle::Normal ? &DMSans16_Normal : &DMSans16_Bold;
        case 18:
            return style == FontStyle::Normal ? &DMSans18_Normal : &DMSans18_Bold;
        case 20:
            return style == FontStyle::Normal ? &DMSans20_Normal : &DMSans20_Bold;
        case 24:
            return style == FontStyle::Normal ? &DMSans24_Normal : &DMSans24_Bold;
        case 32:
            return style == FontStyle::Normal ? &DMSans32_Normal : &DMSans32_Bold;
        case 36:
            return style == FontStyle::Normal ? &DMSans36_Normal : &DMSans36_Bold;
        case 48:
            return style == FontStyle::Normal ? &DMSans48_Normal : &DMSans48_Bold;
        case 64:
            return &DMSans64_Bold;
        case 72:
            return style == FontStyle::Normal ? &DMSans72_Normal : &DMSans72_Bold;
        case 120:
            return &DMSans120_Bold;
        default:
            return style == FontStyle::Normal ? &DMSans16_Normal : &DMSans16_Bold;
        }
    } else if (name == FontName::SansSC) {
        switch (size) {
        case 16:
            return &SYHT_16Normal;
        case 18:
            return style == FontStyle::Normal ? &DMSans18_Normal : &DMSans18_Bold;
        case 20:
            // return style == FontStyle::Normal ? &DMSans20_Normal : &DMSans20_Bold;
            return &SYHT_20Normal;
        case 24:
            // return style==FontStyle::Normal?&DMSans24_Normal:&DMSans24_Bold;
            return &SYHT_24Normal;
        case 32:
            // return style == FontStyle::Normal ? &DMSans32_Normal : &SYHT_32Bold;
            return &SYHT_32Bold;
        case 36:
            // return style==FontStyle::Normal?&DMSans36_Normal:&DMSans36_Bold;
            return &SYHT_36Normal;
        case 48:
            return style == FontStyle::Normal ? &DMSans48_Normal : &DMSans48_Bold;
        case 64:
            return &SYHT_64Bold;
        case 72:
            return style == FontStyle::Normal ? &DMSans72_Normal : &DMSans72_Bold;
        case 120:
            return &DMSans120_Bold;
        default:
            return style == FontStyle::Normal ? &DMSans16_Normal : &DMSans16_Bold;
        }
    }
    return &DMSans16_Normal;
}
} // namespace tools
} // namespace heygears