/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-06-06

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_SPINBOX_H
#define ULTRACORE_FOR_MAKERS_SPINBOX_H

#include "BaseItem.h"

namespace heygears
{
    namespace widgets
    {

        class SpinBox : public BaseItem
        {
        public:
            explicit SpinBox(BaseItem *parent = nullptr);

            void setRange(int min, int max);

            void setDigitFormat(int total, int intNum);

            void setVal(int val);

            int getVal() const;

        private:
            lv_style_t font_style_{};
        };

    }
} // widgets

#endif //ULTRACORE_FOR_MAKERS_SPINBOX_H
