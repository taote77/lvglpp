/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan eliang@heygears.com

Date:2022-09-01

Class:MouseArea

Description:透明的触摸事件控件

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_MOUSEAREA_H
#define ULTRACORE_FOR_MAKERS_MOUSEAREA_H
#include "BaseItem.h"
#include <functional>
namespace heygears
{
    namespace widgets
    {
        struct Point
        {
            int32_t x;
            int32_t y;

            Point()
            {
                x=0;
                y=0;
            }
            Point(int xPos,int yPos)
            {
                x=xPos;
                y=yPos;
            }
        };
        typedef std::function<void(const Point&)> MousePressingCB;
        typedef std::function<void(const Point&)> MouseReleaseCB;
        class MouseArea:public BaseItem
        {
        public:
            explicit MouseArea(BaseItem* parent=nullptr);
            void setMousePressingListener(MousePressingCB cb)
            {
                pressing_cb_=cb;
            }
            void setMouseReleasedListener(MouseReleaseCB cb)
            {
                released_cb_=cb;
            }
        private:
            MousePressingCB pressing_cb_;
            MouseReleaseCB released_cb_;
        };

    } // heygears
} // widgets

#endif //ULTRACORE_FOR_MAKERS_MOUSEAREA_H
