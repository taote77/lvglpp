
#include "Image.h"
#include "core/tools/ImageManager.h"

namespace lvglpp {
namespace widgets {

Image::Image(const std::string &path, BaseItem *parent)
    : BaseItem(BaseItem::ItemType::Image, parent)
{
    setImageSrcPath(path);
}

void Image::setImageSrcPath(const std::string &path)
{
    src_path_ = path;
    if (getLvglItem() != nullptr && !src_path_.empty()) {
        if (src_path_[0] == ':') {
            lv_img_set_src(getLvglItem(), tools::ImageManager::getImageDscByUrl(src_path_));
        } else {
            lv_img_set_src(getLvglItem(), src_path_.c_str());
        }
    }
}

void Image::setEnable(bool enable)
{
    BaseItem::setEnable(enable);
    if (enable) {
        lv_obj_set_ext_click_area(getLvglItem(), 15);
    }
}
} // namespace widgets
} // namespace lvglpp
