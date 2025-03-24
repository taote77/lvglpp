/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-29

Class:${CLASS}

Description:

**************************************************************************/

#include "Toast.h"
#include "GlobalVar.h"

namespace heygears
{
    namespace widgets
    {
        Toast::Toast(): BaseItem(BaseItem::Dialog)
        {
            lv_obj_clear_flag(getLvglItem(), LV_OBJ_FLAG_SCROLLABLE);
            setBgColor(CLR_PRIMARY_DIS_TEXT_PRESS);
            setRadius(8);
            setAligment(LV_ALIGN_TOP_MID,0,70);
            icon_=std::make_shared<widgets::Image>("",this);
            icon_->setAligment(LV_ALIGN_LEFT_MID,8,0);
            desc_=std::make_shared<ConsumeText>("",CLR_PRIMARY_BRIGHT,this,ConsumeText::FontSize24,ConsumeText::SansSC);
            desc_->setAligment(LV_ALIGN_LEFT_MID,64, 0);
        }

        void Toast::setMessage(const std::string &str_msg, Toast::IconType type)
        {
            //setSize(str_msg.length()*12+80,56);
            desc_->setText(str_msg);
            setSize(desc_->getDisplayTextWidth()+72,56);
            if(type==Toast::Succeed)
            {
                icon_->setImageSrcPath(":/image/40x40/icon_success.png");
            }
            else if(type==Toast::Warn)
            {
                icon_->setImageSrcPath(":/image/40x40/icon_warn.png");
            }
            else
            {
                icon_->setImageSrcPath(":/image/40x40/icon_error.png");
            }
        }
    } // heygears
} // widgets