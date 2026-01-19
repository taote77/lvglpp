#ifndef LVGLPP_WIDGET_LUA_BIND_H
#define LVGLPP_WIDGET_LUA_BIND_H

#include <sol/sol.hpp>
#include <memory>
#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/Image.h"
#include "core/widgets/PushButton.h"

namespace lvglpp::lua {

class WidgetLuaBind {
public:
    static void bindBaseItem(sol::state& lua);
    static void bindLvText(sol::state& lua);
    static void bindImage(sol::state& lua);
    static void bindPushButton(sol::state& lua);
    
    static void bindAll(sol::state& lua);
    
private:
    template<typename T>
    static void bindCommonMethods(sol::usertype<T>& usertype);
};

}

#endif
