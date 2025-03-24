/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-15

Class:${CLASS}

Description:

**************************************************************************/

#include "PageIndicator.h"
#include "GlobalVar.h"
namespace heygears
{
    namespace widgets
    {
        PageIndicator::PageIndicator(int count, BaseItem *parent): BaseItem(parent),
                                                                   count_(count),current_index_(-1)
        {
            setSize((lv_coord_t)(12*count+8*(count-1)),12);
            lv_obj_set_flex_flow(getLvglItem(),LV_FLEX_FLOW_ROW);
            lv_obj_set_style_pad_column(getLvglItem(),8,LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(getLvglItem(),LV_OPA_TRANSP,LV_STATE_DEFAULT);
            for(int i=0;i<count_;i++)
            {
                indicator_vec_.emplace_back(std::make_shared<BaseItem>(this));
                indicator_vec_[i]->setSize(12,12);
                indicator_vec_[i]->setRadius(6);
            }
            update();
        }

        PageIndicator::PageIndicator(BaseItem *parent) : BaseItem(parent),count_(0),current_index_(-1)
        {
            lv_obj_set_flex_flow(getLvglItem(),LV_FLEX_FLOW_ROW);
            lv_obj_set_style_pad_column(getLvglItem(),8,LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(getLvglItem(),LV_OPA_TRANSP,LV_STATE_DEFAULT);
        }

        void PageIndicator::update()
        {
            for(int i=0;i<count_;i++)
            {
                if(current_index_==i)
                {
                    indicator_vec_[i]->setBgColor(CLR_PRIMARY_DARK_BG);
                }
                else
                {
                    indicator_vec_[i]->setBgColor(CLR_PRIMARY_BLACK_HOVER);
                }
            }
        }

        void PageIndicator::changeCurrentIndex(int n)
        {
            if(n==current_index_)
            {
                return;
            }
            current_index_=n;
            update();
        }

        void PageIndicator::setPageCount(int n)
        {
            count_=n;
            setSize((lv_coord_t)(12*count_+8*(count_-1)),12);
            lv_obj_clean(getLvglItem());
            indicator_vec_.clear();
            for(int i=0;i<count_;i++)
            {
                indicator_vec_.emplace_back(std::make_shared<BaseItem>(this));
                indicator_vec_[i]->setSize(12,12);
                indicator_vec_[i]->setRadius(6);
            }
            update();
        }
    } // ui
} // controls