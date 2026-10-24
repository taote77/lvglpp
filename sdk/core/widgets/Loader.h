#ifndef LVGLPP_LOADER_H
#define LVGLPP_LOADER_H

#include "Widget.h"
#include "core/sys/Event.h"
#include <memory>

namespace lvglpp::widgets {

class LoaderComponent;

/// Owns a LoaderComponent and gives it a widget subtree to populate.
class Loader : public Widget
{
public:
    explicit Loader(Widget *parent = nullptr);

    /// Replace the current component. Passing nullptr clears the Loader.
    /// The previous component receives onDestroy() and its subtree is removed.
    void setSourceComponent(std::unique_ptr<LoaderComponent> component);

    LoaderComponent *component() const
    {
        return loader_component_ptr_.get();
    }

    [[deprecated("use component()")]]
    LoaderComponent *getSourceComponent() const
    {
        return component();
    }

    void sendEventToComponent(const sys::Event &event);

private:
    std::unique_ptr<LoaderComponent> loader_component_ptr_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_LOADER_H
