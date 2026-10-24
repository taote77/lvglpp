#include "TestFramework.h"
#include "config/color_sheet.h"
#include "core/style/Style.h"
#include "core/style/Theme.h"
#include "core/widgets/Widget.h"

#include <memory>
#include <vector>

using namespace lvglpp::core;
using namespace lvglpp::test;
using namespace lvglpp::widgets;

TEST(Style_applies_properties)
{
    Widget item;

    Style style;
    style.bgColor(0x112233).radius(7).padAll(3).border(2, 0x445566);

    item.applyStyle(style);

    CHECK(lv_color_eq(lv_obj_get_style_bg_color(item.lvObj(), LV_PART_MAIN), lv_color_hex(0x112233)));
    CHECK_EQ(lv_obj_get_style_radius(item.lvObj(), LV_PART_MAIN), 7);
    CHECK_EQ(lv_obj_get_style_pad_top(item.lvObj(), LV_PART_MAIN), 3);
    CHECK_EQ(lv_obj_get_style_border_width(item.lvObj(), LV_PART_MAIN), 2);
    CHECK_TRUE(style.isAttached());
    return true;
}

TEST(Style_is_immutable_after_application)
{
    Widget item;

    Style style;
    style.radius(7);
    item.applyStyle(style);

    // Must be ignored: the style is already attached.
    style.radius(21);
    CHECK_EQ(lv_obj_get_style_radius(item.lvObj(), LV_PART_MAIN), 7);

    Style variant = style.clone();
    variant.radius(21);
    CHECK_EQ(lv_obj_get_style_radius(item.lvObj(), LV_PART_MAIN), 7);
    CHECK_FALSE(variant.isAttached());
    return true;
}

TEST(Style_keepalive_survives_wrapper_destruction)
{
    auto parent = std::make_unique<Widget>();

    lv_obj_t *child_obj = nullptr;
    {
        auto  child = std::make_unique<Widget>(parent.get());
        Style style;
        style.bgColor(0x445566).radius(5);
        child->applyStyle(style);
        child_obj = child->lvObj();
    }

    // Wrapper and stack Style are gone; LVGL still references the style.
    CHECK(child_obj != nullptr);
    CHECK(lv_color_eq(lv_obj_get_style_bg_color(child_obj, LV_PART_MAIN), lv_color_hex(0x445566)));
    CHECK_EQ(lv_obj_get_style_radius(child_obj, LV_PART_MAIN), 5);
    return true;
}

TEST(Style_can_be_removed)
{
    Widget item;

    Style style;
    style.radius(9);
    item.applyStyle(style);
    CHECK_EQ(lv_obj_get_style_radius(item.lvObj(), LV_PART_MAIN), 9);

    item.removeStyle(style);
    CHECK_EQ(lv_obj_get_style_radius(item.lvObj(), LV_PART_MAIN), 0);
    return true;
}

TEST(Theme_mode_switches_palette)
{
    Theme &theme = Theme::instance();

    theme.setMode(ThemeMode::Dark);
    CHECK_EQ(theme.hex(ColorRole::Primary), 0x4986FF);
    CHECK_EQ(theme.hex(ColorRole::Surface), 0x12151E);

    theme.setMode(ThemeMode::Light);
    CHECK_EQ(theme.hex(ColorRole::Primary), 0x3760EA);
    CHECK_EQ(theme.hex(ColorRole::Surface), 0xF7F9FF);

    theme.setMode(ThemeMode::Dark);
    CHECK_EQ(theme.hex(ColorRole::Primary), 0x4986FF);
    return true;
}

TEST(Theme_light_tokens_cover_semantic_roles)
{
    Theme &theme = Theme::instance();

    theme.setMode(ThemeMode::Light);
    CHECK_EQ(theme.hex(ColorRole::Warning), 0xED6C02);
    CHECK_EQ(theme.hex(ColorRole::Info), 0x0288D1);
    CHECK_EQ(theme.hex(ColorRole::SurfaceCanvas), 0xE7EBF2);
    CHECK_EQ(theme.hex(ColorRole::ButtonRedPressed), 0x9A1F1F);
    CHECK_EQ(theme.hex(ColorRole::ButtonDarkGrayBg), 0xD8DDE9);
    // Disabled label and disabled button fill are distinct roles.
    CHECK_EQ(theme.hex(ColorRole::Disabled), 0xB0B7C3);
    CHECK_EQ(theme.hex(ColorRole::ButtonDisabled), 0xCFD4DF);

    theme.setMode(ThemeMode::Dark);
    CHECK_EQ(theme.hex(ColorRole::Warning), 0xF5A34A);
    CHECK_EQ(theme.hex(ColorRole::Info), 0x34BAEF);
    CHECK_EQ(theme.hex(ColorRole::SurfaceCanvas), 0x444E5B);
    CHECK_EQ(theme.hex(ColorRole::ButtonRedPressed), 0x661616);
    CHECK_EQ(theme.hex(ColorRole::ButtonDarkGrayBg), 0x485479);
    return true;
}

