/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-07

Class:${CLASS}

Description:搭配Loader使用

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_COMPONENT_H
#define ULTRACORE_FOR_MAKERS_COMPONENT_H
#include "core/sys/Event.h"
namespace heygears {
namespace widgets {
using namespace heygears::sys;
class BaseItem;
class Component
{
public:
    virtual ~Component() = default;

protected:
    /**
     * 加载控件时使用的回调
     * @param parent 传入loader的指针
     * @return
     */
    virtual bool onLoad(BaseItem *parent) = 0;
    /**
     * 组件销毁前执行的回调
     */
    virtual void onDestroy() { }

    virtual void onNotifyUI(const Event &e) { }

private:
    friend class Loader;
};

} // namespace widgets
} // namespace heygears

#endif // ULTRACORE_FOR_MAKERS_COMPONENT_H
