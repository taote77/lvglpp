#ifndef LVGLPP_LUA_UTILS_H
#define LVGLPP_LUA_UTILS_H

#include <sol/sol.hpp>
#include <string>
#include <memory>
#include <functional>

namespace lvglpp::lua {

class LuaUtils {
public:
    static void printError(sol::state& lua, const std::string& message);
    static void printInfo(sol::state& lua, const std::string& message);
    
    template<typename T>
    static std::shared_ptr<T> makeShared() {
        return std::make_shared<T>();
    }
    
    template<typename T, typename... Args>
    static std::shared_ptr<T> makeShared(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

}

#endif