TEST(DesignToken_resolves_against_active_theme)
{
    Theme &theme = Theme::instance();

    theme.setMode(ThemeMode::Dark);
    CHECK_EQ(static_cast<uint32_t>(form::design::CLR_PRIMARY), 0x4986FF);
    CHECK_EQ(static_cast<uint32_t>(form::design::CLR_ON_SURFACE), 0xFFFFFF);

    theme.setMode(ThemeMode::Light);
    CHECK_EQ(static_cast<uint32_t>(form::design::CLR_PRIMARY), 0x3760EA);
    CHECK_EQ(static_cast<uint32_t>(form::design::CLR_ON_SURFACE), 0x161616);
    // Explicit values are available for non-themed uses.
    CHECK_EQ(form::design::CLR_ON_SURFACE.darkValue(), 0xFFFFFF);
    CHECK_EQ(form::design::CLR_ON_SURFACE.lightValue(), 0x161616);

    theme.setMode(ThemeMode::Dark);
    CHECK_EQ(static_cast<uint32_t>(form::design::CLR_PRIMARY), 0x4986FF);
    return true;
}

TEST(DesignToken_new_widgets_use_switched_theme)
{
    Theme &theme = Theme::instance();

    theme.setMode(ThemeMode::Dark);
    Widget dark_widget;
    dark_widget.setBgColor(form::design::CLR_SURFACE);
    CHECK(lv_color_eq(lv_obj_get_style_bg_color(dark_widget.lvObj(), LV_PART_MAIN), lv_color_hex(0x12151E)));

    theme.setMode(ThemeMode::Light);
    Widget light_widget;
    light_widget.setBgColor(form::design::CLR_SURFACE);
    CHECK(lv_color_eq(lv_obj_get_style_bg_color(light_widget.lvObj(), LV_PART_MAIN), lv_color_hex(0xF7F9FF)));

    // Existing widgets keep the colour they were created with.
    CHECK(lv_color_eq(lv_obj_get_style_bg_color(dark_widget.lvObj(), LV_PART_MAIN), lv_color_hex(0x12151E)));

    theme.setMode(ThemeMode::Dark);
    return true;
}

TEST(Theme_focus_style_applies_outline)
{
    Widget item;
    item.applyStyle(Theme::instance().focusStyle(), LV_PART_MAIN | LV_STATE_FOCUS_KEY);
    item.addState(LV_STATE_FOCUS_KEY);

    CHECK_EQ(lv_obj_get_style_outline_width(item.lvObj(), LV_PART_MAIN), 2);
    return true;
}

TEST(Theme_button_palettes_are_distinct)
{
    Theme               &theme = Theme::instance();
    const ButtonPalette &blue  = theme.buttonPalette(ButtonStyle::Blue);
    const ButtonPalette &red   = theme.buttonPalette(ButtonStyle::Red);

    CHECK(blue.normal_bg != red.normal_bg);
    CHECK(blue.pressed_bg != blue.normal_bg);
    CHECK_TRUE(theme.buttonPalette(ButtonStyle::BlackBorder).transparent);
    return true;
}

TEST(Grid_layout_places_cells)
{
    Widget container;
    container.setSize(200, 100);
    container.setGridColumns("100px 100px");
    container.setGridRows("50px 50px");

    std::vector<std::unique_ptr<Widget>> children;
    for (uint32_t row = 0; row < 2; ++row)
    {
        for (uint32_t col = 0; col < 2; ++col)
        {
            auto child = std::make_unique<Widget>(&container);
            child->setGridCell(col, row);
            child->setSize(10, 10);
            children.push_back(std::move(child));
        }
    }

    lv_obj_update_layout(container.lvObj());

    const Widget::Rect container_rect = container.bounds();
    const Widget::Rect r00            = children[0]->bounds();
    const Widget::Rect r10            = children[1]->bounds();
    const Widget::Rect r01            = children[2]->bounds();

    CHECK_EQ(r00.x, container_rect.x);
    CHECK(r10.x >= r00.x + 100);
    CHECK(r01.y >= r00.y + 50);
    return true;
}

TEST(Grid_layout_supports_spans)
{
    Widget container;
    container.setSize(200, 100);
    container.setGridColumns("1fr 1fr");
    container.setGridRows("1fr 1fr");

    auto span = std::make_unique<Widget>(&container);
    span->setGridCell(0, 0, 2, 1);
    span->setSize(10, 10);

    lv_obj_update_layout(container.lvObj());

    CHECK_EQ(span->getWidth(), 200);
    return true;
}

TEST(Grid_layout_cell_alignment_preserves_position)
{
    Widget container;
    container.setSize(200, 100);
    container.setGridColumns("100px 100px");
    container.setGridRows("50px 50px");

    auto child = std::make_unique<Widget>(&container);
    child->setGridCell(1, 1);
    child->setSize(10, 10);
    child->setGridCellColumnAlign(LV_GRID_ALIGN_CENTER);

    lv_obj_update_layout(container.lvObj());

    const Widget::Rect container_rect = container.bounds();
    const Widget::Rect child_rect     = child->bounds();

    // Changing the column alignment must not reset the cell to (0, 0).
    CHECK(child_rect.x >= container_rect.x + 100);
    CHECK(child_rect.y >= container_rect.y + 50);
    return true;
}

TEST(Grid_layout_invalid_descriptor_is_rejected)
{
    Widget container;
    container.setSize(200, 100);
    container.setGridColumns("1fr 1fr");
    container.setGridRows("1fr");

    // Invalid input must leave the previous template intact (no crash).
    container.setGridColumns("1fr nope");

    auto child = std::make_unique<Widget>(&container);
    child->setGridCell(1, 0);
    child->setSize(10, 10);

    lv_obj_update_layout(container.lvObj());
    CHECK_EQ(container.childCount(), 1u);
    return true;
}
