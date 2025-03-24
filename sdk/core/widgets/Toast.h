/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-29

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_TOAST_H
#define ULTRACORE_FOR_MAKERS_TOAST_H
#include "BaseItem.h"
#include "Image.h"
#include "ConsumeText.h"
#include <memory>
namespace heygears
{
    namespace widgets
    {
        class Toast:public BaseItem
        {
        public:
            enum IconType{
                Succeed,
                Error,
                Warn
            };
            explicit Toast();
            void setMessage(const std::string& str_msg,IconType type);
        private:
            std::shared_ptr<widgets::Image> icon_;
            std::shared_ptr<ConsumeText> desc_;
        };

    } // heygears
} // widgets

#endif //ULTRACORE_FOR_MAKERS_TOAST_H
