
#include "Utils.h"
#include "base64.h"
#include <codecvt>
#include <locale>

namespace lvglpp {
namespace tools {
std::string Utils::convertSecToTimeLabel(int secRemain, const std::string &fmt)
{
    int sec       = secRemain % 60;
    int totalMin  = secRemain / 60;
    int totalHour = totalMin / 60;
    int showMin   = totalMin % 60;

    // Simple printf-style replacement for %1%, %2%, %3%
    std::string result  = fmt;
    auto        replace = [&result](const std::string &token, int value) {
        size_t pos = result.find(token);
        if (pos != std::string::npos)
        {
            result.replace(pos, token.size(), std::to_string(value));
        }
    };
    replace("%1%", totalHour);
    replace("%2%", showMin);
    replace("%3%", sec);
    return result;
}

std::string Utils::convertTimeStampToTimeStr(time_t ts, const std::string &fmt)
{
    struct tm time_info = {0};
    localtime_r(&ts, &time_info);
    char buffer[256] = {0};
    strftime(buffer, 256, fmt.c_str(), &time_info);
    return buffer;
}

std::wstring Utils::s2ws(const std::string &s)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.from_bytes(s);
}

std::string Utils::ws2s(const std::wstring &ws)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
    return convert.to_bytes(ws);
}

bool Utils::base64Decode(const std::string &input, std::string &output)
{
    output = base64_decode(input);
    return !output.empty();
}

bool Utils::base64Encode(const std::string &input, std::string &output)
{
    output = base64_encode(input);
    return !output.empty();
}

} // namespace tools
} // namespace lvglpp
