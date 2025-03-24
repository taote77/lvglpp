
#ifndef LVGLPP_IMAGEMANAGERIMPL_H
#define LVGLPP_IMAGEMANAGERIMPL_H

#include <string>
#include <map>
#include "lvgl.h"
#include "core/db/AssetResDataBase.h"

namespace heygears
{
    namespace tools
    {

        class ImageManagerImpl
        {
        public:
            virtual ~ImageManagerImpl();
            static ImageManagerImpl* getInstance();
            lv_img_dsc_t * getImageDscByUrl(const std::string& url);
            std::string getImageFileDataByUrl(const std::string& url);
        private:
            static ImageManagerImpl* instance_;
            ImageManagerImpl();
            std::map<std::string,lv_img_dsc_t> app_image_map_{};
            db::AssetResDataBase* asset_db_=nullptr;
        };

    } // heygears
} // tools

#endif //LVGLPPP_IMAGEMANAGERIMPL_H
