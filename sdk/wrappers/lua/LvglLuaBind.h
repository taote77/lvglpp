#ifndef LVGLPP_LVGL_LUA_BIND_H
#define LVGLPP_LVGL_LUA_BIND_H

#include <sol/sol.hpp>
#include <memory>
#include "core/widgets/BaseItem.h"

namespace lvglpp::lua {

class LvglLuaBind {
public:
    LvglLuaBind();
    ~LvglLuaBind();
    
    void initialize();
    void shutdown();
    
    sol::state& getLuaState();
    
    bool executeScript(const std::string& script);
    bool executeFile(const std::string& filename);
    
    void setRootItem(std::shared_ptr<widgets::BaseItem> root);
    std::shared_ptr<widgets::BaseItem> getRootItem() const;
    
private:
    void registerLuaAPI();
    void setupLuaLibraries();
    
    std::unique_ptr<sol::state> lua_state_;
    std::shared_ptr<widgets::BaseItem> root_item_;
    bool initialized_;
};

}

#endif
