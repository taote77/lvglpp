#include "core/base/LvObjFactory.h"

#include "Image.h"
#include "core/log/log.h"
#include "core/sys/UiConfig.h"
#include "core/tools/ImageManager.h"
#include "core/tools/Utils.h"
#include "core/tools/stb/StbImageLock.h"
#include "core/tools/stb/stb_image.h"
#include "core/tools/stb/stb_image_resize2.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <utility>
#include <vector>

namespace lvglpp::widgets {

struct Image::EncodedState {
    lv_image_dsc_t dsc{};
    lv_draw_buf_t *draw_buf = nullptr;

    ~EncodedState()
    {
        if (draw_buf != nullptr)
        {
            lv_draw_buf_destroy(draw_buf);
        }
    }
};

Image::Image(const std::string &assetPath, Widget *parent) : Widget(tools::LvObjFactory::createLvglImage, parent)
{
    setAsset(assetPath);
}

Image::Image(FileSource source, Widget *parent) : Widget(tools::LvObjFactory::createLvglImage, parent)
{
    setFile(source.path);
}

Image::Image(Base64Source source, Widget *parent) : Widget(tools::LvObjFactory::createLvglImage, parent)
{
    setBase64(source.data, source.width, source.height);
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
    source_kind_ = fileSource ? SourceKind::File : SourceKind::Asset;

    if (lvObj() == nullptr)
    {
        return;
    }

    if (path.empty())
    {
        lv_image_set_src(lvObj(), nullptr);
        clearObjectKeepalive("image.resource");
        image_resource_.reset();
        clearBase64State();
        source_kind_ = SourceKind::None;
        return;
    }

    tools::ImageHandle handle = fileSource ? tools::ImageManager::loadFile(path) : tools::ImageManager::load(path);
    if (!handle)
    {
        lv_image_set_src(lvObj(), nullptr);
        clearObjectKeepalive("image.resource");
        image_resource_.reset();
        clearBase64State();
        source_kind_ = SourceKind::None;
        return;
    }

    // Switch LVGL to the new descriptor before releasing the previous
    // resource, then retain the handle until the lv_obj is deleted.
    lv_image_set_src(lvObj(), handle->descriptor());
    setObjectKeepalive("image.resource", handle);
    image_resource_ = std::move(handle);
    clearBase64State();
}

void Image::setBase64(const std::string &data, int width, int height)
{
    if (data.empty() || width <= 0 || height <= 0 || lvObj() == nullptr)
    {
        return;
    }

    auto state = applyBase64(data, width, height);
    if (!state)
    {
        return;
    }

    // Switch LVGL to the new descriptor before releasing the previous source.
    lv_image_set_src(lvObj(), &state->dsc);
    setObjectKeepalive("image.base64", state);
    encoded_state_ = std::move(state);

    clearObjectKeepalive("image.resource");
    image_resource_.reset();

    base64_data_   = data;
    base64_width_  = width;
    base64_height_ = height;
    src_path_.clear();
    source_kind_ = SourceKind::Base64;
}

void Image::setBase64(const std::string &data)
{
    if (base64_width_ <= 0 || base64_height_ <= 0)
    {
        LOG_WARN() << "Image::setBase64(data) called without a previous canvas size";
        return;
    }
    setBase64(data, base64_width_, base64_height_);
}

std::shared_ptr<Image::EncodedState> Image::applyBase64(const std::string &data, int width, int height)
{
    std::string decoded;
    if (!tools::Utils::base64Decode(data, decoded))
    {
        LOG_ERROR() << "Image: invalid base64 data";
        return nullptr;
    }

    int            sourceWidth  = 0;
    int            sourceHeight = 0;
    int            channels     = 0;
    unsigned char *pixels       = nullptr;
    {
        // stb keeps process-global decoder state; serialize with ImageManager.
        std::lock_guard<std::mutex> stbLock(tools::stbImageMutex());
        pixels = stbi_load_from_memory(reinterpret_cast<const unsigned char *>(decoded.data()), static_cast<int>(decoded.size()),
                                       &sourceWidth, &sourceHeight, &channels, 4);
    }
    if (pixels == nullptr || sourceWidth <= 0 || sourceHeight <= 0)
    {
        LOG_ERROR() << "Image: failed to decode base64 image data";
        return nullptr;
    }
    (void)channels; // stbi expands to RGBA because desired_channels == 4

    std::unique_ptr<unsigned char, void (*)(void *)> pixelsGuard(pixels, stbi_image_free);

    // Preserve the aspect ratio and center the scaled image on a transparent
    // canvas of the requested size.
    const float scale   = std::min(static_cast<float>(width) / static_cast<float>(sourceWidth),
                                   static_cast<float>(height) / static_cast<float>(sourceHeight));
    const int   targetW = std::max(1, static_cast<int>(static_cast<float>(sourceWidth) * scale));
    const int   targetH = std::max(1, static_cast<int>(static_cast<float>(sourceHeight) * scale));
    const int   offsetX = (width - targetW) / 2;
    const int   offsetY = (height - targetH) / 2;

    std::vector<unsigned char> resized;
    const unsigned char       *image       = pixels;
    int                        imageStride = sourceWidth * 4;

    if (targetW != sourceWidth || targetH != sourceHeight)
    {
        resized.resize(static_cast<std::size_t>(targetW) * static_cast<std::size_t>(targetH) * 4);
        std::lock_guard<std::mutex> stbLock(tools::stbImageMutex());
        if (!stbir_resize_uint8_srgb(pixels, sourceWidth, sourceHeight, 0, resized.data(), targetW, targetH, 0, STBIR_RGBA))
        {
            LOG_ERROR() << "Image: failed to resize base64 image";
            return nullptr;
        }
        image       = resized.data();
        imageStride = targetW * 4;
    }

    lv_draw_buf_t *drawBuf =
        lv_draw_buf_create(static_cast<uint32_t>(width), static_cast<uint32_t>(height), LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    if (drawBuf == nullptr || drawBuf->data == nullptr)
    {
        LOG_ERROR() << "Image: failed to allocate draw buffer";
        return nullptr;
    }

    auto *destination = static_cast<unsigned char *>(drawBuf->data);
    std::memset(destination, 0, drawBuf->data_size);

    const uint32_t stride = drawBuf->header.stride;
    for (int y = 0; y < targetH; ++y)
    {
        const unsigned char *source = image + y * imageStride;
        unsigned char       *target = destination + (offsetY + y) * stride + offsetX * 4;
        for (int x = 0; x < targetW; ++x)
        {
            // Source is RGBA; LVGL's ARGB8888 is BGRA in memory.
            target[x * 4 + 0] = source[x * 4 + 2];
            target[x * 4 + 1] = source[x * 4 + 1];
            target[x * 4 + 2] = source[x * 4 + 0];
            target[x * 4 + 3] = source[x * 4 + 3];
        }
    }

    auto state               = std::make_shared<EncodedState>();
    state->draw_buf          = drawBuf;
    state->dsc.header.magic  = LV_IMAGE_HEADER_MAGIC;
    state->dsc.header.w      = static_cast<int32_t>(width);
    state->dsc.header.h      = static_cast<int32_t>(height);
    state->dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    state->dsc.header.stride = stride;
    state->dsc.data_size     = drawBuf->data_size;
    state->dsc.data          = destination;
    return state;
}

void Image::clearBase64State()
{
    clearObjectKeepalive("image.base64");
    encoded_state_.reset();
    base64_data_.clear();
    base64_width_  = 0;
    base64_height_ = 0;
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
    switch (source_kind_)
    {
    case SourceKind::Base64:
        if (!base64_data_.empty())
        {
            setBase64(base64_data_, base64_width_, base64_height_);
        }
        break;
    case SourceKind::Asset:
        if (!src_path_.empty())
        {
            tools::ImageManager::invalidate(src_path_);
            setAsset(src_path_);
        }
        break;
    case SourceKind::File:
        if (!src_path_.empty())
        {
            tools::ImageManager::invalidateFile(src_path_);
            setFile(src_path_);
        }
        break;
    case SourceKind::None:
        break;
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
