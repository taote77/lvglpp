/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-05

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_IMAGEMANAGER_H
#define ULTRACORE_FOR_MAKERS_IMAGEMANAGER_H
#include "lvgl.h"
#include <string>
namespace heygears
{
    namespace tools
    {

        class ImageManager
        {
        public:
            static lv_img_dsc_t * getImageDscByUrl(const std::string& url);
            static std::string getImageFileDataByUrl(const std::string& url);
        private:
        };

    } // heygears
} // tools

#endif //ULTRACORE_FOR_MAKERS_IMAGEMANAGER_H
