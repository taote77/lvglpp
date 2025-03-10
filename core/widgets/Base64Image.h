/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-14

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_BASE64IMAGE_H
#define ULTRACORE_FOR_MAKERS_BASE64IMAGE_H
#include "BaseItem.h"
#include <string>
namespace heygears
{
    namespace widgets
    {
        class Base64Image:public BaseItem
        {
        public:
            explicit Base64Image(std::string  base64,int w,int h,BaseItem* parent);
            ~Base64Image() override;
            void updateImage(const std::string& data);

        private:
            lv_img_dsc_t img_struct_{};
            std::string base64Data_;
            int w_;
            int h_;

        };

    } // heygears
} // widgets

#endif //ULTRACORE_FOR_MAKERS_BASE64IMAGE_H
