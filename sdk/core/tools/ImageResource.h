#ifndef LVGLPP_IMAGERESOURCE_H
#define LVGLPP_IMAGERESOURCE_H

#include "lvgl.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace lvglpp::tools {

/**
 * Decoded ARGB8888 image owned through shared_ptr.
 *
 * LVGL keeps a raw pointer to descriptor(), so every widget using the image
 * must hold the ImageHandle for as long as the lv_obj can render it. The
 * ImageManager LRU cache owns a strong reference for recently used images;
 * eviction only drops the cache's reference, never invalidates an image that
 * is still in use.
 */
class ImageResource
{
public:
    ImageResource(std::string url, int width, int height, std::unique_ptr<uint8_t[]> pixels);

    ImageResource(const ImageResource &)            = delete;
    ImageResource &operator=(const ImageResource &) = delete;

    const std::string &url() const
    {
        return url_;
    }

    const lv_img_dsc_t *descriptor() const
    {
        return &dsc_;
    }

    lv_img_dsc_t *descriptor()
    {
        return &dsc_;
    }

    int width() const
    {
        return width_;
    }

    int height() const
    {
        return height_;
    }

    std::size_t byteSize() const
    {
        return static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_) * 4;
    }

private:
    std::string                url_;
    int                        width_  = 0;
    int                        height_ = 0;
    std::unique_ptr<uint8_t[]> pixels_;
    lv_img_dsc_t               dsc_{};
};

using ImageHandle = std::shared_ptr<ImageResource>;

} // namespace lvglpp::tools

#endif // LVGLPP_IMAGERESOURCE_H
