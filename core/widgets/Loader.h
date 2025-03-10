/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-07

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_LOADER_H
#define ULTRACORE_FOR_MAKERS_LOADER_H
#include "BaseItem.h"
#include "core/sys/Event.h"
#include <memory>
namespace heygears {
namespace widgets {
using namespace heygears::sys;
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
} // namespace heygears

#endif // ULTRACORE_FOR_MAKERS_LOADER_H
