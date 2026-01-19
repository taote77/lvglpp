#include "LvglLuaBind.h"
#include "WidgetLuaBind.h"
#include "LuaUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace lvglpp::lua {

LvglLuaBind::LvglLuaBind()
    : lua_state_(std::make_unique<sol::state>())
    , root_item_(nullptr)
    , initialized_(false) {
}

LvglLuaBind::~LvglLuaBind() {
    shutdown();
}

void LvglLuaBind::initialize() {
    if (initialized_) {
        return;
    }
    
    setupLuaLibraries();
    registerLuaAPI();
    
    initialized_ = true;
    LuaUtils::printInfo(*lua_state_, "LvglLuaBind initialized successfully");
}

void LvglLuaBind::shutdown() {
    if (!initialized_) {
        return;
    }
    
    lua_state_.reset();
    root_item_.reset();
    initialized_ = false;
    
    LuaUtils::printInfo(*lua_state_, "LvglLuaBind shutdown");
}

sol::state& LvglLuaBind::getLuaState() {
    return *lua_state_;
}

bool LvglLuaBind::executeScript(const std::string& script) {
    if (!initialized_) {
        LuaUtils::printError(*lua_state_, "Lua engine not initialized");
        return false;
    }
    
    try {
        auto result = lua_state_->script(script);
        if (!result.valid()) {
            sol::error err = result;
            LuaUtils::printError(*lua_state_, err.what());
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        LuaUtils::printError(*lua_state_, std::string("Script execution error: ") + e.what());
        return false;
    }
}

bool LvglLuaBind::executeFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        LuaUtils::printError(*lua_state_, "Cannot open file: " + filename);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string script = buffer.str();
    
    return executeScript(script);
}

void LvglLuaBind::setRootItem(std::shared_ptr<widgets::BaseItem> root) {
    root_item_ = root;
    
    if (lua_state_ && root) {
        (*lua_state_)["root"] = root;
    }
}

std::shared_ptr<widgets::BaseItem> LvglLuaBind::getRootItem() const {
    return root_item_;
}

void LvglLuaBind::setupLuaLibraries() {
    lua_state_->open_libraries(
        sol::lib::base,
        sol::lib::coroutine,
        sol::lib::string,
        sol::lib::io,
        sol::lib::math,
        sol::lib::table
    );
}

void LvglLuaBind::registerLuaAPI() {
    auto lvgl = lua_state_->create_table("lvgl");
    
    WidgetLuaBind::bindAll(*lua_state_);
    
    lvgl["create_text"] = [this](const std::string& text, uint32_t color, sol::object parent_obj) -> std::shared_ptr<widgets::LvText> {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.as<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.as<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        return std::make_shared<widgets::LvText>(text, color, parent, widgets::LvText::TextStyle::FontSize20, widgets::LvText::FontType::Auto);
    };
    
    lvgl["create_text_ex"] = [this](const std::string& text, uint32_t color, sol::object parent_obj, 
                                     widgets::LvText::TextStyle style, widgets::LvText::FontType font_type) -> std::shared_ptr<widgets::LvText> {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.as<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.as<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        return std::make_shared<widgets::LvText>(text, color, parent, style, font_type);
    };
    
    lvgl["create_image"] = [this](const std::string& path, sol::object parent_obj) -> std::shared_ptr<widgets::Image> {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.as<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.as<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        return std::make_shared<widgets::Image>(path, parent);
    };
    
    lvgl["create_button"] = [this](int width, int height, const std::string& text, sol::object parent_obj) -> std::shared_ptr<widgets::PushButton> {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.as<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.as<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        return std::make_shared<widgets::PushButton>(width, height, widgets::PushButton::ColorStyle::Blue, text, parent);
    };
    
    lvgl["create_button_ex"] = [this](int width, int height, widgets::PushButton::ColorStyle color_style, 
                                       const std::string& text, sol::object parent_obj) -> std::shared_ptr<widgets::PushButton> {
        widgets::BaseItem* parent = nullptr;
        if (parent_obj.is<widgets::BaseItem*>()) {
            parent = parent_obj.as<widgets::BaseItem*>();
        } else if (parent_obj.is<std::shared_ptr<widgets::BaseItem>>()) {
            parent = parent_obj.as<std::shared_ptr<widgets::BaseItem>>().get();
        }
        
        return std::make_shared<widgets::PushButton>(width, height, color_style, text, parent);
    };
    
    lvgl["print"] = [](const std::string& msg) {
        std::cout << "[Lua] " << msg << std::endl;
    };
    
    lvgl["log"] = [](const std::string& msg) {
        std::cout << "[Lua Log] " << msg << std::endl;
    };
    
    if (root_item_) {
        (*lua_state_)["root"] = root_item_;
    }
}

}
