#include "LuaUtils.h"
#include <iostream>

namespace lvglpp::lua {

void LuaUtils::printError(sol::state& lua, const std::string& message) {
    std::cerr << "[Lua Error] " << message << std::endl;
}

void LuaUtils::printInfo(sol::state& lua, const std::string& message) {
    std::cout << "[Lua Info] " << message << std::endl;
}

}
