/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan eliang@heygears.com

Date:2022-09-01

Class:MouseArea

Description:

**************************************************************************/

#include "MouseArea.h"

namespace heygears
{
    namespace widgets
    {
        MouseArea::MouseArea(BaseItem *parent) : BaseItem(parent),
                                                 pressing_cb_(nullptr), released_cb_(nullptr)
        {
            this->setOpacity(0);
            lv_obj_add_event_cb(this->getLvglItem(),[](lv_event_t * e)->void{
                auto p=(MouseArea*)lv_event_get_user_data(e);
                if(p!=nullptr)
                {
                    if(p->pressing_cb_!=nullptr)
                    {
                        lv_indev_t * indev = lv_indev_get_act();
                        if(indev == NULL)  return;
                        lv_point_t vect;
                        lv_indev_get_point(indev, &vect);
                        Point pt(vect.x,vect.y);
                        p->pressing_cb_(pt);
                    }
                }
            },LV_EVENT_PRESSING, this);

            lv_obj_add_event_cb(this->getLvglItem(),[](lv_event_t * e)->void{
                auto p=(MouseArea*)lv_event_get_user_data(e);
                if(p!=nullptr)
                {
                    if(p->released_cb_!=nullptr)
                    {
                        lv_indev_t * indev = lv_indev_get_act();
                        if(indev == NULL)  return;
                        lv_point_t vect;
                        lv_indev_get_point(indev, &vect);
                        Point pt(vect.x,vect.y);
                        p->released_cb_(pt);
                    }
                }
            },LV_EVENT_RELEASED, this);
        }
    } // heygears
} // widgets