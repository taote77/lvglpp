/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-25

Class:${CLASS}

Description:ListView控件代理基类

**************************************************************************/

#ifndef LVGL_XFJ_DEMO_LISTVIEWDELEGATE_H
#define LVGL_XFJ_DEMO_LISTVIEWDELEGATE_H

#include "Delegate.hpp"

namespace heygears
{
    namespace widgets
    {
        template<typename T>
        class ListViewDelegate : public Delegate<T>
        {
        public:
            explicit ListViewDelegate(int index, BaseItem *parent = nullptr) : Delegate<T>(parent), index_(index)
            {}

        protected:
            int index_;
        };
    }
}
#endif //LVGL_XFJ_DEMO_LISTVIEWDELEGATE_H
