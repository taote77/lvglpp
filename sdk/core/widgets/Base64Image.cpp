#include <iostream>
#include <memory>
#include <utility>
#include "Base64Image.h"
#include "core/tools/Utils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "core/tools/stb/stb_image.h"
#include "core/tools/stb/stb_image_write.h"
#include "core/tools/stb/stb_image_resize2.h"

namespace heygears {
namespace widgets {

void Base64Resize(const std::string &src, std::string &output, int width, int height)
{
    if (src.empty()) {
        throw std::runtime_error("src is empty.");
    }

    // 解码Base64数据
    int srcImageWidth, srcImageHeight, srcImageChannels;
    unsigned char *srcImageData = stbi_load_from_memory(
            reinterpret_cast<const unsigned char *>(src.c_str()), static_cast<int>(src.length()),
            &srcImageWidth, &srcImageHeight, &srcImageChannels, 0);
    if (!srcImageData) {
        throw std::runtime_error("stbi_load_from_memory error.");
    }
    // 在这里使用智能指针来管理srcImageData内存
    std::unique_ptr<unsigned char, void (*)(void *)> imageDataGuard(srcImageData, stbi_image_free);

    if (srcImageHeight == height && srcImageWidth == width) {
        output = src;
        return;
    }

    // 计算等比例缩放后的尺寸
    float scale = std::min(static_cast<float>(width) / static_cast<float>(srcImageWidth),
                           static_cast<float>(height) / static_cast<float>(srcImageHeight));
    int newWidth = static_cast<int>(static_cast<float>(srcImageWidth) * scale);
    int newHeight = static_cast<int>(static_cast<float>(srcImageHeight) * scale);

    // 创建一个新的图像数据缓冲区
    const int outputChannels = srcImageChannels; // 保持通道数与原图一致
    std::unique_ptr<unsigned char[]> resizedData(
            new unsigned char[width * height * outputChannels]);

    // 获取输出图像的像素布局
    auto pixelLayout = stbir_pixel_layout(srcImageChannels);

    // 使用stbir_resize_uint8_srgb函数进行图像缩放
    bool resizeResult =
            stbir_resize_uint8_srgb(srcImageData, srcImageWidth, srcImageHeight, 0,
                                    resizedData.get(), newWidth, newHeight, 0, pixelLayout);
    if (!resizeResult) {
        throw std::runtime_error("stbir_resize_uint8_srgb error.");
    }

    // 创建新的画布，并将缩放后的图像居中显示，其余部分填充透明色
    int offsetX = (width - newWidth) / 2;
    int offsetY = (height - newHeight) / 2;
    std::unique_ptr<unsigned char[]> finalData(new unsigned char[width * height * outputChannels]);

    // 填充透明色
    for (int i = 0; i < width * height * outputChannels; i += outputChannels) {
        finalData[i] = 0; // R 通道
        finalData[i + 1] = 0; // G 通道
        finalData[i + 2] = 0; // B 通道
        if (outputChannels == 4) {
            finalData[i + 3] = 0; // Alpha 通道
        }
    }

    // 将缩放后的图像绘制到画布中心
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            int sourceIndex = (y * newWidth + x) * outputChannels;
            int targetIndex = ((y + offsetY) * width + x + offsetX) * outputChannels;

            for (int c = 0; c < outputChannels; ++c) {
                finalData[targetIndex + c] = resizedData[sourceIndex + c];
            }
        }
    }

    // 编码为Base64格式
    int encodedLength;
    unsigned char *encodedData = stbi_write_png_to_mem(finalData.get(), 0, width, height,
                                                       outputChannels, &encodedLength);
    if (!encodedData) {
        throw std::runtime_error("stbi_write_png_to_mem error.");
    }

    // 在这里使用智能指针来管理encodedData内存
    std::unique_ptr<unsigned char, void (*)(void *)> encodedDataGuard(encodedData, stbi_image_free);

    output = std::string(reinterpret_cast<char *>(encodedData), encodedLength);

    if (output.empty()) {
        throw std::runtime_error("encode error.");
    }
}

Base64Image::Base64Image(std::string base64, int w, int h, BaseItem *parent)
    : BaseItem(BaseItem::Image, parent), w_(w), h_(h), base64Data_(std::move(base64))
{
    updateImage(base64Data_);
}

Base64Image::~Base64Image()
{
    // delete[] img_struct_.data;
    // TODO

    lv_free((void *)img_struct_.data);
}

void Base64Image::updateImage(const std::string &data)
{
    if (data.empty()) {
        return;
    }

    std::string img_decode;
    std::string img_data;
    if (!tools::Utils::base64Decode(data, img_decode)) {
        return;
    }

    try {
        Base64Resize(img_decode, img_data, w_, h_);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return;
    }

    unsigned char *output_buf = nullptr;
    unsigned decode_w = 0, decode_h = 0;

    //    lv_image_dsc_t img_struct;
    //    img_struct.header.magic = LV_IMAGE_HEADER_MAGIC;
    //    img_struct.header.w  = data->width;
    //    img_struct.header.h  = data->height;
    //    img_struct.header.cf = LV_COLOR_FORMAT_ARGB8888;

    // TODO
    lodepng_decode32_2(&output_buf, &decode_w, &decode_h, (unsigned char *)img_data.data(),
                       img_data.length());

    img_struct_.header.magic = LV_IMAGE_HEADER_MAGIC;
    img_struct_.header.w = decode_w;
    img_struct_.header.h = decode_h;
    img_struct_.header.cf = LV_COLOR_FORMAT_ARGB8888;
    //     img_struct_.header.always_zero = 0;
    //     img_struct_.header.reserved = 0;
    img_struct_.data_size = decode_w * decode_h * 4;
    img_struct_.data = output_buf;
    lv_img_set_src(getLvglItem(), &img_struct_);
}
} // namespace widgets
} // namespace heygears
