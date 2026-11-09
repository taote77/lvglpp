#ifndef LVGLPP_EVENT_H
#define LVGLPP_EVENT_H

#include "core/log/log.h"
#include "source_location.hpp"
#include <any>
#include <optional>
#include <string_view>

namespace lvglpp::sys {

class Event
{
public:
    Event(int type, int error_code, std::any data);
    Event(int type, int error_code);

    const std::any &getData() const
    {
        return data_;
    }

    int getType() const
    {
        return type_;
    }

    int getErrorCode() const
    {
        return error_code_;
    }

    template <class T>
    std::optional<T> convertData(const nostd::source_location &l = nostd::source_location::current()) const noexcept;

private:
    int      type_;
    int      error_code_;
    std::any data_;
};

template <class T>
std::optional<T> Event::convertData(const nostd::source_location &l) const noexcept
{
    try
    {
        return std::any_cast<T>(data_);
    }
    catch (const std::bad_any_cast &)
    {
        std::string_view file_path(l.file_name());
        auto              index = file_path.find_last_of('/');
        LogWarn << "bad any_cast!!![" << (index == std::string_view::npos ? file_path : file_path.substr(index + 1)) << ":" << l.line() << "]";
    }
    return std::nullopt;
}

} // namespace lvglpp::sys

#endif // LVGLPP_EVENT_H
