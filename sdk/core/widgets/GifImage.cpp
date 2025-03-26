#include "GifImage.h"

namespace lvglpp {
namespace widgets {
GifImage::GifImage(const std::string &path, BaseItem *parent) : BaseItem(ItemType::GIF, parent)
{
    setImageSrcPath(path);
}

void GifImage::setImageSrcPath(const std::string &path)
{
    src_path_ = path;
    if (getLvglItem() != nullptr && !src_path_.empty()) {
        lv_gif_set_src(getLvglItem(), src_path_.c_str());
    }
}
} // namespace widgets
} // namespace lvglpp