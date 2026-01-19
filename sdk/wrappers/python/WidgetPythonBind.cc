#include "WidgetPythonBind.h"
#include "PythonUtils.h"
#include <iostream>

namespace lvglpp::python {

template<typename T>
void WidgetPythonBind::bindCommonMethods(py::class_<T>& cls) {
    // Position and size
    cls.def("set_pos", &T::setPos, "Set widget position");
    cls.def("get_x", &T::getX, "Get X position");
    cls.def("get_y", &T::getY, "Get Y position");
    cls.def("set_size", &T::setSize, "Set widget size");
    cls.def("set_width", &T::setWidth, "Set widget width");
    cls.def("get_width", &T::getWidth, "Get widget width");
    cls.def("get_height", &T::getHeight, "Get widget height");
    
    // Style
    cls.def("set_bg_color", &T::setBgColor, "Set background color");
    cls.def("set_disable_bg_color", &T::setDisableBgColor, "Set disable background color");
    cls.def("set_opacity", &T::setOpacity, "Set opacity");
    cls.def("set_radius", &T::setRadius, "Set radius");
    cls.def("get_radius", &T::getRadius, "Get radius");
    cls.def("set_clip", &T::setClip, "Set clip");
    cls.def("set_border", &T::setBorder, "Set border");
    
    // Visibility and enable
    cls.def("set_visible", &T::setVisible, "Set visible");
    cls.def("get_visible", &T::getVisible, "Get visible");
    cls.def("set_enable", &T::setEnable, "Set enable");
    cls.def("get_enable", &T::getEnable, "Get enable");
    
    // Parent
    cls.def("set_parent", &T::setParent, "Set parent");
    cls.def("get_parent", &T::getParent, "Get parent");
}

void WidgetPythonBind::bindBaseItem(py::module& m) {
    auto baseitem_cls = py::class_<widgets::BaseItem, std::shared_ptr<widgets::BaseItem>>(m, "BaseItem");
    
    baseitem_cls.def(py::init<widgets::BaseItem*>(), "Create a new BaseItem", py::arg("parent") = nullptr);
    
    bindCommonMethods(baseitem_cls);
    
    // Events
    baseitem_cls.def("set_on_clicked", [](widgets::BaseItem& self, py::function func) {
        self.setOnClickedListener([func]() {
            try {
                func();
            } catch (const py::error_already_set& e) {
                PythonUtils::printError(std::string("Event handler error: ") + e.what());
            }
        });
    }, "Set clicked event handler");
    
    baseitem_cls.def("set_on_pressed", [](widgets::BaseItem& self, py::function func) {
        self.setOnPressedListener([func]() {
            try {
                func();
            } catch (const py::error_already_set& e) {
                PythonUtils::printError(std::string("Event handler error: ") + e.what());
            }
        });
    }, "Set pressed event handler");
    
    baseitem_cls.def("set_on_released", [](widgets::BaseItem& self, py::function func) {
        self.setOnReleasedListener([func]() {
            try {
                func();
            } catch (const py::error_already_set& e) {
                PythonUtils::printError(std::string("Event handler error: ") + e.what());
            }
        });
    }, "Set released event handler");
}

void WidgetPythonBind::bindLvText(py::module& m) {
    auto lvtext_cls = py::class_<widgets::LvText, std::shared_ptr<widgets::LvText>>(m, "LvText");
    
    lvtext_cls.def(py::init<const std::string&, uint32_t, widgets::BaseItem*>(), 
                  "Create a new LvText", 
                  py::arg("text"), 
                  py::arg("color"), 
                  py::arg("parent") = nullptr);
    
    bindCommonMethods(lvtext_cls);
    
    // Text specific methods
    lvtext_cls.def("set_text", &widgets::LvText::setText, "Set text content");
    lvtext_cls.def("get_text", &widgets::LvText::getText, "Get text content");
    lvtext_cls.def("set_text_color", &widgets::LvText::setTextClr, "Set text color");
    lvtext_cls.def("set_text_align", &widgets::LvText::setTextAlign, "Set text alignment");
    lvtext_cls.def("set_recolor_enable", &widgets::LvText::setReColorEnable, "Enable/disable recoloring");
    lvtext_cls.def("set_max_len", &widgets::LvText::setMaxLen, "Set maximum text length");
    lvtext_cls.def("get_display_text_width", &widgets::LvText::getDisplayTextWidth, "Get display text width");
    lvtext_cls.def("set_long_mode", &widgets::LvText::setLongMode, "Set long text mode");
    
    // Enum constants
    py::enum_<widgets::LvText::TextStyle>(lvtext_cls, "TextStyle")
        .value("FontSize20", widgets::LvText::TextStyle::FontSize20)
        .value("FontSize24", widgets::LvText::TextStyle::FontSize24)
        .value("FontSize28", widgets::LvText::TextStyle::FontSize28)
        .value("FontSize32", widgets::LvText::TextStyle::FontSize32)
        .value("FontSize36", widgets::LvText::TextStyle::FontSize36)
        .value("FontSize20Bold", widgets::LvText::TextStyle::FontSize20Bold)
        .value("FontSize24Bold", widgets::LvText::TextStyle::FontSize24Bold)
        .value("FontSize28Bold", widgets::LvText::TextStyle::FontSize28Bold)
        .value("FontSize32Bold", widgets::LvText::TextStyle::FontSize32Bold)
        .value("FontSize36Bold", widgets::LvText::TextStyle::FontSize36Bold)
        .value("FontSize64Bold", widgets::LvText::TextStyle::FontSize64Bold)
        .export_values();
    
    py::enum_<widgets::LvText::FontType>(lvtext_cls, "FontType")
        .value("Auto", widgets::LvText::FontType::Auto)
        .value("DMSans", widgets::LvText::FontType::DMSans)
        .value("SansSC", widgets::LvText::FontType::SansSC)
        .value("Oswald", widgets::LvText::FontType::Oswald)
        .export_values();
}

void WidgetPythonBind::bindImage(py::module& m) {
    auto image_cls = py::class_<widgets::Image, std::shared_ptr<widgets::Image>>(m, "Image");
    
    image_cls.def(py::init<const std::string&, widgets::BaseItem*>(), 
                 "Create a new Image", 
                 py::arg("src_path"), 
                 py::arg("parent") = nullptr);
    
    bindCommonMethods(image_cls);
    
    // Image specific methods
    image_cls.def("set_image_src_path", &widgets::Image::setImageSrcPath, "Set image source path");
    image_cls.def("get_image_src_path", &widgets::Image::getImageSrcPath, "Get image source path");
}

void WidgetPythonBind::bindPushButton(py::module& m) {
    auto button_cls = py::class_<widgets::PushButton, std::shared_ptr<widgets::PushButton>>(m, "PushButton");
    
    button_cls.def(py::init<int, int, widgets::PushButton::ColorStyle, const std::string&, widgets::BaseItem*>(), 
                  "Create a new PushButton", 
                  py::arg("width"), 
                  py::arg("height"), 
                  py::arg("color_style"), 
                  py::arg("text"), 
                  py::arg("parent") = nullptr);
    
    bindCommonMethods(button_cls);
    
    // Button specific methods
    button_cls.def("set_image_pos", py::overload_cast<int, int>(&widgets::PushButton::setImagePos), "Set image position");
    button_cls.def("set_text_pos", py::overload_cast<int, int>(&widgets::PushButton::setTextPos), "Set text position");
    button_cls.def("set_text_alignment", &widgets::PushButton::setTextAlignment, "Set text alignment");
    button_cls.def("set_text_msg", &widgets::PushButton::setTextMsg, "Set text message");
    button_cls.def("change_image_url", &widgets::PushButton::changeImageUrl, "Change image URL");
    button_cls.def("set_color_type", &widgets::PushButton::setColorType, "Set color type");
    button_cls.def("set_repeat_ms", &widgets::PushButton::setRepeatMs, "Set repeat milliseconds");
    
    // Events
    button_cls.def("set_on_clicked", [](widgets::PushButton& self, py::function func) {
        self.setOnClickedListener([func]() {
            try {
                func();
            } catch (const py::error_already_set& e) {
                PythonUtils::printError(std::string("Event handler error: ") + e.what());
            }
        });
    }, "Set clicked event handler");
    
    // Enum constants
    py::enum_<widgets::PushButton::ColorStyle>(button_cls, "ColorStyle")
        .value("Blue", widgets::PushButton::ColorStyle::Blue)
        .value("Dark", widgets::PushButton::ColorStyle::Dark)
        .value("Gray", widgets::PushButton::ColorStyle::Gray)
        .value("LightBlue", widgets::PushButton::ColorStyle::LightBlue)
        .value("BlackBroder", widgets::PushButton::ColorStyle::BlackBroder)
        .value("Red", widgets::PushButton::ColorStyle::Red)
        .value("DarkGray", widgets::PushButton::ColorStyle::DarkGray)
        .export_values();
}

void WidgetPythonBind::bindAll(py::module& m) {
    bindBaseItem(m);
    bindLvText(m);
    bindImage(m);
    bindPushButton(m);
}

} // namespace lvglpp::python
