
#include "Event.h"

namespace lvglpp::sys {
Event::Event(int type, int error_code, std::any data) : type_(type), data_(std::move(data)), error_code_(error_code)
{}

Event::Event(int type, int error_code) : type_(type), error_code_(error_code)
{}

} // namespace lvglpp::sys
