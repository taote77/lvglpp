#include "GifImage.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

GifImage::GifImage(const std::string &assetPath, Widget *parent) : Widget(tools::LvObjFactory::createLvglGif, parent)
{
    setAsset(assetPath);
}

void GifImage::setAsset(const std::string &assetPath)
{
    src_path_ = assetPath;
    if (assetPath.empty())
    {
        applyLvglPath({});
        return;
    }
    applyLvglPath("A:/" + assetPath);
}

void GifImage::setFsPath(const std::string &lvglPath)
{
    src_path_ = lvglPath;
    applyLvglPath(lvglPath);
}

void GifImage::applyLvglPath(const std::string &lvglPath)
{
    lvgl_path_ = lvglPath;
    if (lvObj() == nullptr)
    {
        return;
    }

    if (lvgl_path_.empty())
    {
        lv_gif_set_src(lvObj(), nullptr);
        clearObjectKeepalive("gif.src");
        return;
    }

    auto persistent = std::make_shared<std::string>(lvgl_path_);
    setObjectKeepalive("gif.src", persistent);
    lv_gif_set_src(lvObj(), persistent->c_str());
}

} // namespace lvglpp::widgets
