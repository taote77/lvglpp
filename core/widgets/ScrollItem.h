/**************************************************************************

Copyright:Copyright  2022 HeyGears. All rights reserved

Author: Lhyang hli3@heygears.com

Date:2022-11-29

Class:ScrollItem

Description:

**************************************************************************/

#ifndef ULTRACORE_REFLEX_SCROLLITEM_H
#define ULTRACORE_REFLEX_SCROLLITEM_H

#include "BaseItem.h"
namespace heygears
{
    namespace widgets
    {
        class ScrollItem : public BaseItem
        {
        public:
            ScrollItem(BaseItem *parent);

            void setScrollable(bool b);

            bool getScrollable();

            void setFlickable(bool b);

            bool getFlickable();

            void setScrollBarVisible(bool b);

            void setScrollSnapping(lv_scroll_snap_t l);

            lv_coord_t getScrollSnapX();

            void setScrollToX(lv_coord_t x, bool animEnable);

            lv_coord_t getScrollSnapY();

            void setScrollToY(lv_coord_t y, bool animEnable);
        };
    }
}


#endif //ULTRACORE_REFLEX_SCROLLITEM_H
