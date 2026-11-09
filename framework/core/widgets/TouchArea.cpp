#include "TouchArea.h"

namespace lvglpp::widgets {

TouchArea::TouchArea(Widget *parent) : Widget(parent)
{
    setOpacity(0);

    addLvglEventHandler(
        [](lv_event_t *e) -> void {
            auto *area = static_cast<TouchArea *>(lv_event_get_user_data(e));
            if (area == nullptr || !area->pressing_cb_)
            {
                return;
            }

            lv_indev_t *indev = lv_indev_active();
            if (indev == nullptr)
            {
                return;
            }

            lv_point_t point{};
            lv_indev_get_point(indev, &point);
            area->pressing_cb_(TouchPoint(point.x, point.y));
        },
        LV_EVENT_PRESSING, this);

    addLvglEventHandler(
        [](lv_event_t *e) -> void {
            auto *area = static_cast<TouchArea *>(lv_event_get_user_data(e));
            if (area == nullptr || !area->released_cb_)
            {
                return;
            }

            lv_indev_t *indev = lv_indev_active();
            if (indev == nullptr)
            {
                return;
            }

            lv_point_t point{};
            lv_indev_get_point(indev, &point);
            area->released_cb_(TouchPoint(point.x, point.y));
        },
        LV_EVENT_RELEASED, this);
}

} // namespace lvglpp::widgets
