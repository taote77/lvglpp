#ifndef LVGLPP_UI_TYPES_H
#define LVGLPP_UI_TYPES_H

#include "UiFeatures.h"

#include <cstdint>

namespace lvglpp::sys {

/**
 * Logical device orientation. The value is the clockwise rotation of the
 * native (landscape) panel in degrees.
 */
enum class Orientation : std::uint16_t {
    Landscape        = 0,
    Portrait         = 90,
    ReverseLandscape = 180,
    ReversePortrait  = 270,
};

enum class LayoutDirection : std::uint8_t {
    Ltr = 0,
    Rtl = 1,
};

struct UiConfiguration {
    Orientation     orientation = Orientation::Landscape;
    LayoutDirection direction   = LayoutDirection::Ltr;
    std::int32_t    width       = 800; // logical size after rotation
    std::int32_t    height      = 480;
    std::uint32_t   revision    = 0;

    constexpr bool isPortrait() const noexcept
    {
        return orientation == Orientation::Portrait || orientation == Orientation::ReversePortrait;
    }

    constexpr bool isReverse() const noexcept
    {
        return orientation == Orientation::ReverseLandscape || orientation == Orientation::ReversePortrait;
    }

    constexpr bool isRtl() const noexcept
    {
        return direction == LayoutDirection::Rtl;
    }

    constexpr int degrees() const noexcept
    {
        return static_cast<int>(orientation);
    }
};

constexpr Orientation normalizeOrientation(int degrees) noexcept
{
    int value = degrees % 360;
    if (value < 0)
    {
        value += 360;
    }
    switch (value)
    {
    case 90:
        return Orientation::Portrait;
    case 180:
        return Orientation::ReverseLandscape;
    case 270:
        return Orientation::ReversePortrait;
    default:
        return Orientation::Landscape;
    }
}

} // namespace lvglpp::sys

#endif // LVGLPP_UI_TYPES_H
