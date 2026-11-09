
#ifndef LVGLPP_PAGEVIEWDELEGATE_H
#define LVGLPP_PAGEVIEWDELEGATE_H

#include "Delegate.hpp"

namespace lvglpp::widgets {

template <typename T>
class PageViewDelegate : public Delegate<T>
{
public:
    explicit PageViewDelegate(Widget *parent = nullptr) : Delegate<T>(parent)
    {}

private:
};
} // namespace lvglpp::widgets

#endif // LVGLPP_PAGEVIEWDELEGATE_H
