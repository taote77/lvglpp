#include "Loader.h"
#include "LoaderComponent.h"
#include "core/log/log.h"

namespace lvglpp::widgets {

Loader::Loader(Widget *parent) : Widget(parent)
{}

void Loader::setSourceComponent(std::unique_ptr<LoaderComponent> component)
{
    if (loader_component_ptr_ != nullptr)
    {
        loader_component_ptr_->onDestroy();
        lv_obj_clean(lvObj());
        loader_component_ptr_.reset();
    }

    if (component == nullptr)
    {
        return;
    }

    loader_component_ptr_ = std::move(component);
    if (!loader_component_ptr_->onLoad(this))
    {
        LOG_WARN() << "LoaderComponent::onLoad() failed; discarding component";
        loader_component_ptr_->onDestroy();
        lv_obj_clean(lvObj());
        loader_component_ptr_.reset();
    }
}

void Loader::sendEventToComponent(const sys::Event &event)
{
    if (loader_component_ptr_ != nullptr)
    {
        loader_component_ptr_->onNotifyUI(event);
    }
}

} // namespace lvglpp::widgets
