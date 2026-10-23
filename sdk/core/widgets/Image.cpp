#include "core/base/LvObjFactory.h"

#include "Image.h"
#include "core/log/log.h"
#include "core/sys/UiConfig.h"
#include "core/tools/ImageManager.h"

namespace lvglpp::widgets {

Image::Image(const std::string &assetPath, Widget *parent) : Widget(tools::LvObjFactory::createLvglImage, parent)
{
    setAsset(assetPath);
}

Image::Image(FileSource source, Widget *parent) : Widget(tools::LvObjFactory::createLvglImage, parent)
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

    if (lvObj() == nullptr)
    {
        return;
    }

    if (path.empty())
    {
        lv_image_set_src(lvObj(), nullptr);
        clearObjectKeepalive("image.resource");
        image_resource_.reset();
        return;
    }

    tools::ImageHandle handle = fileSource ? tools::ImageManager::loadFile(path) : tools::ImageManager::load(path);
    if (!handle)
    {
        lv_image_set_src(lvObj(), nullptr);
        clearObjectKeepalive("image.resource");
        image_resource_.reset();
        return;
    }

    // Switch LVGL to the new descriptor before releasing the previous
    // resource, then retain the handle until the lv_obj is deleted.
    lv_image_set_src(lvObj(), handle->descriptor());
    setObjectKeepalive("image.resource", handle);
    image_resource_ = std::move(handle);
}

void Image::setRtlAsset(const std::string &assetPath)
{
    rtl_asset_path_ = assetPath;
    if (sys::UiConfig::instance().isRtl())
    {
        setAsset(assetPath);
    }
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

void Image::setEnabled(bool enable)
{
    Widget::setEnabled(enable);
    if (lvObj() == nullptr)
    {
        return;
    }

    // Images are not clickable in LVGL by default. Enabling one is the
    // established way to turn it into a click target (e.g. back arrows).
    if (enable)
    {
        lv_obj_add_flag(lvObj(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_ext_click_area(lvObj(), 15);
    } else
    {
        lv_obj_clear_flag(lvObj(), LV_OBJ_FLAG_CLICKABLE);
    }
}

} // namespace lvglpp::widgets
