
#ifndef LVGLPP_LISTVIEWDELEGATE_H
#define LVGLPP_LISTVIEWDELEGATE_H

#include "Delegate.hpp"

namespace lvglpp::widgets {

template <typename T>
class ListViewDelegate : public Delegate<T>
{
public:
    explicit ListViewDelegate(int index, Widget *parent = nullptr) : Delegate<T>(parent), index_(index)
    {}

protected:
    int index_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_LISTVIEWDELEGATE_H
