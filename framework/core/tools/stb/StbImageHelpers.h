#ifndef LVGLPP_STBIMAGEHELPERS_H
#define LVGLPP_STBIMAGEHELPERS_H

#include <cstdint>
#include <vector>

namespace lvglpp::tools {

/**
 * Encode raw pixels as PNG into memory.
 *
 * stb_image_write exposes stbi_write_png_to_mem only in its implementation
 * translation unit, so the SDK provides this small wrapper for the few places
 * that need an in-memory PNG (Image base64 sources and tests).
 */
std::vector<std::uint8_t> encodePng(const std::uint8_t *pixels, int width, int height, int channels);

} // namespace lvglpp::tools

#endif // LVGLPP_STBIMAGEHELPERS_H
