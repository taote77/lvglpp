/**************************************************************************

Description: content controller loaded and owned by Loader

**************************************************************************/

#ifndef LVGLPP_LOADERCOMPONENT_H
#define LVGLPP_LOADERCOMPONENT_H

#include "core/sys/Event.h"

namespace lvglpp::widgets {

class Widget;

/// Content controller owned by a Loader. It is not a widget itself: onLoad()
/// builds the widget subtree under the supplied Loader object.
class LoaderComponent
{
public:
    virtual ~LoaderComponent() = default;

protected:
    /**
     * @brief Called when the component is loaded into a Loader.
     * @param parent the Loader widget that will own the created subtree
     * @return true on success; a failed component is destroyed by Loader
     */
    virtual bool onLoad(Widget *parent) = 0;

    /// Called before the component is replaced or destroyed.
    virtual void onDestroy()
    {}

    /// Receive UI notifications forwarded by Loader.
    virtual void onNotifyUI(const sys::Event &event)
    {
        (void)event;
    }

private:
    friend class Loader;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_LOADERCOMPONENT_H
