#ifndef LVGLPP_TOUCHAREA_H
#define LVGLPP_TOUCHAREA_H

#include "Widget.h"
#include <functional>
#include <utility>

namespace lvglpp::widgets {

struct TouchPoint {
    int32_t x = 0;
    int32_t y = 0;

    TouchPoint() = default;

    TouchPoint(int32_t xPos, int32_t yPos) : x(xPos), y(yPos)
    {}
};

using TouchPressingHandler = std::function<void(const TouchPoint &)>;
using TouchReleasedHandler = std::function<void(const TouchPoint &)>;

/// Invisible area that reports raw touch coordinates while pressing/releasing.
class TouchArea : public Widget
{
public:
    explicit TouchArea(Widget *parent = nullptr);

    void setTouchPressingHandler(TouchPressingHandler handler)
    {
        pressing_cb_ = std::move(handler);
    }

    void setTouchReleasedHandler(TouchReleasedHandler handler)
    {
        released_cb_ = std::move(handler);
    }

private:
    TouchPressingHandler pressing_cb_;
    TouchReleasedHandler released_cb_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TOUCHAREA_H
