/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-08-22

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_LVFONTMANAGER_H
#define ULTRACORE_FOR_MAKERS_LVFONTMANAGER_H
#include "lvgl.h"
#include <map>
#include <string>

namespace heygears::tools
{
    class LvFontManager
    {
    public:
        enum FontName
        {
            SansSC,
            DMSans,
            Oswald
        };
        enum FontStyle
        {
            Normal,
            Bold
        };

        LvFontManager() = delete;

        LvFontManager(const LvFontManager &) = delete;

        LvFontManager operator=(const LvFontManager &) = delete;

        static const lv_font_t *getFontInfo(FontName name, int size, FontStyle style);

    private:
    };

}// namespace heygears::tools
 // tools

#endif//ULTRACORE_FOR_MAKERS_LVFONTMANAGER_H
