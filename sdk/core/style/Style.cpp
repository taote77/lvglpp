#include "Style.h"

#include "core/log/log.h"

namespace lvglpp::core {

Style::Style() : state_(std::make_shared<State>())
{
    lv_style_init(&state_->style);
    state_->initialized = true;
}

Style Style::clone() const
{
    Style copy;
    lv_style_copy(&copy.state_->style, &state_->style);
    copy.state_->initialized = true;
    return copy;
}

bool Style::isAttached() const
{
    return state_ && state_->attached;
}

lv_style_t *Style::raw()
{
    return &state_->style;
}

const lv_style_t *Style::raw() const
{
    return &state_->style;
}

bool Style::ensureMutable(const char *setter) const
{
    if (state_->attached)
    {
        LOG_ERROR() << "Style::" << setter << " ignored: style is already attached; use clone() for a variant";
        return false;
    }
    return true;
}

void Style::applyTo(lv_obj_t *obj, lv_style_selector_t selector) const
{
    if (obj == nullptr)
    {
        return;
    }
    lv_obj_add_style(obj, &state_->style, selector);
    state_->attached = true;
}

// ---------------------------------------------------------------- background

Style &Style::bgColor(uint32_t hex)
{
    return bgColor(lv_color_hex(hex));
}

Style &Style::bgColor(lv_color_t color)
{
    if (ensureMutable("bgColor"))
    {
        lv_style_set_bg_color(&state_->style, color);
    }
    return *this;
}

Style &Style::bgOpacity(lv_opa_t opacity)
{
    if (ensureMutable("bgOpacity"))
    {
        lv_style_set_bg_opa(&state_->style, opacity);
    }
    return *this;
}

// ---------------------------------------------------------------- shape

Style &Style::radius(lv_coord_t radius)
{
    if (ensureMutable("radius"))
    {
        lv_style_set_radius(&state_->style, radius);
    }
    return *this;
}

Style &Style::borderWidth(lv_coord_t width)
{
    if (ensureMutable("borderWidth"))
    {
        lv_style_set_border_width(&state_->style, width);
    }
    return *this;
}

Style &Style::borderColor(uint32_t hex)
{
    if (ensureMutable("borderColor"))
    {
        lv_style_set_border_color(&state_->style, lv_color_hex(hex));
    }
    return *this;
}

Style &Style::border(lv_coord_t width, uint32_t hex)
{
    if (ensureMutable("border"))
    {
        lv_style_set_border_width(&state_->style, width);
        lv_style_set_border_color(&state_->style, lv_color_hex(hex));
    }
    return *this;
}

Style &Style::outline(lv_coord_t width, uint32_t hex, lv_coord_t pad)
{
    if (ensureMutable("outline"))
    {
        lv_style_set_outline_width(&state_->style, width);
        lv_style_set_outline_color(&state_->style, lv_color_hex(hex));
        lv_style_set_outline_pad(&state_->style, pad);
        lv_style_set_outline_opa(&state_->style, LV_OPA_COVER);
    }
    return *this;
}

// ---------------------------------------------------------------- text

Style &Style::textColor(uint32_t hex)
{
    return textColor(lv_color_hex(hex));
}

Style &Style::textColor(lv_color_t color)
{
    if (ensureMutable("textColor"))
    {
        lv_style_set_text_color(&state_->style, color);
    }
    return *this;
}

Style &Style::textFont(const lv_font_t *font)
{
    if (ensureMutable("textFont"))
    {
        lv_style_set_text_font(&state_->style, font);
    }
    return *this;
}

Style &Style::textAlign(lv_text_align_t align)
{
    if (ensureMutable("textAlign"))
    {
        lv_style_set_text_align(&state_->style, align);
    }
    return *this;
}

Style &Style::textLineSpace(lv_coord_t space)
{
    if (ensureMutable("textLineSpace"))
    {
        lv_style_set_text_line_space(&state_->style, space);
    }
    return *this;
}

// ---------------------------------------------------------------- padding

Style &Style::padAll(lv_coord_t value)
{
    if (ensureMutable("padAll"))
    {
        lv_style_set_pad_all(&state_->style, value);
    }
    return *this;
}

Style &Style::padHor(lv_coord_t value)
{
    if (ensureMutable("padHor"))
    {
        lv_style_set_pad_hor(&state_->style, value);
    }
    return *this;
}

Style &Style::padVer(lv_coord_t value)
{
    if (ensureMutable("padVer"))
    {
        lv_style_set_pad_ver(&state_->style, value);
    }
    return *this;
}

Style &Style::padTop(lv_coord_t value)
{
    if (ensureMutable("padTop"))
    {
        lv_style_set_pad_top(&state_->style, value);
    }
    return *this;
}

Style &Style::padBottom(lv_coord_t value)
{
    if (ensureMutable("padBottom"))
    {
        lv_style_set_pad_bottom(&state_->style, value);
    }
    return *this;
}

Style &Style::padLeft(lv_coord_t value)
{
    if (ensureMutable("padLeft"))
    {
        lv_style_set_pad_left(&state_->style, value);
    }
    return *this;
}

Style &Style::padRight(lv_coord_t value)
{
    if (ensureMutable("padRight"))
    {
        lv_style_set_pad_right(&state_->style, value);
    }
    return *this;
}

Style &Style::padRow(lv_coord_t value)
{
    if (ensureMutable("padRow"))
    {
        lv_style_set_pad_row(&state_->style, value);
    }
    return *this;
}

Style &Style::padColumn(lv_coord_t value)
{
    if (ensureMutable("padColumn"))
    {
        lv_style_set_pad_column(&state_->style, value);
    }
    return *this;
}

// ---------------------------------------------------------------- margin

Style &Style::marginAll(lv_coord_t value)
{
    if (ensureMutable("marginAll"))
    {
        lv_style_set_margin_top(&state_->style, value);
        lv_style_set_margin_bottom(&state_->style, value);
        lv_style_set_margin_left(&state_->style, value);
        lv_style_set_margin_right(&state_->style, value);
    }
    return *this;
}

Style &Style::marginHor(lv_coord_t value)
{
    if (ensureMutable("marginHor"))
    {
        lv_style_set_margin_left(&state_->style, value);
        lv_style_set_margin_right(&state_->style, value);
    }
    return *this;
}

Style &Style::marginVer(lv_coord_t value)
{
    if (ensureMutable("marginVer"))
    {
        lv_style_set_margin_top(&state_->style, value);
        lv_style_set_margin_bottom(&state_->style, value);
    }
    return *this;
}

Style &Style::marginTop(lv_coord_t value)
{
    if (ensureMutable("marginTop"))
    {
        lv_style_set_margin_top(&state_->style, value);
    }
    return *this;
}

Style &Style::marginBottom(lv_coord_t value)
{
    if (ensureMutable("marginBottom"))
    {
        lv_style_set_margin_bottom(&state_->style, value);
    }
    return *this;
}

Style &Style::marginLeft(lv_coord_t value)
{
    if (ensureMutable("marginLeft"))
    {
        lv_style_set_margin_left(&state_->style, value);
    }
    return *this;
}

Style &Style::marginRight(lv_coord_t value)
{
    if (ensureMutable("marginRight"))
    {
        lv_style_set_margin_right(&state_->style, value);
    }
    return *this;
}

// ---------------------------------------------------------------- size

Style &Style::width(lv_coord_t value)
{
    if (ensureMutable("width"))
    {
        lv_style_set_width(&state_->style, value);
    }
    return *this;
}

Style &Style::height(lv_coord_t value)
{
    if (ensureMutable("height"))
    {
        lv_style_set_height(&state_->style, value);
    }
    return *this;
}

Style &Style::minWidth(lv_coord_t value)
{
    if (ensureMutable("minWidth"))
    {
        lv_style_set_min_width(&state_->style, value);
    }
    return *this;
}

Style &Style::maxWidth(lv_coord_t value)
{
    if (ensureMutable("maxWidth"))
    {
        lv_style_set_max_width(&state_->style, value);
    }
    return *this;
}

Style &Style::minHeight(lv_coord_t value)
{
    if (ensureMutable("minHeight"))
    {
        lv_style_set_min_height(&state_->style, value);
    }
    return *this;
}

Style &Style::maxHeight(lv_coord_t value)
{
    if (ensureMutable("maxHeight"))
    {
        lv_style_set_max_height(&state_->style, value);
    }
    return *this;
}

// ---------------------------------------------------------------- other

Style &Style::shadow(lv_coord_t width, uint32_t color, lv_opa_t opacity, lv_coord_t offsetX, lv_coord_t offsetY)
{
    if (ensureMutable("shadow"))
    {
        lv_style_set_shadow_width(&state_->style, width);
        lv_style_set_shadow_color(&state_->style, lv_color_hex(color));
        lv_style_set_shadow_opa(&state_->style, opacity);
        lv_style_set_shadow_offset_x(&state_->style, offsetX);
        lv_style_set_shadow_offset_y(&state_->style, offsetY);
    }
    return *this;
}

Style &Style::opacity(lv_opa_t value)
{
    if (ensureMutable("opacity"))
    {
        lv_style_set_opa(&state_->style, value);
    }
    return *this;
}

} // namespace lvglpp::core
