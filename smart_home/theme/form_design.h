#ifndef UI_FORM_DESIGN
#define UI_FORM_DESIGN

/**************************************************************************

Class:定义全局变量。如颜色

Description:

**************************************************************************/

#include "form_design_dark.h"

namespace form::design
{

    using LColorType = uint32_t;

    enum class DesignStyle
    {
        DARK = 0,
        SHALLOW = 1,
    };

    static LColorType ThemeColor(LColorType color, DesignStyle style = DesignStyle::DARK)
    {
        if (style == DesignStyle::DARK)
        {
            return color;
        }

        return color;
    };

}// namespace form::design

#endif// UI_FORM_DESIGN
