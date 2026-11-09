#ifndef LVGLPP_GRIDLAYOUT_H
#define LVGLPP_GRIDLAYOUT_H

#include "lvgl.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace lvglpp::widgets {

/**
 * Parser for LVGL grid track descriptors.
 *
 * Supported tokens (whitespace separated):
 *   1fr 2fr          fractional tracks (clamped to 1..255)
 *   auto | content   content-sized track
 *   100px | 100      fixed pixel track
 *
 * The produced vector always ends with LV_GRID_TEMPLATE_LAST.
 */
class GridLayout
{
public:
    static constexpr uint32_t kMaxTracks = 32;

    static bool parseTracks(std::string_view desc, std::vector<lv_coord_t> &out, std::string *error = nullptr);
};

} // namespace lvglpp::widgets

#endif // LVGLPP_GRIDLAYOUT_H
