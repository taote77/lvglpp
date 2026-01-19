#include "LvglPythonBind.h"
#include "WidgetPythonBind.h"
#include "PythonUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace lvglpp::python {

LvglPythonBind::LvglPythonBind()
    : python_module_(nullptr)
    , initialized_(false) {
}

LvglPythonBind::~LvglPythonBind() {
    shutdown();
}

void LvglPythonBind::initialize() {
    if (initialized_) {
        return;
    }
    
    // Create Python module with unique name
    python_module_ = std::make_unique<py::module>(py::module::create_extension_module(
        "lvglpy", nullptr, PYBIND11_MODULE_NAME));
    
    registerPythonAPI();
    
    initialized_ = true;
    PythonUtils::printInfo("LvglPythonBind initialized successfully");
}

void LvglPythonBind::shutdown() {
    if (!initialized_) {
        return;
    }
    
    python_module_.reset();
    root_item_.reset();
    initialized_ = false;
    
    PythonUtils::printInfo("LvglPythonBind shutdown");
}

py::module& LvglPythonBind::getPythonModule() {
    return *python_module_;
}

bool LvglPythonBind::executeScript(const std::string& script) {
    if (!initialized_) {
        PythonUtils::printError("Python engine not initialized");
        return false;
    }
    
    try {
        py::exec(script.c_str(), python_module_->attr("__dict__"));
        return true;
    } catch (const py::error_already_set& e) {
        PythonUtils::printError(std::string("Script execution error: ") + e.what());
        return false;
    }
}

bool LvglPythonBind::executeFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        PythonUtils::printError("Cannot open file: " + filename);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string script = buffer.str();
    
    return executeScript(script);
}

void LvglPythonBind::setRootItem(std::shared_ptr<widgets::BaseItem> root) {
    root_item_ = root;
    
    if (python_module_ && root) {
        (*python_module_)["root"] = root;
    }
}

std::shared_ptr<widgets::BaseItem> LvglPythonBind::getRootItem() const {
    return root_item_;
}

void LvglPythonBind::registerPythonAPI() {
    auto& m = *python_module_;
    
    // Bind widget classes
    WidgetPythonBind::bindBaseItem(m);
    WidgetPythonBind::bindLvText(m);
    WidgetPythonBind::bindImage(m);
    WidgetPythonBind::bindPushButton(m);
    
    // Create lvgl namespace
    auto lvgl = m.create_submodule("lvgl");
    
    // Create functions for UI creation
    lvgl.def("create_text", [this](const std::string& text, uint32_t color, py::object parent_obj) {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.cast<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.cast<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        return std::make_shared<widgets::LvText>(text, color, parent);
    });
    
    lvgl.def("create_button", [this](int width, int height, py::object color_style_obj, const std::string& text, py::object parent_obj) {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.cast<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.cast<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        widgets::PushButton::ColorStyle color_style = widgets::PushButton::ColorStyle::Blue;
        if (color_style_obj.is<py::enum_<widgets::PushButton::ColorStyle>>()) {
            color_style = color_style_obj.cast<widgets::PushButton::ColorStyle>();
        }
        
        return std::make_shared<widgets::PushButton>(width, height, color_style, text, parent);
    });
    
    // Utility functions
    lvgl.def("print", [](const std::string& msg) {
        std::cout << "[Python] " << msg << std::endl;
    });
    
    lvgl.def("log", [](const std::string& msg) {
        std::cout << "[Python Log] " << msg << std::endl;
    });
    
    // Set root if available
    if (root_item_) {
        lvgl["root"] = root_item_;
    }
}

} // namespace lvglpp::python

// PYBIND11_MODULE definition
pybind11_MODULE(lvglpy, m) {
    m.doc() = "LVGL Python binding";
    
    // Create lvgl submodule for backward compatibility
    auto lvgl = m.create_submodule("lvgl");
    
    // Bind widget classes
    lvglpp::python::WidgetPythonBind::bindBaseItem(lvgl);
    lvglpp::python::WidgetPythonBind::bindLvText(lvgl);
    lvglpp::python::WidgetPythonBind::bindImage(lvgl);
    lvglpp::python::WidgetPythonBind::bindPushButton(lvgl);
    
    // Create functions for UI creation
    lvgl.def("create_text", [](const std::string& text, uint32_t color, py::object parent_obj) {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.cast<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.cast<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        return std::make_shared<widgets::LvText>(text, color, parent);
    });
    
    lvgl.def("create_button", [](int width, int height, py::object color_style_obj, const std::string& text, py::object parent_obj) {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.cast<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.cast<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        widgets::PushButton::ColorStyle color_style = widgets::PushButton::ColorStyle::Blue;
        if (color_style_obj.is<py::enum_<widgets::PushButton::ColorStyle>>()) {
            color_style = color_style_obj.cast<widgets::PushButton::ColorStyle>();
        }
        
        return std::make_shared<widgets::PushButton>(width, height, color_style, text, parent);
    });
    
    // Utility functions
    lvgl.def("print", [](const std::string& msg) {
        std::cout << "[Python] " << msg << std::endl;
    });
    
    lvgl.def("log", [](const std::string& msg) {
        std::cout << "[Python Log] " << msg << std::endl;
    });
}
