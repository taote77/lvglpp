#include "Base64Image.h"
#include "core/base/LvObjFactory.h"
#include "core/log/log.h"
#include "core/tools/Utils.h"
#include "core/tools/stb/StbImageLock.h"
#include "core/tools/stb/stb_image.h"
#include "core/tools/stb/stb_image_resize2.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace lvglpp::widgets {

struct Base64Image::ImageState {
    lv_image_dsc_t dsc{};
    lv_draw_buf_t *draw_buf = nullptr;

    ~ImageState()
    {
        if (draw_buf != nullptr)
        {
            lv_draw_buf_destroy(draw_buf);
        }
    }
};

Base64Image::Base64Image(std::string base64, int width, int height, Widget *parent) :
    Widget(tools::LvObjFactory::createLvglImage, parent), width_(width), height_(height), base64_(std::move(base64))
{
    setBase64(base64_);
}

void Base64Image::setBase64(const std::string &data)
{
    base64_ = data;
    if (data.empty() || lvObj() == nullptr)
    {
        return;
    }
    if (width_ <= 0 || height_ <= 0)
    {
        LOG_ERROR() << "Base64Image requires a positive target size, got " << width_ << "x" << height_;
        return;
    }

    std::string decoded;
    if (!tools::Utils::base64Decode(data, decoded))
    {
        LOG_ERROR() << "Base64Image: invalid base64 data";
        return;
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
        LOG_ERROR() << "Base64Image: failed to decode image data";
        return;
    }
    (void)channels; // stbi expands to RGBA because desired_channels == 4

    std::unique_ptr<unsigned char, void (*)(void *)> pixelsGuard(pixels, stbi_image_free);
    applyDecoded(pixels, sourceWidth, sourceHeight);
}

void Base64Image::applyDecoded(const unsigned char *rgba, int sourceWidth, int sourceHeight)
{
    if (rgba == nullptr || sourceWidth <= 0 || sourceHeight <= 0)
    {
        return;
    }

    // Preserve the aspect ratio and center the scaled image on a transparent
    // canvas of the requested size.
    const float scale    = std::min(static_cast<float>(width_) / static_cast<float>(sourceWidth),
                                    static_cast<float>(height_) / static_cast<float>(sourceHeight));
    const int   targetW  = std::max(1, static_cast<int>(static_cast<float>(sourceWidth) * scale));
    const int   targetH  = std::max(1, static_cast<int>(static_cast<float>(sourceHeight) * scale));
    const int   offsetX  = (width_ - targetW) / 2;
    const int   offsetY  = (height_ - targetH) / 2;

    std::vector<unsigned char> resized;
    const unsigned char       *image       = rgba;
    int                        imageStride = sourceWidth * 4;

    if (targetW != sourceWidth || targetH != sourceHeight)
    {
        resized.resize(static_cast<std::size_t>(targetW) * static_cast<std::size_t>(targetH) * 4);
        std::lock_guard<std::mutex> stbLock(tools::stbImageMutex());
        if (!stbir_resize_uint8_srgb(rgba, sourceWidth, sourceHeight, 0, resized.data(), targetW, targetH, 0, STBIR_RGBA))
        {
            LOG_ERROR() << "Base64Image: failed to resize image";
            return;
        }
        image       = resized.data();
        imageStride = targetW * 4;
    }

    lv_draw_buf_t *drawBuf = lv_draw_buf_create(static_cast<uint32_t>(width_), static_cast<uint32_t>(height_),
                                                LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    if (drawBuf == nullptr || drawBuf->data == nullptr)
    {
        LOG_ERROR() << "Base64Image: failed to allocate draw buffer";
        return;
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

    auto state               = std::make_shared<ImageState>();
    state->draw_buf          = drawBuf;
    state->dsc.header.magic  = LV_IMAGE_HEADER_MAGIC;
    state->dsc.header.w      = static_cast<int32_t>(width_);
    state->dsc.header.h      = static_cast<int32_t>(height_);
    state->dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    state->dsc.header.stride = stride;
    state->dsc.data_size     = drawBuf->data_size;
    state->dsc.data          = destination;

    // Switch LVGL to the new descriptor before releasing the previous state.
    lv_image_set_src(lvObj(), &state->dsc);
    setObjectKeepalive("base64image.dsc", state);
    image_state_ = std::move(state);
}

} // namespace lvglpp::widgets
