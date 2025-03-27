#include "Loader.h"
#include "Component.h"

namespace lvglpp {
namespace widgets {

Loader::Loader(BaseItem *parent) : BaseItem(parent)
{}

void Loader::setSourceComponent(const std::shared_ptr<Component> &component)
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
    loader_component_ptr_ = component;
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
} // namespace widgets
} // namespace lvglpp