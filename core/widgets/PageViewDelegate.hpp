/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-25

Class:${CLASS}

Description:PageView控件代理基类

**************************************************************************/

#ifndef LVGL_XFJ_DEMO_PAGEVIEWDELEGATE_H
#define LVGL_XFJ_DEMO_PAGEVIEWDELEGATE_H

#include "Delegate.hpp"

namespace heygears
{
    namespace widgets
    {
        template<typename T>
        class PageViewDelegate : public Delegate<T>
        {
        public:
            explicit PageViewDelegate(BaseItem *parent = nullptr) : Delegate<T>(parent)
            {}

        private:
        };
    }
}
#endif //LVGL_XFJ_DEMO_PAGEVIEWDELEGATE_H
