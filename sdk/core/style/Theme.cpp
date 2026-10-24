#include "Theme.h"

#include "config/color_sheet.h"
#include "config/color_sheet_light.h"

namespace lvglpp::core {

namespace {

using namespace form::design;
namespace light = form::design::light;

struct PaletteEntry {
    ColorRole role;
    uint32_t  dark;
    uint32_t  light;
};

// Both themes are defined side by side so it is impossible to add a role to
// one palette and forget the other.
constexpr PaletteEntry kPalette[] = {
    {ColorRole::Primary, CLR_PRIMARY, light::CLR_PRIMARY},
    {ColorRole::OnPrimary, CLR_ON_PRIMARY, light::CLR_ON_PRIMARY},
    {ColorRole::PrimaryContainer, CLR_PRIMARY_CONTAINER, light::CLR_PRIMARY_CONTAINER},
    {ColorRole::OnPrimaryContainer, CLR_ON_PRIMARY_CONTAINER, light::CLR_ON_PRIMARY_CONTAINER},

    {ColorRole::Secondary, CLR_SECONDARY, light::CLR_SECONDARY},
    {ColorRole::OnSecondary, CLR_ON_SECONDARY, light::CLR_ON_SECONDARY},
    {ColorRole::SecondaryContainer, CLR_SECONDARY_CONTAINER, light::CLR_SECONDARY_CONTAINER},
    {ColorRole::OnSecondaryContainer, CLR_ON_SECONDARY_CONTAINER, light::CLR_ON_SECONDARY_CONTAINER},

    {ColorRole::Surface, CLR_SURFACE, light::CLR_SURFACE},
    {ColorRole::SurfaceDim, CLR_SURFACE_DIM, light::CLR_SURFACE_DIM},
    {ColorRole::SurfaceCanvas, CLR_SURFACE_CANVAS, light::CLR_SURFACE_CANVAS},
    {ColorRole::SurfaceContainer, CLR_SURFACE_CONTAINER, light::CLR_SURFACE_CONTAINER},
    {ColorRole::SurfaceContainerHigh, CLR_SURFACE_CONTAINER_HIGH, light::CLR_SURFACE_CONTAINER_HIGH},
    {ColorRole::SurfaceContainerHighest, CLR_SURFACE_CONTAINER_HIGHEST, light::CLR_SURFACE_CONTAINER_HIGHEST},

    {ColorRole::OnSurface, CLR_ON_SURFACE, light::CLR_ON_SURFACE},
    {ColorRole::OnSurfaceVariant, CLR_ON_SURFACE_VARIANT_1, light::CLR_ON_SURFACE_VARIANT_1},
    {ColorRole::Outline, CLR_OUTLINE, light::CLR_OUTLINE},
    {ColorRole::OutlineVariant, CLR_OUTLINE_VARIANT, light::CLR_OUTLINE_VARIANT},

    {ColorRole::Error, CLR_ERROR, light::CLR_ERROR},
    {ColorRole::OnError, CLR_ON_ERROR, light::CLR_ON_ERROR},
    {ColorRole::Warning, CLR_WARNING, light::CLR_WARNING},
    {ColorRole::OnWarning, CLR_ON_WARNING, light::CLR_ON_WARNING},
    {ColorRole::Success, CLR_SUCCESS, light::CLR_SUCCESS},
    {ColorRole::OnSuccess, CLR_ON_SUCCESS, light::CLR_ON_SUCCESS},
    {ColorRole::Info, CLR_INFO, light::CLR_INFO},

    {ColorRole::FocusRing, CLR_PRIMARY, light::CLR_PRIMARY},
    {ColorRole::Disabled, CLR_PRIMARY_DIS_BG, light::CLR_PRIMARY_DIS_BG},
    {ColorRole::DialogMask, CLR_DIALOG_BG_MASK, light::CLR_DIALOG_BG_MASK},

    {ColorRole::ButtonBlue, CLR_WIDGET_BUTTON_BLUE, light::CLR_WIDGET_BUTTON_BLUE},
    {ColorRole::ButtonBluePressed, CLR_WIDGET_BUTTON_BLUE_DIS, light::CLR_WIDGET_BUTTON_BLUE_DIS},
    {ColorRole::ButtonDisabled, CLR_SURFACE_DIS_CONTAINER_1, light::CLR_SURFACE_DIS_CONTAINER_1},
    {ColorRole::ButtonGrayPressed, CLR_WIDGET_BUTTON_GRAY_DIS, light::CLR_WIDGET_BUTTON_GRAY_DIS},
    {ColorRole::ButtonDarkBg, CLR_PRIMARY_DARK_BG, light::CLR_PRIMARY_DARK_BG},
    {ColorRole::ButtonDarkPressed, CLR_BUTTON_DARK_PRESSED, light::CLR_BUTTON_DARK_PRESSED},
    {ColorRole::ButtonDarkDisabled, CLR_BUTTON_DARK_DISABLED, light::CLR_BUTTON_DARK_DISABLED},
    {ColorRole::ButtonLightBlueBg, CLR_BUTTON_LIGHT_BLUE_BG, light::CLR_BUTTON_LIGHT_BLUE_BG},
    {ColorRole::ButtonLightBluePressed, CLR_BUTTON_LIGHT_BLUE_PRESSED, light::CLR_BUTTON_LIGHT_BLUE_PRESSED},
    {ColorRole::ButtonRedBg, CLR_WARNING_DARK, light::CLR_WARNING_DARK},
    {ColorRole::ButtonRedPressed, CLR_BUTTON_RED_PRESSED, light::CLR_BUTTON_RED_PRESSED},
    {ColorRole::ButtonDarkGrayBg, CLR_BUTTON_DARK_GRAY_BG, light::CLR_BUTTON_DARK_GRAY_BG},
};

static_assert(sizeof(kPalette) / sizeof(kPalette[0]) == static_cast<size_t>(ColorRole::Count), "every ColorRole needs dark and light design tokens");

uint32_t &at(std::array<uint32_t, static_cast<size_t>(ColorRole::Count)> &palette, ColorRole role)
{
    return palette[static_cast<size_t>(role)];
}

} // namespace

Theme &Theme::instance()
{
    // Process-lifetime: styles are attached to LVGL objects and must not be
    // reset by a static destructor after lv_deinit().
    static Theme *theme = new Theme();
    return *theme;
}

Theme::Theme()
{
    rebuild();
}

void Theme::setMode(ThemeMode mode)
{
    if (mode_ == mode)
    {
        return;
    }
    mode_ = mode;
    rebuild();
}

ThemeMode Theme::mode() const
{
    return mode_;
}

uint32_t Theme::hex(ColorRole role) const
{
    const auto index = static_cast<size_t>(role);
    if (index >= palette_.size())
    {
        return 0;
    }
    return palette_[index];
}

lv_color_t Theme::color(ColorRole role) const
{
    return lv_color_hex(hex(role));
}

const ButtonPalette &Theme::buttonPalette(ButtonStyle style) const
{
    const auto index = static_cast<size_t>(style);
    if (index >= buttons_.size())
    {
        return buttons_[0];
    }
    return buttons_[index];
}

const Style &Theme::focusStyle() const
{
    return *focus_style_;
}

const Style &Theme::cardStyle() const
{
    return *card_style_;
}

const Style &Theme::dialogStyle() const
{
    return *dialog_style_;
}

const Style &Theme::buttonBaseStyle() const
{
    return *button_base_style_;
}

void Theme::rebuild()
{
    const bool is_dark = mode_ == ThemeMode::Dark;
    for (const auto &entry : kPalette)
    {
        at(palette_, entry.role) = is_dark ? entry.dark : entry.light;
    }

    // ---- button palettes ----
    auto &blue          = buttons_[static_cast<size_t>(ButtonStyle::Blue)];
    blue.normal_bg      = hex(ColorRole::ButtonBlue);
    blue.pressed_bg     = hex(ColorRole::ButtonBluePressed);
    blue.disabled_bg    = hex(ColorRole::ButtonDisabled);
    blue.label_color    = hex(ColorRole::OnPrimary);
    blue.label_disabled = hex(ColorRole::Disabled);

    auto &dark          = buttons_[static_cast<size_t>(ButtonStyle::Dark)];
    dark.normal_bg      = hex(ColorRole::ButtonDarkBg);
    dark.pressed_bg     = hex(ColorRole::ButtonDarkPressed);
    dark.disabled_bg    = hex(ColorRole::ButtonDarkDisabled);
    dark.label_color    = hex(ColorRole::OnSurface);
    dark.label_disabled = hex(ColorRole::Disabled);

    auto &gray          = buttons_[static_cast<size_t>(ButtonStyle::Gray)];
    gray.normal_bg      = hex(ColorRole::Secondary);
    gray.pressed_bg     = hex(ColorRole::ButtonGrayPressed);
    gray.disabled_bg    = hex(ColorRole::ButtonDisabled);
    gray.label_color    = hex(ColorRole::OnSurface);
    gray.label_disabled = hex(ColorRole::Disabled);

    auto &light_blue          = buttons_[static_cast<size_t>(ButtonStyle::LightBlue)];
    light_blue.normal_bg      = hex(ColorRole::ButtonLightBlueBg);
    light_blue.pressed_bg     = hex(ColorRole::ButtonLightBluePressed);
    light_blue.disabled_bg    = hex(ColorRole::ButtonLightBlueBg);
    light_blue.label_color    = CLR_PRIMARY_BLUE;
    light_blue.label_disabled = hex(ColorRole::Disabled);

    auto &black_border          = buttons_[static_cast<size_t>(ButtonStyle::BlackBorder)];
    black_border.transparent    = true;
    black_border.normal_bg      = 0;
    black_border.pressed_bg     = 0;
    black_border.disabled_bg    = 0;
    black_border.border_color   = hex(ColorRole::Outline);
    black_border.label_color    = CLR_TEXT_PRIMARY_BLACK;
    black_border.label_disabled = hex(ColorRole::Disabled);

    auto &red          = buttons_[static_cast<size_t>(ButtonStyle::Red)];
    red.normal_bg      = hex(ColorRole::ButtonRedBg);
    red.pressed_bg     = hex(ColorRole::ButtonRedPressed);
    red.disabled_bg    = hex(ColorRole::ButtonDarkDisabled);
    red.label_color    = hex(ColorRole::OnError);
    red.label_disabled = hex(ColorRole::Disabled);

    auto &dark_gray          = buttons_[static_cast<size_t>(ButtonStyle::DarkGray)];
    dark_gray.normal_bg      = hex(ColorRole::ButtonDarkGrayBg);
    dark_gray.pressed_bg     = hex(ColorRole::ButtonDarkDisabled);
    dark_gray.disabled_bg    = hex(ColorRole::ButtonDisabled);
    dark_gray.label_color    = hex(ColorRole::OnSurface);
    dark_gray.label_disabled = hex(ColorRole::Disabled);

    // ---- shared styles ----
    focus_style_ = std::make_unique<Style>();
    focus_style_->outline(2, hex(ColorRole::FocusRing), 2);

    card_style_ = std::make_unique<Style>();
    card_style_->bgColor(hex(ColorRole::SurfaceContainer));
    card_style_->radius(RADIUS_16);
    card_style_->border(2, hex(ColorRole::OutlineVariant));

    dialog_style_ = std::make_unique<Style>();
    dialog_style_->bgColor(hex(ColorRole::SurfaceContainer));
    dialog_style_->radius(0);

    button_base_style_ = std::make_unique<Style>();
    button_base_style_->radius(RADIUS_8);
}

} // namespace lvglpp::core
