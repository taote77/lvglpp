#ifndef LVGLPP_SMART_HOME_FORM_DESIGN_H
#define LVGLPP_SMART_HOME_FORM_DESIGN_H

/**
 * Design-token facade used by smart_home.
 *
 * The values are no longer maintained here: design/tokens.json is the source
 * of truth and sdk/tools/design/sync_tokens.py generates
 * sdk/config/color_sheet.h (dark/base) and color_sheet_light.h (light).
 */

#include "config/color_sheet.h" // IWYU pragma: export

namespace form::design {

using LColorType = uint32_t;

enum class DesignStyle {
    DARK    = 0,
    SHALLOW = 1,
};

/**
 * Legacy helper kept for existing call sites.
 *
 * CLR_* tokens resolve to the active ThemeMode before reaching this function,
 * so it is a no-op. Widgets that need a semantic role should use
 * core::Theme::color(ColorRole); the explicit pair overload below is available
 * for explicit dark/light choices.
 */
inline LColorType ThemeColor(LColorType color, DesignStyle = DesignStyle::DARK)
{
    return color;
}

/// Explicit pair for theme-aware call sites.
inline LColorType ThemeColor(LColorType dark, LColorType light, DesignStyle style)
{
    return style == DesignStyle::DARK ? dark : light;
}

} // namespace form::design

#endif // LVGLPP_SMART_HOME_FORM_DESIGN_H
