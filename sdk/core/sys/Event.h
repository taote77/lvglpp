
#ifndef LVGLPP_EVENT_H
#define LVGLPP_EVENT_H

#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/utility/string_view.hpp>
#include "source_location.hpp"
#include "core/log/log.h"

namespace heygears {
namespace sys {
class Event
{
public:
    Event(int type, int error_code, boost::any data);
    Event(int type, int error_code);

    const boost::any &getData() const { return data_; }

    int getType() const { return type_; }

    int getErrorCode() const { return error_code_; };

    template <class T>
    boost::optional<T>
    convertData(const nostd::source_location &l = nostd::source_location::current()) const noexcept;

private:
    int type_;
    int error_code_;
    boost::any data_;
};

template <class T>
boost::optional<T> Event::convertData(const nostd::source_location &l) const noexcept
{
    try {
        return boost::any_cast<T>(data_);
    } catch (const boost::bad_any_cast &) {
        boost::string_view file_path(l.file_name());
        auto index = file_path.find_last_of('/');
        LogWarn << "boost bad any cast!!!["
                << (index == -1 ? file_path : file_path.substr(index + 1)) << ":" << l.line()
                << "]";
    }
    return boost::none;
}
} // namespace sys
} // namespace heygears

#endif // LVGLPP_EVENT_H
