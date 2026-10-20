#ifndef LV_DELEGATE_H
#define LV_DELEGATE_H

#include "ScrollItem.h"

namespace lvglpp {
namespace widgets {
template <class T>
class Delegate : public ScrollItem
{
public:
    explicit Delegate(BaseItem *parent = nullptr) : ScrollItem(parent)
    {}

    ~Delegate() override;

    void initItem(const T &t)
    {
        draw(t);
    }

    bool updateItem(const T &t)
    {
        return updateData(t);
    }

protected:
    virtual void draw(const T &) = 0;
    virtual bool updateData(const T &)
    {
        return false;
    }

private:
};

template <class T>
Delegate<T>::~Delegate()
{
    // ListView/PageView rely on a delegate removing its row when the wrapper
    // is cleared from the model. The delete watchdog detaches this wrapper
    // safely if the parent tree was destroyed first.
    lv_obj_t *obj = getLvglItem();
    if (obj != nullptr && lv_obj_is_valid(obj))
    {
        lv_obj_delete(obj);
    }
}

} // namespace widgets
} // namespace lvglpp

#endif // LV_DELEGATE_H
