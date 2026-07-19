#ifndef LVGLPP_AUTOMATION_CONTROLLER_H
#define LVGLPP_AUTOMATION_CONTROLLER_H

#include "core/widgets/BaseItem.h"
#include <chrono>
#include <functional>
#include <string>
#include <vector>

namespace lvglpp::sys {

/// Simulates user interactions on widgets — for AI agents, testing, automation
class AutomationController
{
public:
    static AutomationController &getInstance()
    {
        static AutomationController instance;
        return instance;
    }

    AutomationController(const AutomationController &) = delete;
    AutomationController &operator=(const AutomationController &) = delete;

    /// Simulate a click on the given widget
    void click(widgets::BaseItem *item);

    /// Simulate a long press (ms duration)
    void longPress(widgets::BaseItem *item, int durationMs = 500);

    /// Set text on a text-based widget (LvText/Text/SpinBox)
    void typeText(widgets::BaseItem *item, const std::string &text);

    /// Scroll the widget to a position
    void scrollTo(widgets::BaseItem *item, lv_coord_t x, lv_coord_t y);

    /// Scroll by delta
    void scrollBy(widgets::BaseItem *item, lv_coord_t dx, lv_coord_t dy);

    /// Move to a screen coordinate and click (absolute position)
    void clickAt(lv_coord_t x, lv_coord_t y);

    /// Navigate to parent widget
    widgets::BaseItem *goBack(widgets::BaseItem *current);

    /// Get text content of a widget (if it has text)
    std::string getText(widgets::BaseItem *item);

    /// Find the widget at a given screen coordinate
    widgets::BaseItem *hitTest(lv_coord_t x, lv_coord_t y);

private:
    AutomationController() = default;
    void _sendClickEvent(lv_obj_t *obj);

    std::chrono::steady_clock::time_point _lastClickTime;
};

} // namespace lvglpp::sys

#endif // LVGLPP_AUTOMATION_CONTROLLER_H
