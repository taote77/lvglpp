
#ifndef LV_LOADER_H
#define LV_LOADER_H

#include "BaseItem.h"
#include "core/sys/Event.h"
#include <memory>

namespace lvglpp {
namespace widgets {

using namespace lvglpp::sys;
class Component;
class Loader : public BaseItem
{
public:
    explicit Loader(BaseItem *parent = nullptr);
    void setSourceComponent(const std::shared_ptr<Component> &component);
    Component *getSourceComponent() const { return loader_component_ptr_.get(); }
    void sendEventToComponent(const Event &e);

private:
    std::shared_ptr<Component> loader_component_ptr_;
};

} // namespace widgets
} // namespace lvglpp

#endif // LV_LOADER_H
