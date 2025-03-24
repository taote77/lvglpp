/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-15

Class:PageIndicator

Description:样式和配色目前仅适用于消费级项目

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_PAGEINDICATOR_H
#define ULTRACORE_FOR_MAKERS_PAGEINDICATOR_H
#include "BaseItem.h"
#include <vector>
#include <memory>
namespace heygears
{
    namespace widgets
    {
        class PageIndicator:public BaseItem
        {
        public:
            explicit PageIndicator(int count,BaseItem* parent=nullptr);
            explicit PageIndicator(BaseItem* parent=nullptr);
            void changeCurrentIndex(int n);
            void setPageCount(int n);
        private:
            int count_;
            int current_index_;
            std::vector<std::shared_ptr<BaseItem>> indicator_vec_;
            void update();
        };
    } // ui
} // controls

#endif //ULTRACORE_FOR_MAKERS_PAGEINDICATOR_H
