
#ifndef LV_PAGEVIEW_DELEGATE_H
#define LV_PAGEVIEW_DELEGATE_H

#include "Delegate.hpp"

namespace lvglpp {
namespace widgets {
template <typename T>
class PageViewDelegate : public Delegate<T>
{
public:
    explicit PageViewDelegate(BaseItem *parent = nullptr) : Delegate<T>(parent)
    {}

private:
};
} // namespace widgets
} // namespace lvglpp
#endif // LV_PAGEVIEW_DELEGATE_H
