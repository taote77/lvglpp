// Single translation unit providing the stb implementations used by the SDK.
// Keeping them here (instead of a widget .cpp) avoids ImageManager depending
// on symbols compiled in Image.cpp.

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_resize2.h"
#include "stb_image_write.h"

#include "StbImageHelpers.h"

namespace lvglpp::tools {

std::vector<std::uint8_t> encodePng(const std::uint8_t *pixels, int width, int height, int channels)
{
    int            length  = 0;
    unsigned char *encoded = stbi_write_png_to_mem(pixels, 0, width, height, channels, &length);
    if (encoded == nullptr)
    {
        return {};
    }

    std::vector<std::uint8_t> result(encoded, encoded + length);
    stbi_image_free(encoded);
    return result;
}

} // namespace lvglpp::tools
