/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-05

Class:${CLASS}

Description:

**************************************************************************/

#include "ImageManager.h"
#include "ImageManagerImpl.h"
namespace heygears
{
    namespace tools
    {
        lv_img_dsc_t *ImageManager::getImageDscByUrl(const std::string& url)
        {
            return ImageManagerImpl::getInstance()->getImageDscByUrl(url);
        }

        std::string ImageManager::getImageFileDataByUrl(const std::string& url)
        {
            return ImageManagerImpl::getInstance()->getImageFileDataByUrl(url);
        }
    } // heygears
} // tools