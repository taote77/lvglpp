
#ifndef LV_LISTVIEW_DELEGATE_H
#define LV_LISTVIEW_DELEGATE_H

#include "Delegate.hpp"

namespace lvglpp::widgets {

template <typename T>
class ListViewDelegate : public Delegate<T>
{
public:
    explicit ListViewDelegate(int index, BaseItem *parent = nullptr) : Delegate<T>(parent), index_(index)
    {}

protected:
    int index_;
};

} // namespace lvglpp::widgets

#endif // LV_LISTVIEW_DELEGATE_H
