/**************************************************************************

Copyright:Copyright  2023 HeyGears. All rights reserved

Author: Lhyang hli3@heygears.com

Date:2023-05-07

Class:CheckBox

Description:

**************************************************************************/

#ifndef ULTRACORE_REFLEX_CHECKBOX_H
#define ULTRACORE_REFLEX_CHECKBOX_H

#include "BaseItem.h"
#include <memory>
#include "MouseArea.h"

namespace heygears
{
    namespace widgets
    {

        class CheckBox : public BaseItem
        {
        public:
            explicit CheckBox(int width, int height, uint32_t bg_clr, uint32_t check_clr, BaseItem *parent = nullptr);

            bool getCheckState();

        private:
            std::shared_ptr<BaseItem> check_item_;

            std::shared_ptr<MouseArea> check_mouse_area_;

            void init();

            bool check_state_ = false;

        };

    } // heygears
} // widgets

#endif //ULTRACORE_REFLEX_CHECKBOX_H
