#include "Activity.h"
#include <typeinfo>

namespace lvglpp::sys {

std::string Activity::routeId() const
{
    return typeid(*this).name();
}

} // namespace lvglpp::sys
