#ifndef LVGLPP_CALENDAR_H
#define LVGLPP_CALENDAR_H

#include "BaseItem.h"
#include <vector>

namespace lvglpp::widgets {

struct CalendarDate {
    uint32_t year;
    uint32_t month;
    uint32_t day;
};

class Calendar : public BaseItem
{
public:
    explicit Calendar(BaseItem *parent = nullptr);

    /// Set the "today" date (highlighted)
    void setTodayDate(uint32_t year, uint32_t month, uint32_t day);

    /// Set the currently displayed month
    void setShownDate(uint32_t year, uint32_t month);

    /// Get the "today" date
    CalendarDate todayDate() const;

    /// Get the currently shown date
    CalendarDate shownDate() const;

    /// Set highlighted dates (e.g., for marking events)
    void setHighlightedDates(const std::vector<CalendarDate> &dates);

    /// Get the pressed/selected date (if any). Returns false if no date is pressed.
    bool getPressedDate(CalendarDate &date) const;

    /// Get the underlying button matrix for custom styling
    lv_obj_t *buttonMatrix() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_CALENDAR_H
