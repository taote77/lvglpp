#include "WidgetLuaBind.h"
#include "LuaUtils.h"
#include <iostream>

namespace lvglpp::lua {

template<typename T>
void WidgetLuaBind::bindCommonMethods(sol::usertype<T>& usertype) {
    usertype["set_pos"] = &T::setPos;
    usertype["get_x"] = &T::getX;
    usertype["get_y"] = &T::getY;
    usertype["set_alignment"] = &T::setAligment;
    usertype["set_alignment_to"] = &T::setAligmentTo;
    usertype["set_bg_color"] = &T::setBgColor;
    usertype["set_disable_bg_color"] = &T::setDisableBgColor;
    usertype["set_opacity"] = &T::setOpacity;
    usertype["set_size"] = &T::setSize;
    usertype["set_width"] = &T::setWidth;
    usertype["get_width"] = &T::getWidth;
    usertype["get_height"] = &T::getHeight;
    usertype["set_visible"] = &T::setVisible;
    usertype["get_visible"] = &T::getVisible;
    usertype["set_enable"] = &T::setEnable;
    usertype["get_enable"] = &T::getEnable;
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

void WidgetLuaBind::bindBaseItem(sol::state& lua) {
    auto baseitem_type = lua.new_usertype<widgets::BaseItem>(
        "BaseItem",
        sol::constructors<widgets::BaseItem(widgets::BaseItem*)>()
    );
    
    bindCommonMethods(baseitem_type);
    
    baseitem_type["set_on_clicked"] = [](widgets::BaseItem& self, sol::function func) {
        self.setOnClickedListener([func]() {
            func();
        });
    };
    
    baseitem_type["set_on_pressed"] = [](widgets::BaseItem& self, sol::function func) {
        self.setOnPressedListener([func]() {
            func();
        });
    };
    
    baseitem_type["set_on_released"] = [](widgets::BaseItem& self, sol::function func) {
        self.setOnReleasedListener([func]() {
            func();
        });
    };
}

void WidgetLuaBind::bindLvText(sol::state& lua) {
    auto lvtext_type = lua.new_usertype<widgets::LvText>(
        "LvText",
        sol::constructors<
            widgets::LvText(const std::string&, uint32_t, widgets::BaseItem*, widgets::LvText::TextStyle, widgets::LvText::FontType)
        >()
    );
    
    bindCommonMethods(lvtext_type);
    
    lvtext_type["set_text"] = &widgets::LvText::setText;
    lvtext_type["get_text"] = &widgets::LvText::getText;
    lvtext_type["set_text_color"] = &widgets::LvText::setTextClr;
    lvtext_type["set_text_align"] = &widgets::LvText::setTextAlign;
    lvtext_type["set_recolor_enable"] = &widgets::LvText::setReColorEnable;
    lvtext_type["set_max_len"] = &widgets::LvText::setMaxLen;
    lvtext_type["get_display_text_width"] = &widgets::LvText::getDisplayTextWidth;
    lvtext_type["set_long_mode"] = &widgets::LvText::setLongMode;
    
    lvtext_type["TextStyle"] = lua.create_table();
    lvtext_type["TextStyle"]["FontSize20"] = widgets::LvText::TextStyle::FontSize20;
    lvtext_type["TextStyle"]["FontSize24"] = widgets::LvText::TextStyle::FontSize24;
    lvtext_type["TextStyle"]["FontSize28"] = widgets::LvText::TextStyle::FontSize28;
    lvtext_type["TextStyle"]["FontSize32"] = widgets::LvText::TextStyle::FontSize32;
    lvtext_type["TextStyle"]["FontSize36"] = widgets::LvText::TextStyle::FontSize36;
    lvtext_type["TextStyle"]["FontSize20Bold"] = widgets::LvText::TextStyle::FontSize20Bold;
    lvtext_type["TextStyle"]["FontSize24Bold"] = widgets::LvText::TextStyle::FontSize24Bold;
    lvtext_type["TextStyle"]["FontSize28Bold"] = widgets::LvText::TextStyle::FontSize28Bold;
    lvtext_type["TextStyle"]["FontSize32Bold"] = widgets::LvText::TextStyle::FontSize32Bold;
    lvtext_type["TextStyle"]["FontSize36Bold"] = widgets::LvText::TextStyle::FontSize36Bold;
    lvtext_type["TextStyle"]["FontSize64Bold"] = widgets::LvText::TextStyle::FontSize64Bold;
    
    lvtext_type["FontType"] = lua.create_table();
    lvtext_type["FontType"]["Auto"] = widgets::LvText::FontType::Auto;
    lvtext_type["FontType"]["DMSans"] = widgets::LvText::FontType::DMSans;
    lvtext_type["FontType"]["SansSC"] = widgets::LvText::FontType::SansSC;
    lvtext_type["FontType"]["Oswald"] = widgets::LvText::FontType::Oswald;
    
    lvtext_type["LongMode"] = lua.create_table();
    lvtext_type["LongMode"]["None"] = widgets::LvText::LongMode::None;
    lvtext_type["LongMode"]["AutoEllipsis"] = widgets::LvText::LongMode::AutoEllipsis;
    lvtext_type["LongMode"]["MiddleEllipsis"] = widgets::LvText::LongMode::MiddleEllipsis;
}

void WidgetLuaBind::bindImage(sol::state& lua) {
    auto image_type = lua.new_usertype<widgets::Image>(
        "Image",
        sol::constructors<widgets::Image(const std::string&, widgets::BaseItem*)>()
    );
    
    bindCommonMethods(image_type);
    
    image_type["set_image_src_path"] = &widgets::Image::setImageSrcPath;
    image_type["get_image_src_path"] = &widgets::Image::getImageSrcPath;
}

void WidgetLuaBind::bindPushButton(sol::state& lua) {
    auto button_type = lua.new_usertype<widgets::PushButton>(
        "PushButton",
        sol::constructors<
            widgets::PushButton(int, int, widgets::PushButton::ColorStyle, const std::string&, widgets::BaseItem*),
            widgets::PushButton(int, int, widgets::PushButton::ColorStyle, const std::string&, const std::string&, widgets::BaseItem*)
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
    button_type["set_text_msg"] = &widgets::PushButton::setTextMsg;
    button_type["change_image_url"] = &widgets::PushButton::changeImageUrl;
    button_type["set_color_type"] = &widgets::PushButton::setColorType;
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
    bindBaseItem(lua);
    bindLvText(lua);
    bindImage(lua);
    bindPushButton(lua);
}

}
