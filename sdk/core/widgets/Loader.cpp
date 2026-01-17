#include "Loader.h"
#include "Component.h"

namespace lvglpp::widgets {

Loader::Loader(BaseItem *parent) : BaseItem(parent)
{}

void Loader::setSourceComponent(std::unique_ptr<Component> &component)
{
    if (loader_component_ptr_ != nullptr && component != nullptr && typeid(*loader_component_ptr_) == typeid(*component))
    {
        return;
    }
    if (loader_component_ptr_ != nullptr)
    {
        loader_component_ptr_->onDestroy();
        lv_obj_clean(this->getLvglItem());
    }
    loader_component_ptr_ = std::move(component);
    if (loader_component_ptr_ != nullptr)
    {
        loader_component_ptr_->onLoad(this);
    }
}

void Loader::sendEventToComponent(const Event &e)
{
    if (loader_component_ptr_ != nullptr)
    {
        loader_component_ptr_->onNotifyUI(e);
    }
}
} // namespace lvglpp::widgets