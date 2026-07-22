#include "Calendar.h"
#include "core/base/LvObjFactory.h"
#include <algorithm>

namespace lvglpp::widgets {

Calendar::Calendar(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglCalendar, parent)
{}

void Calendar::setTodayDate(uint32_t year, uint32_t month, uint32_t day)
{
    lv_calendar_set_today_date(getLvglItem(), year, month, day);
}

void Calendar::setShownDate(uint32_t year, uint32_t month)
{
    lv_calendar_set_month_shown(getLvglItem(), year, month);
}

CalendarDate Calendar::todayDate() const
{
    auto *d = lv_calendar_get_today_date(getLvglItem());
    if (d) {
        return {static_cast<uint32_t>(d->year),
                static_cast<uint32_t>(d->month),
                static_cast<uint32_t>(d->day)};
    }
    return {2025, 1, 1};
}

CalendarDate Calendar::shownDate() const
{
    auto *d = lv_calendar_get_showed_date(getLvglItem());
    if (d) {
        return {static_cast<uint32_t>(d->year),
                static_cast<uint32_t>(d->month),
                static_cast<uint32_t>(d->day)};
    }
    return {2025, 1, 1};
}

void Calendar::setHighlightedDates(const std::vector<CalendarDate> &dates)
{
    std::vector<lv_calendar_date_t> lvDates(dates.size());
    std::transform(dates.begin(), dates.end(), lvDates.begin(),
        [](const CalendarDate &d) -> lv_calendar_date_t {
            return {static_cast<uint16_t>(d.year),
                    static_cast<int8_t>(d.month),
                    static_cast<int8_t>(d.day)};
        });
    lv_calendar_set_highlighted_dates(getLvglItem(), lvDates.data(), lvDates.size());
}

bool Calendar::getPressedDate(CalendarDate &date) const
{
    lv_calendar_date_t d;
    if (lv_calendar_get_pressed_date(getLvglItem(), &d) == LV_RESULT_OK) {
        date = {static_cast<uint32_t>(d.year),
                static_cast<uint32_t>(d.month),
                static_cast<uint32_t>(d.day)};
        return true;
    }
    return false;
}

lv_obj_t *Calendar::buttonMatrix() const
{
    return lv_calendar_get_btnmatrix(getLvglItem());
}

} // namespace lvglpp::widgets
