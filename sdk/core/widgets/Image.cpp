#include "core/base/LvObjFactory.h"

#include "Image.h"
#include "core/log/log.h"
#include "core/tools/ImageManager.h"

namespace lvglpp::widgets {

Image::Image(const std::string &assetPath, BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglImage, parent)
{
    setAsset(assetPath);
}

Image::Image(FileSource source, BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglImage, parent)
{
    setFile(source.path);
}

void Image::setAsset(const std::string &assetPath)
{
    applySource(assetPath, false);
}

void Image::setFile(const std::string &fsPath)
{
    applySource(fsPath, true);
}

void Image::applySource(const std::string &path, bool fileSource)
{
    src_path_    = path;
    file_source_ = fileSource;

    if (getLvglItem() == nullptr)
    {
        return;
    }

    if (path.empty())
    {
        lv_image_set_src(getLvglItem(), nullptr);
        clearObjectKeepalive("image.resource");
        image_resource_.reset();
        return;
    }

    tools::ImageHandle handle = fileSource ? tools::ImageManager::loadFile(path) : tools::ImageManager::load(path);
    if (!handle)
    {
        lv_image_set_src(getLvglItem(), nullptr);
        clearObjectKeepalive("image.resource");
        image_resource_.reset();
        return;
    }

    // Switch LVGL to the new descriptor before releasing the previous
    // resource, then retain the handle until the lv_obj is deleted.
    lv_image_set_src(getLvglItem(), handle->descriptor());
    setObjectKeepalive("image.resource", handle);
    image_resource_ = std::move(handle);
}

void Image::reload()
{
    if (src_path_.empty())
    {
        return;
    }

    if (file_source_)
    {
        tools::ImageManager::invalidateFile(src_path_);
        setFile(src_path_);
    } else
    {
        tools::ImageManager::invalidate(src_path_);
        setAsset(src_path_);
    }
}

void Image::setEnable(bool enable)
{
    BaseItem::setEnable(enable);
    if (enable && getLvglItem() != nullptr)
    {
        lv_obj_set_ext_click_area(getLvglItem(), 15);
    }
}

} // namespace lvglpp::widgets
