#include "Base64Image.h"
#include "core/base/LvObjFactory.h"
#include "core/tools/Utils.h"
#include <cstdint>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "core/tools/stb/StbImageHelpers.h"
#include "core/tools/stb/StbImageLock.h"
#include "core/tools/stb/stb_image.h"
#include "core/tools/stb/stb_image_resize2.h"
#include "core/tools/stb/stb_image_write.h"

namespace lvglpp {
namespace widgets {

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

void Base64Resize(const std::string &src, std::string &output, int width, int height)
{
    if (src.empty())
    {
        throw std::runtime_error("src is empty.");
    }

    // 解码Base64数据
    int            srcImageWidth, srcImageHeight, srcImageChannels;
    unsigned char *srcImageData =
        stbi_load_from_memory(reinterpret_cast<const unsigned char *>(src.c_str()), static_cast<int>(src.length()), &srcImageWidth, &srcImageHeight, &srcImageChannels, 0);
    if (!srcImageData)
    {
        throw std::runtime_error("stbi_load_from_memory error.");
    }
    // 在这里使用智能指针来管理srcImageData内存
    std::unique_ptr<unsigned char, void (*)(void *)> imageDataGuard(srcImageData, stbi_image_free);

    if (srcImageHeight == height && srcImageWidth == width)
    {
        output = src;
        return;
    }

    // 计算等比例缩放后的尺寸
    float scale     = std::min(static_cast<float>(width) / static_cast<float>(srcImageWidth), static_cast<float>(height) / static_cast<float>(srcImageHeight));
    int   newWidth  = static_cast<int>(static_cast<float>(srcImageWidth) * scale);
    int   newHeight = static_cast<int>(static_cast<float>(srcImageHeight) * scale);

    // 创建一个新的图像数据缓冲区
    const int                        outputChannels = srcImageChannels; // 保持通道数与原图一致
    std::unique_ptr<unsigned char[]> resizedData(new unsigned char[width * height * outputChannels]);

    // 获取输出图像的像素布局
    auto pixelLayout = stbir_pixel_layout(srcImageChannels);

    // 使用stbir_resize_uint8_srgb函数进行图像缩放
    bool resizeResult = stbir_resize_uint8_srgb(srcImageData, srcImageWidth, srcImageHeight, 0, resizedData.get(), newWidth, newHeight, 0, pixelLayout);
    if (!resizeResult)
    {
        throw std::runtime_error("stbir_resize_uint8_srgb error.");
    }

    // 创建新的画布，并将缩放后的图像居中显示，其余部分填充透明色
    int                              offsetX = (width - newWidth) / 2;
    int                              offsetY = (height - newHeight) / 2;
    std::unique_ptr<unsigned char[]> finalData(new unsigned char[width * height * outputChannels]);

    // 填充透明色
    for (int i = 0; i < width * height * outputChannels; i += outputChannels)
    {
        finalData[i]     = 0; // R 通道
        finalData[i + 1] = 0; // G 通道
        finalData[i + 2] = 0; // B 通道
        if (outputChannels == 4)
        {
            finalData[i + 3] = 0; // Alpha 通道
        }
    }

    // 将缩放后的图像绘制到画布中心
    for (int y = 0; y < newHeight; ++y)
    {
        for (int x = 0; x < newWidth; ++x)
        {
            int sourceIndex = (y * newWidth + x) * outputChannels;
            int targetIndex = ((y + offsetY) * width + x + offsetX) * outputChannels;

            for (int c = 0; c < outputChannels; ++c)
            {
                finalData[targetIndex + c] = resizedData[sourceIndex + c];
            }
        }
    }

    // 编码为Base64格式
    const std::vector<std::uint8_t> encoded = tools::encodePng(finalData.get(), width, height, outputChannels);
    if (encoded.empty())
    {
        throw std::runtime_error("encodePng error.");
    }

    output = std::string(reinterpret_cast<const char *>(encoded.data()), encoded.size());

    if (output.empty())
    {
        throw std::runtime_error("encode error.");
    }
}

Base64Image::Base64Image(std::string base64, int w, int h, Widget *parent) : Widget(tools::LvObjFactory::createLvglImage, parent), w_(w), h_(h), base64Data_(std::move(base64))
{
    updateImage(base64Data_);
}

void Base64Image::updateImage(const std::string &data)
{
    if (data.empty() || lvObj() == nullptr)
    {
        return;
    }

    std::string img_decode;
    std::string img_data;
    if (!tools::Utils::base64Decode(data, img_decode))
    {
        return;
    }

    try
    {
        // stb keeps process-global decoder state; serialize with the
        // ImageManager prefetch worker.
        std::lock_guard<std::mutex> stbLock(tools::stbImageMutex());
        Base64Resize(img_decode, img_data, w_, h_);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return;
    }

    unsigned char *output_buf = nullptr;
    unsigned       decode_w   = 0;
    unsigned       decode_h   = 0;

    const unsigned decode_error = lodepng_decode32_2(&output_buf, &decode_w, &decode_h, (unsigned char *)img_data.data(), img_data.length());
    if (decode_error != 0 || output_buf == nullptr || decode_w == 0 || decode_h == 0)
    {
        std::cerr << "lodepng_decode32_2 failed: " << decode_error << '\n';
        return;
    }

    // LVGL's vendored lodepng is patched: the decoder returns an lv_draw_buf_t*
    // (ARGB8888) instead of a bare pixel buffer.
    auto *draw_buf = reinterpret_cast<lv_draw_buf_t *>(output_buf);
    if (draw_buf->data == nullptr || draw_buf->data_size < static_cast<uint32_t>(decode_w) * decode_h * 4)
    {
        lv_draw_buf_destroy(draw_buf);
        return;
    }

    // postProcessScanlines writes RGBA bytes; LVGL's ARGB8888 is BGRA in memory.
    const uint32_t pixel_bytes = static_cast<uint32_t>(decode_w) * decode_h * 4;
    for (uint32_t i = 0; i + 3 < pixel_bytes; i += 4)
    {
        std::swap(draw_buf->data[i], draw_buf->data[i + 2]);
    }

    auto state               = std::make_shared<ImageState>();
    state->draw_buf          = draw_buf;
    state->dsc.header.magic  = LV_IMAGE_HEADER_MAGIC;
    state->dsc.header.w      = static_cast<int32_t>(decode_w);
    state->dsc.header.h      = static_cast<int32_t>(decode_h);
    state->dsc.header.cf     = LV_COLOR_FORMAT_ARGB8888;
    state->dsc.header.stride = draw_buf->header.stride;
    state->dsc.data_size     = pixel_bytes;
    state->dsc.data          = draw_buf->data;

    // Switch LVGL to the new descriptor before releasing the previous state.
    lv_image_set_src(lvObj(), &state->dsc);
    setObjectKeepalive("base64image.dsc", state);
    image_state_ = std::move(state);
}
} // namespace widgets
} // namespace lvglpp
