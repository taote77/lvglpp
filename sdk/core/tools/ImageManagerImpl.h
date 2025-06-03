
#ifndef LVGLPP_IMAGEMANAGERIMPL_H
#define LVGLPP_IMAGEMANAGERIMPL_H

#include "core/db/AssetResDataBase.h"
#include "lvgl.h"
#include <map>
#include <string>

namespace lvglpp {
namespace tools {

class ImageManagerImpl
{
public:
    virtual ~ImageManagerImpl();
    static ImageManagerImpl *getInstance();
    lv_img_dsc_t            *getImageDscByUrl(const std::string &url);

private:
    static ImageManagerImpl *instance_;
    ImageManagerImpl();
    std::map<std::string, lv_img_dsc_t> app_image_map_{};
    db::AssetResDataBase               *asset_db_ = nullptr;
};

} // namespace tools
} // namespace lvglpp

#endif // LVGLPPP_IMAGEMANAGERIMPL_H
