#ifndef LVGLPP_STYLE_H
#define LVGLPP_STYLE_H

#include "lvgl.h"

#include <memory>

namespace lvglpp::core {

/**
 * Shared, RAII wrapper around lv_style_t.
 *
 * Copies of a Style share the same underlying lv_style_t. A style becomes
 * immutable on first application; create variants with clone().
 *
 * When applied through Widget::applyStyle() the style is kept alive by the
 * object registry until the underlying lv_obj is deleted, so a stack-allocated
 * Style is safe to use.
 */
class Style
{
public:
    Style();

    Style(const Style &)            = default;
    Style &operator=(const Style &) = default;

    ~Style() = default;

    /// Deep copy that can be modified independently.
    Style clone() const;

    bool isAttached() const;

    lv_style_t *raw();

    const lv_style_t *raw() const;

    // ---- background ----
    Style &bgColor(uint32_t hex);
    Style &bgColor(lv_color_t color);
    Style &bgOpacity(lv_opa_t opacity);

    // ---- shape / border ----
    Style &radius(lv_coord_t radius);
    Style &borderWidth(lv_coord_t width);
    Style &borderColor(uint32_t hex);
    Style &border(lv_coord_t width, uint32_t hex);
    Style &outline(lv_coord_t width, uint32_t hex, lv_coord_t pad = 0);

    // ---- text ----
    Style &textColor(uint32_t hex);
    Style &textColor(lv_color_t color);
    Style &textFont(const lv_font_t *font);
    Style &textAlign(lv_text_align_t align);
    Style &textLineSpace(lv_coord_t space);

    // ---- spacing ----
    Style &padAll(lv_coord_t value);
    Style &padHor(lv_coord_t value);
    Style &padVer(lv_coord_t value);
    Style &padTop(lv_coord_t value);
    Style &padBottom(lv_coord_t value);
    Style &padLeft(lv_coord_t value);
    Style &padRight(lv_coord_t value);
    Style &padRow(lv_coord_t value);
    Style &padColumn(lv_coord_t value);

    Style &marginAll(lv_coord_t value);
    Style &marginHor(lv_coord_t value);
    Style &marginVer(lv_coord_t value);
    Style &marginTop(lv_coord_t value);
    Style &marginBottom(lv_coord_t value);
    Style &marginLeft(lv_coord_t value);
    Style &marginRight(lv_coord_t value);

    // ---- size ----
    Style &width(lv_coord_t value);
    Style &height(lv_coord_t value);
    Style &minWidth(lv_coord_t value);
    Style &maxWidth(lv_coord_t value);
    Style &minHeight(lv_coord_t value);
    Style &maxHeight(lv_coord_t value);

    // ---- other ----
    Style &shadow(lv_coord_t width, uint32_t color, lv_opa_t opacity, lv_coord_t offsetX, lv_coord_t offsetY);
    Style &opacity(lv_opa_t value);

    /// Apply to an object. Prefer Widget::applyStyle() for lifetime safety.
    void applyTo(lv_obj_t *obj, lv_style_selector_t selector = LV_PART_MAIN | LV_STATE_DEFAULT) const;

private:
    struct State {
        lv_style_t style{};
        bool       attached    = false;
        bool       initialized = false;

        ~State()
        {
            if (initialized)
            {
                lv_style_reset(&style);
            }
        }
    };

    bool ensureMutable(const char *setter) const;

    std::shared_ptr<State> state_;
};

} // namespace lvglpp::core

#endif // LVGLPP_STYLE_H
