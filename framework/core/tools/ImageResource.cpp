#include "ImageResource.h"

#include <utility>

namespace lvglpp::tools {

ImageResource::ImageResource(std::string url, int width, int height, std::unique_ptr<uint8_t[]> pixels) :
    url_(std::move(url)), width_(width), height_(height), pixels_(std::move(pixels))
{
    dsc_.header.magic      = LV_IMAGE_HEADER_MAGIC;
    dsc_.header.w          = width_;
    dsc_.header.h          = height_;
    dsc_.header.cf         = LV_COLOR_FORMAT_ARGB8888;
    dsc_.header.stride     = static_cast<uint32_t>(width_) * 4;
    dsc_.header.reserved_2 = 0;
    dsc_.data_size         = static_cast<uint32_t>(byteSize());
    dsc_.data              = pixels_.get();
}

} // namespace lvglpp::tools
