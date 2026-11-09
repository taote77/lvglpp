#ifndef LVGLPP_THEME_H
#define LVGLPP_THEME_H

#include "Style.h"

#include <array>
#include <cstdint>
#include <memory>

namespace lvglpp::core {

enum class ThemeMode {
    Dark,
    Light,
};

enum class ColorRole {
    Primary,
    OnPrimary,
    PrimaryContainer,
    OnPrimaryContainer,

    Secondary,
    OnSecondary,
    SecondaryContainer,
    OnSecondaryContainer,

    Surface,
    SurfaceDim,
    SurfaceCanvas,
    SurfaceContainer,
    SurfaceContainerHigh,
    SurfaceContainerHighest,

    OnSurface,
    OnSurfaceVariant,
    Outline,
    OutlineVariant,

    Error,
    OnError,
    Warning,
    OnWarning,
    Success,
    OnSuccess,
    Info,

    FocusRing,
    Disabled,
    DialogMask,

    // Legacy button roles (dark values preserve the existing smart_home look)
    ButtonBlue,
    ButtonBluePressed,
    ButtonDisabled,
    ButtonGrayPressed,
    ButtonDarkBg,
    ButtonDarkPressed,
    ButtonDarkDisabled,
    ButtonLightBlueBg,
    ButtonLightBluePressed,
    ButtonRedBg,
    ButtonRedPressed,
    ButtonDarkGrayBg,

    Count,
};

enum class ButtonStyle {
    Blue,
    Dark,
    Gray,
    LightBlue,
    BlackBorder,
    Red,
    DarkGray,
    Count,
};

struct ButtonPalette {
    uint32_t normal_bg      = 0;
    uint32_t pressed_bg     = 0;
    uint32_t disabled_bg    = 0;
    uint32_t label_color    = 0;
    uint32_t label_disabled = 0;
    bool     transparent    = false;
    uint32_t border_color   = 0;
};

/**
 * Semantic color palette and shared styles.
 *
 * Set the mode before building the UI. Existing widgets keep the styles they
 * were created with; switching mode does not retroactively restyle them.
 */
class Theme
{
public:
    static Theme &instance();

    Theme(const Theme &)            = delete;
    Theme &operator=(const Theme &) = delete;

    void setMode(ThemeMode mode);

    ThemeMode mode() const;

    uint32_t hex(ColorRole role) const;

    lv_color_t color(ColorRole role) const;

    const ButtonPalette &buttonPalette(ButtonStyle style) const;

    const Style &focusStyle() const;

    const Style &cardStyle() const;

    const Style &dialogStyle() const;

    const Style &buttonBaseStyle() const;

private:
    Theme();

    void rebuild();

    ThemeMode mode_ = ThemeMode::Dark;

    std::array<uint32_t, static_cast<size_t>(ColorRole::Count)>        palette_{};
    std::array<ButtonPalette, static_cast<size_t>(ButtonStyle::Count)> buttons_{};

    std::unique_ptr<Style> focus_style_;
    std::unique_ptr<Style> card_style_;
    std::unique_ptr<Style> dialog_style_;
    std::unique_ptr<Style> button_base_style_;
};

} // namespace lvglpp::core

#endif // LVGLPP_THEME_H
