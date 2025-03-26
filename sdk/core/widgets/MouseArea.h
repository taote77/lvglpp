#ifndef LV_MOUSEAREA_H
#define LV_MOUSEAREA_H

#include "BaseItem.h"
#include <functional>

namespace lvglpp {
namespace widgets {

struct Point
{
    int32_t x;
    int32_t y;

    Point()
    {
        x = 0;
        y = 0;
    }
    Point(int xPos, int yPos)
    {
        x = xPos;
        y = yPos;
    }
};
typedef std::function<void(const Point &)> MousePressingCB;
typedef std::function<void(const Point &)> MouseReleaseCB;

class MouseArea : public BaseItem
{
public:
    explicit MouseArea(BaseItem *parent = nullptr);
    void setMousePressingListener(MousePressingCB cb) { pressing_cb_ = cb; }
    void setMouseReleasedListener(MouseReleaseCB cb) { released_cb_ = cb; }

private:
    MousePressingCB pressing_cb_;
    MouseReleaseCB released_cb_;
};

} // namespace widgets
} // namespace lvglpp

#endif // LV_MOUSEAREA_H
