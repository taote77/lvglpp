#include "WidgetLuaBind.h"
#include "LuaUtils.h"
#include <iostream>

namespace lvglpp::lua {

template<typename T>
void WidgetLuaBind::bindCommonMethods(sol::usertype<T>& usertype) {
    usertype["set_pos"] = &T::setPos;
    usertype["get_x"] = &T::getX;
    usertype["get_y"] = &T::getY;
    usertype["set_alignment"] = &T::setAlignment;
    usertype["set_alignment_to"] = &T::setAlignmentTo;
    usertype["set_bg_color"] = &T::setBgColor;
    usertype["set_disable_bg_color"] = &T::setDisabledBgColor;
    usertype["set_opacity"] = &T::setOpacity;
    usertype["set_size"] = &T::setSize;
    usertype["set_width"] = &T::setWidth;
    usertype["get_width"] = &T::getWidth;
    usertype["get_height"] = &T::getHeight;
    usertype["set_visible"] = &T::setVisible;
    usertype["get_visible"] = &T::isVisible;
    usertype["is_visible"] = &T::isVisible;
    usertype["set_enable"] = &T::setEnabled;
    usertype["get_enable"] = &T::isEnabled;
    usertype["set_enabled"] = &T::setEnabled;
    usertype["is_enabled"] = &T::isEnabled;
    usertype["set_radius"] = &T::setRadius;
    usertype["get_radius"] = &T::getRadius;
    usertype["set_clip"] = &T::setClip;
    usertype["set_border"] = &T::setBorder;
    usertype["set_padding_bottom"] = &T::setPaddingBottom;
    usertype["set_padding_top"] = &T::setPaddingTop;
    usertype["set_padding_hor"] = &T::setPaddingHor;
    usertype["set_parent"] = &T::setParent;
    usertype["get_parent"] = &T::getParent;
}

void WidgetLuaBind::bindWidget(sol::state& lua) {
    auto widget_type = lua.new_usertype<widgets::Widget>(
        "Widget",
        sol::constructors<widgets::Widget(widgets::Widget*)>()
    );

    bindCommonMethods(widget_type);

    widget_type["set_on_clicked"] = [](widgets::Widget& self, sol::function func) {
        self.setOnClickedListener([func]() {
            func();
        });
    };

    widget_type["set_on_pressed"] = [](widgets::Widget& self, sol::function func) {
        self.setOnPressedListener([func]() {
            func();
        });
    };

    widget_type["set_on_released"] = [](widgets::Widget& self, sol::function func) {
        self.setOnReleasedListener([func]() {
            func();
        });
    };
}

void WidgetLuaBind::bindLabel(sol::state& lua) {
    auto label_type = lua.new_usertype<widgets::Label>(
        "Label",
        sol::constructors<
            widgets::Label(const std::string&, uint32_t, widgets::Widget*, widgets::Label::TextStyle, widgets::Label::FontType)
        >()
    );

    bindCommonMethods(label_type);

    label_type["set_text"] = &widgets::Label::setText;
    label_type["get_text"] = &widgets::Label::getText;
    label_type["set_text_color"] = &widgets::Label::setTextColor;
    label_type["set_text_align"] = &widgets::Label::setTextAlign;
    label_type["set_recolor_enable"] = &widgets::Label::setRecolorEnabled;
    label_type["set_max_len"] = &widgets::Label::setMaxLength;
    label_type["get_display_text_width"] = &widgets::Label::getDisplayTextWidth;
    label_type["set_long_mode"] = static_cast<void (widgets::Label::*)(int, widgets::Label::LongMode)>(&widgets::Label::setLongMode);

    label_type["TextStyle"] = lua.create_table();
    label_type["TextStyle"]["FontSize20"] = widgets::Label::TextStyle::FontSize20;
    label_type["TextStyle"]["FontSize24"] = widgets::Label::TextStyle::FontSize24;
    label_type["TextStyle"]["FontSize28"] = widgets::Label::TextStyle::FontSize28;
    label_type["TextStyle"]["FontSize32"] = widgets::Label::TextStyle::FontSize32;
    label_type["TextStyle"]["FontSize36"] = widgets::Label::TextStyle::FontSize36;
    label_type["TextStyle"]["FontSize20Bold"] = widgets::Label::TextStyle::FontSize20Bold;
    label_type["TextStyle"]["FontSize24Bold"] = widgets::Label::TextStyle::FontSize24Bold;
    label_type["TextStyle"]["FontSize28Bold"] = widgets::Label::TextStyle::FontSize28Bold;
    label_type["TextStyle"]["FontSize32Bold"] = widgets::Label::TextStyle::FontSize32Bold;
    label_type["TextStyle"]["FontSize36Bold"] = widgets::Label::TextStyle::FontSize36Bold;
    label_type["TextStyle"]["FontSize64Bold"] = widgets::Label::TextStyle::FontSize64Bold;

    label_type["FontType"] = lua.create_table();
    label_type["FontType"]["Auto"] = widgets::Label::FontType::Auto;
    label_type["FontType"]["DMSans"] = widgets::Label::FontType::DMSans;
    label_type["FontType"]["SansSC"] = widgets::Label::FontType::SansSC;
    label_type["FontType"]["Oswald"] = widgets::Label::FontType::Oswald;

    label_type["LongMode"] = lua.create_table();
    label_type["LongMode"]["None"] = widgets::Label::LongMode::None;
    label_type["LongMode"]["AutoEllipsis"] = widgets::Label::LongMode::AutoEllipsis;
    label_type["LongMode"]["MiddleEllipsis"] = widgets::Label::LongMode::MiddleEllipsis;
}

void WidgetLuaBind::bindImage(sol::state& lua) {
    auto image_type = lua.new_usertype<widgets::Image>(
        "Image",
        sol::constructors<widgets::Image(const std::string&, widgets::Widget*)>()
    );

    bindCommonMethods(image_type);

    image_type["set_image_src_path"] = &widgets::Image::setAsset;
    image_type["set_asset"] = &widgets::Image::setAsset;
    image_type["set_file"] = &widgets::Image::setFile;
    image_type["get_image_src_path"] = &widgets::Image::getImageSrcPath;
}

void WidgetLuaBind::bindPushButton(sol::state& lua) {
    auto button_type = lua.new_usertype<widgets::PushButton>(
        "PushButton",
        sol::constructors<
            widgets::PushButton(int, int, widgets::PushButton::ColorStyle, const std::string&, widgets::Widget*),
            widgets::PushButton(int, int, widgets::PushButton::ColorStyle, const std::string&, const std::string&, widgets::Widget*)
        >()
    );

    bindCommonMethods(button_type);

    button_type["set_on_clicked"] = [](widgets::PushButton& self, sol::function func) {
        self.setOnClickedListener([func]() {
            func();
        });
    };

    button_type["set_on_pressed"] = [](widgets::PushButton& self, sol::function func) {
        self.setOnPressedListener([func]() {
            func();
        });
    };

    button_type["set_on_released"] = [](widgets::PushButton& self, sol::function func) {
        self.setOnReleasedListener([func]() {
            func();
        });
    };

    button_type["set_image_pos"] = sol::overload(
        [](widgets::PushButton& self, int x, int y) { self.setImagePos(x, y); },
        [](widgets::PushButton& self, lv_align_t align, int x, int y) { self.setImagePos(align, x, y); }
    );

    button_type["set_text_pos"] = sol::overload(
        [](widgets::PushButton& self, int x, int y) { self.setTextPos(x, y); },
        [](widgets::PushButton& self, lv_align_t align, int x, int y) { self.setTextPos(align, x, y); }
    );

    button_type["set_text_alignment"] = &widgets::PushButton::setTextAlignment;
    button_type["set_text_msg"] = &widgets::PushButton::setText;
    button_type["change_image_asset"] = &widgets::PushButton::setImageAsset;
    button_type["set_color_type"] = &widgets::PushButton::setColorStyle;
    button_type["set_repeat_ms"] = &widgets::PushButton::setRepeatMs;

    button_type["ColorStyle"] = lua.create_table();
    button_type["ColorStyle"]["Blue"] = widgets::PushButton::ColorStyle::Blue;
    button_type["ColorStyle"]["Dark"] = widgets::PushButton::ColorStyle::Dark;
    button_type["ColorStyle"]["Gray"] = widgets::PushButton::ColorStyle::Gray;
    button_type["ColorStyle"]["LightBlue"] = widgets::PushButton::ColorStyle::LightBlue;
    button_type["ColorStyle"]["BlackBroder"] = widgets::PushButton::ColorStyle::BlackBroder;
    button_type["ColorStyle"]["Red"] = widgets::PushButton::ColorStyle::Red;
    button_type["ColorStyle"]["DarkGray"] = widgets::PushButton::ColorStyle::DarkGray;
}

void WidgetLuaBind::bindAll(sol::state& lua) {
    bindWidget(lua);
    bindLabel(lua);
    bindImage(lua);
    bindPushButton(lua);
}

}
