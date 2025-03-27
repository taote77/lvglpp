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
    if (getLvglItem() != nullptr && lv_obj_is_valid(getLvglItem()))
    {
        // LV_LOG_USER("Delegate delete!! %ld",getLvglItem());
        lv_obj_del(getLvglItem());
        setLvglItem(nullptr);
    }
}

} // namespace widgets
} // namespace lvglpp

#endif // LV_DELEGATE_H
