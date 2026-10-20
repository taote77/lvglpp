#include "Theme.h"

#include "config/color_sheet.h"

namespace lvglpp::core {

namespace {

using namespace form::design;

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
    if (mode_ == ThemeMode::Dark)
    {
        at(palette_, ColorRole::Primary)            = CLR_PRIMARY;
        at(palette_, ColorRole::OnPrimary)          = CLR_ON_PRIMARY;
        at(palette_, ColorRole::PrimaryContainer)   = CLR_PRIMARY_CONTAINER;
        at(palette_, ColorRole::OnPrimaryContainer) = CLR_ON_PRIMARY_CONTAINER;

        at(palette_, ColorRole::Secondary)            = CLR_SECONDARY;
        at(palette_, ColorRole::OnSecondary)          = CLR_ON_SECONDARY;
        at(palette_, ColorRole::SecondaryContainer)   = CLR_SECONDARY_CONTAINER;
        at(palette_, ColorRole::OnSecondaryContainer) = CLR_ON_SECONDARY_CONTAINER;

        at(palette_, ColorRole::Surface)                 = CLR_SURFACE;
        at(palette_, ColorRole::SurfaceDim)              = CLR_SURFACE_DIM;
        at(palette_, ColorRole::SurfaceCanvas)           = 0x444E5B;
        at(palette_, ColorRole::SurfaceContainer)        = CLR_SURFACE_CONTAINER;
        at(palette_, ColorRole::SurfaceContainerHigh)    = CLR_SURFACE_CONTAINER_HIGH;
        at(palette_, ColorRole::SurfaceContainerHighest) = CLR_SURFACE_CONTAINER_HIGHEST;

        at(palette_, ColorRole::OnSurface)        = CLR_ON_SURFACE;
        at(palette_, ColorRole::OnSurfaceVariant) = CLR_ON_SURFACE_VARIANT_1;
        at(palette_, ColorRole::Outline)          = CLR_OUTLINE;
        at(palette_, ColorRole::OutlineVariant)   = CLR_OUTLINE_VARIANT;

        at(palette_, ColorRole::Error)     = CLR_ERROR;
        at(palette_, ColorRole::OnError)   = CLR_ON_ERROR;
        at(palette_, ColorRole::Warning)   = CLR_WARNING;
        at(palette_, ColorRole::OnWarning) = CLR_ON_WARNING;
        at(palette_, ColorRole::Success)   = CLR_SUCCESS;
        at(palette_, ColorRole::OnSuccess) = CLR_ON_SUCCESS;
        at(palette_, ColorRole::Info)      = CLR_INFO;

        at(palette_, ColorRole::FocusRing)  = CLR_PRIMARY;
        at(palette_, ColorRole::Disabled)   = CLR_PRIMARY_DIS_BG;
        at(palette_, ColorRole::DialogMask) = CLR_DIALOG_BG_MASK;

        at(palette_, ColorRole::ButtonBlue)             = CLR_WIDGET_BUTTON_BLUE;
        at(palette_, ColorRole::ButtonBluePressed)      = CLR_WIDGET_BUTTON_BLUE_DIS;
        at(palette_, ColorRole::ButtonDisabled)         = CLR_SURFACE_DIS_CONTAINER_1;
        at(palette_, ColorRole::ButtonGrayPressed)      = CLR_WIDGET_BUTTON_GRAY_DIS;
        at(palette_, ColorRole::ButtonDarkBg)           = CLR_PRIMARY_DARK_BG;
        at(palette_, ColorRole::ButtonDarkPressed)      = CLR_PRIMARY_BLACK;
        at(palette_, ColorRole::ButtonDarkDisabled)     = CLR_PRIMARY_BLACK_HOVER;
        at(palette_, ColorRole::ButtonLightBlueBg)      = 0xF7F9FF;
        at(palette_, ColorRole::ButtonLightBluePressed) = 0xEAEFFF;
        at(palette_, ColorRole::ButtonRedBg)            = CLR_WARNING_DARK;
        at(palette_, ColorRole::ButtonRedPressed)       = 0x661616;
        at(palette_, ColorRole::ButtonDarkGrayBg)       = CLR_SECONDARY_CONTAINER;
    } else
    {
        at(palette_, ColorRole::Primary)            = 0x3760EA;
        at(palette_, ColorRole::OnPrimary)          = 0xFFFFFF;
        at(palette_, ColorRole::PrimaryContainer)   = 0xD5DDFB;
        at(palette_, ColorRole::OnPrimaryContainer) = 0x11225D;

        at(palette_, ColorRole::Secondary)            = 0x5A6070;
        at(palette_, ColorRole::OnSecondary)          = 0xFFFFFF;
        at(palette_, ColorRole::SecondaryContainer)   = 0xE6EAF8;
        at(palette_, ColorRole::OnSecondaryContainer) = 0x191D2A;

        at(palette_, ColorRole::Surface)                 = 0xF7F9FF;
        at(palette_, ColorRole::SurfaceDim)              = 0xE6EAF8;
        at(palette_, ColorRole::SurfaceCanvas)           = 0xE7EBF2;
        at(palette_, ColorRole::SurfaceContainer)        = 0xEEF1F8;
        at(palette_, ColorRole::SurfaceContainerHigh)    = 0xE2E6EF;
        at(palette_, ColorRole::SurfaceContainerHighest) = 0xD8DDE9;

        at(palette_, ColorRole::OnSurface)        = 0x161616;
        at(palette_, ColorRole::OnSurfaceVariant) = 0x4A5060;
        at(palette_, ColorRole::Outline)          = 0xC0C1D0;
        at(palette_, ColorRole::OutlineVariant)   = 0xD5D8E2;

        at(palette_, ColorRole::Error)     = 0xD32F2F;
        at(palette_, ColorRole::OnError)   = 0xFFFFFF;
        at(palette_, ColorRole::Warning)   = 0xED6C02;
        at(palette_, ColorRole::OnWarning) = 0xFFFFFF;
        at(palette_, ColorRole::Success)   = 0x2E7D32;
        at(palette_, ColorRole::OnSuccess) = 0xFFFFFF;
        at(palette_, ColorRole::Info)      = 0x0288D1;

        at(palette_, ColorRole::FocusRing)  = 0x3760EA;
        at(palette_, ColorRole::Disabled)   = 0xB0B7C3;
        at(palette_, ColorRole::DialogMask) = 0x1A1D22;

        at(palette_, ColorRole::ButtonBlue)             = 0x3760EA;
        at(palette_, ColorRole::ButtonBluePressed)      = 0x2448B8;
        at(palette_, ColorRole::ButtonDisabled)         = 0xCFD4DF;
        at(palette_, ColorRole::ButtonGrayPressed)      = 0xB8BFCC;
        at(palette_, ColorRole::ButtonDarkBg)           = 0xE2E6EF;
        at(palette_, ColorRole::ButtonDarkPressed)      = 0xC9CFDA;
        at(palette_, ColorRole::ButtonDarkDisabled)     = 0xA6ADB8;
        at(palette_, ColorRole::ButtonLightBlueBg)      = 0xF7F9FF;
        at(palette_, ColorRole::ButtonLightBluePressed) = 0xEAEFFF;
        at(palette_, ColorRole::ButtonRedBg)            = 0xD32F2F;
        at(palette_, ColorRole::ButtonRedPressed)       = 0x9A1F1F;
        at(palette_, ColorRole::ButtonDarkGrayBg)       = 0xD8DDE9;
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
    light_blue.label_color    = 0x3760EA;
    light_blue.label_disabled = hex(ColorRole::Disabled);

    auto &black_border          = buttons_[static_cast<size_t>(ButtonStyle::BlackBorder)];
    black_border.transparent    = true;
    black_border.normal_bg      = 0;
    black_border.pressed_bg     = 0;
    black_border.disabled_bg    = 0;
    black_border.border_color   = hex(ColorRole::Outline);
    black_border.label_color    = mode_ == ThemeMode::Dark ? 0x161616 : hex(ColorRole::OnSurface);
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
    card_style_->radius(16);
    card_style_->border(2, hex(ColorRole::OutlineVariant));

    dialog_style_ = std::make_unique<Style>();
    dialog_style_->bgColor(hex(ColorRole::SurfaceContainer));
    dialog_style_->radius(0);

    button_base_style_ = std::make_unique<Style>();
    button_base_style_->radius(8);
}

} // namespace lvglpp::core
