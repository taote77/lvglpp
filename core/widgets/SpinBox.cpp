/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-06-06

Class:${CLASS}

Description:

**************************************************************************/

#include "SpinBox.h"

namespace heygears
{
    namespace widgets
    {
        SpinBox::SpinBox(BaseItem *parent) : BaseItem(BaseItem::SpinBox, parent)
        {
            lv_spinbox_step_prev(getLvglItem());
            lv_style_init(&font_style_);
//            lv_style_set_text_font(&font_style_,
//                                   tools::TrueFontManager::getInstance()->getFontInfo(tools::TrueFontManager::DMSans,
//                                                                                      32, FT_FONT_STYLE_BOLD).font);
            lv_style_set_text_align(&font_style_, LV_TEXT_ALIGN_AUTO);
            lv_obj_add_style(getLvglItem(), &font_style_, LV_STATE_DEFAULT);
        }

        void SpinBox::setRange(int min, int max)
        {
            lv_spinbox_set_range(getLvglItem(), min, max);
        }

        void SpinBox::setDigitFormat(int total, int intNum)
        {
            lv_spinbox_set_digit_format(getLvglItem(), total, intNum);
        }

        void SpinBox::setVal(int val)
        {
            lv_spinbox_set_value(getLvglItem(), val);
        }

        int SpinBox::getVal() const
        {
            return lv_spinbox_get_value(getLvglItem());
        }
    }
} // widgets