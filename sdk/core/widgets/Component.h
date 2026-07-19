/**************************************************************************

Description:搭配Loader使用

**************************************************************************/

#ifndef LVGLPP_COMPONENT_H
#define LVGLPP_COMPONENT_H

#include "core/sys/Event.h"

namespace lvglpp {
namespace widgets {
using namespace lvglpp::sys;
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
    virtual void onDestroy()
    {}

    virtual void onNotifyUI(const Event &e)
    {}

private:
    friend class Loader;
};

} // namespace widgets
} // namespace lvglpp

#endif // LVGLPP_COMPONENT_H
