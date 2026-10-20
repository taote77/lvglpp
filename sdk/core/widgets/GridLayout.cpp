#include "GridLayout.h"

#include <algorithm>
#include <cctype>
#include <charconv>

namespace lvglpp::widgets {

namespace {

bool parseUnsigned(std::string_view token, long &value)
{
    if (token.empty())
    {
        return false;
    }

    long        parsed = 0;
    const auto *begin  = token.data();
    const auto *end    = token.data() + token.size();
    auto        result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc() || result.ptr != end || parsed < 0)
    {
        return false;
    }
    value = parsed;
    return true;
}

} // namespace

bool GridLayout::parseTracks(std::string_view desc, std::vector<lv_coord_t> &out, std::string *error)
{
    out.clear();

    auto fail = [&](const std::string &message) {
        if (error != nullptr)
        {
            *error = message;
        }
        out.clear();
        return false;
    };

    size_t pos = 0;
    while (pos < desc.size())
    {
        while (pos < desc.size() && std::isspace(static_cast<unsigned char>(desc[pos])))
        {
            ++pos;
        }
        if (pos >= desc.size())
        {
            break;
        }

        size_t end = pos;
        while (end < desc.size() && !std::isspace(static_cast<unsigned char>(desc[end])))
        {
            ++end;
        }
        std::string_view token = desc.substr(pos, end - pos);
        pos                    = end;

        if (out.size() >= kMaxTracks)
        {
            return fail("too many grid tracks (max 32)");
        }

        if (token == "auto" || token == "content")
        {
            out.push_back(LV_GRID_CONTENT);
            continue;
        }

        if (token.size() > 2 && token.substr(token.size() - 2) == "fr")
        {
            long value = 0;
            if (!parseUnsigned(token.substr(0, token.size() - 2), value))
            {
                return fail("invalid fr track: " + std::string(token));
            }
            value = std::clamp<long>(value, 1, 255);
            out.push_back(LV_GRID_FR(static_cast<int32_t>(value)));
            continue;
        }

        std::string_view number = token;
        if (number.size() > 2 && number.substr(number.size() - 2) == "px")
        {
            number = number.substr(0, number.size() - 2);
        }

        long value = 0;
        if (!parseUnsigned(number, value))
        {
            return fail("invalid grid track: " + std::string(token));
        }

        // Keep away from the sentinel range used by LV_GRID_CONTENT/LAST.
        value = std::clamp<long>(value, 0, LV_COORD_MAX - 200);
        out.push_back(static_cast<lv_coord_t>(value));
    }

    out.push_back(LV_GRID_TEMPLATE_LAST);
    return true;
}

} // namespace lvglpp::widgets
