/**************************************************************************

Description: Component base class for use with Loader

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
     * @brief Called when the component is loaded into a parent
     * @param parent the parent widget (passed from Loader)
     * @return true on success
     */
    virtual bool onLoad(BaseItem *parent) = 0;
    /**
     * @brief Called before the component is destroyed
     */
    virtual void onDestroy()
    {}

    /// Receive notification events
    virtual void onNotifyUI(const Event &e)
    {}

private:
    friend class Loader;
};

} // namespace widgets
} // namespace lvglpp

#endif // LVGLPP_COMPONENT_H
