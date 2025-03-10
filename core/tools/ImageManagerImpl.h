/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-08-26

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_IMAGEMANAGERIMPL_H
#define ULTRACORE_FOR_MAKERS_IMAGEMANAGERIMPL_H
#include <string>
#include <map>
#include "lvgl.h"
#include "core/db/AssetResDataBase.h"
namespace heygears {
namespace tools {

class ImageManagerImpl
{
public:
    virtual ~ImageManagerImpl();
    static ImageManagerImpl *getInstance();
    lv_img_dsc_t *getImageDscByUrl(const std::string &url);
    std::string getImageFileDataByUrl(const std::string &url);

private:
    static ImageManagerImpl *instance_;
    ImageManagerImpl();
    std::map<std::string, lv_img_dsc_t> app_image_map_{};
    db::AssetResDataBase *asset_db_ = nullptr;
};

} // namespace tools
} // namespace heygears

#endif // ULTRACORE_FOR_MAKERS_IMAGEMANAGERIMPL_H
