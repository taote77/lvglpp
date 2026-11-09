#include "AutomationController.h"
#include <lvgl.h>
#include <thread>

namespace lvglpp::sys {

void AutomationController::_sendClickEvent(lv_obj_t *obj)
{
    if (!obj)
        return;
    lv_obj_send_event(obj, LV_EVENT_PRESSED, nullptr);
    lv_obj_send_event(obj, LV_EVENT_CLICKED, nullptr);
    lv_obj_send_event(obj, LV_EVENT_RELEASED, nullptr);
}

void AutomationController::click(widgets::Widget *item)
{
    if (!item)
        return;
    constexpr auto minInterval = std::chrono::milliseconds(100);
    auto           now         = std::chrono::steady_clock::now();
    if (now - _lastClickTime < minInterval)
    {
        std::this_thread::sleep_for(minInterval - (now - _lastClickTime));
    }
    _sendClickEvent(item->lvObj());
    _lastClickTime = std::chrono::steady_clock::now();
}

void AutomationController::longPress(widgets::Widget *item, int durationMs)
{
    if (!item)
        return;
    auto *obj = item->lvObj();
    lv_obj_send_event(obj, LV_EVENT_PRESSED, nullptr);
    if (durationMs > 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
    }
    lv_obj_send_event(obj, LV_EVENT_LONG_PRESSED, nullptr);
    lv_obj_send_event(obj, LV_EVENT_RELEASED, nullptr);
}

void AutomationController::typeText(widgets::Widget *item, const std::string &text)
{
    if (!item)
        return;
    lv_label_set_text(item->lvObj(), text.c_str());
}

void AutomationController::scrollTo(widgets::Widget *item, lv_coord_t x, lv_coord_t y)
{
    if (!item)
        return;
    lv_obj_scroll_to(item->lvObj(), x, y, LV_ANIM_ON);
}

void AutomationController::scrollBy(widgets::Widget *item, lv_coord_t dx, lv_coord_t dy)
{
    if (!item)
        return;
    lv_obj_scroll_by(item->lvObj(), dx, dy, LV_ANIM_ON);
}

void AutomationController::clickAt(lv_coord_t x, lv_coord_t y)
{
    lv_point_t pt = {x, y};
    // Search from the active screen's top layer
    auto *obj = lv_indev_search_obj(lv_layer_top(), &pt);
    if (obj)
        _sendClickEvent(obj);
}

widgets::Widget *AutomationController::goBack(widgets::Widget *current)
{
    return current ? current->getParent() : nullptr;
}

std::string AutomationController::getText(widgets::Widget *item)
{
    if (!item)
        return {};
    return lv_label_get_text(item->lvObj());
}

widgets::Widget *AutomationController::hitTest(lv_coord_t x, lv_coord_t y)
{
    lv_point_t pt  = {x, y};
    lv_obj_t  *obj = lv_indev_search_obj(lv_layer_top(), &pt);
    if (!obj)
        return nullptr;
    return widgets::Widget::fromLvglObject(obj);
}

} // namespace lvglpp::sys
