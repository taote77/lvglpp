
#include "Utils.h"
#include "base64.h"
#include "core/sys/Application.h"
#include "trTranslateImpl.h"
#include <codecvt>
#include <cstring>
#include <locale>

namespace lvglpp {
namespace tools {
char Utils::g_asset_letter     = 'G';
char Utils::g_mem_asset_letter = 'M';

std::string Utils::getAssetDirRoot()
{
    std::string path;
    path = sys::Application::applicationDirPath();
    path += "/asset";
    return path;
}

std::string Utils::convertAssetResToGlobal(const std::string &asset_res)
{
    if (isAssetResUrl(asset_res))
    {
        return getAssetDirRoot() + "/" + asset_res.substr(2);
    }
    return asset_res;
}

std::string Utils::readAllText(const std::string &path)
{
    FILE       *fp = fopen(path.c_str(), "rb");
    std::string result_str;
    if (fp == nullptr)
    {
        return result_str;
    }
    int       buff_size           = 256;
    const int read_size           = 256;
    int       current_pos         = 0;
    char     *res_buf             = new char[buff_size];
    char      temp_buf[read_size] = {0};
    memset(res_buf, 0, buff_size);
    while (1)
    {
        int n = fread(&temp_buf, 1, read_size, fp);
        if (n <= 0)
        {
            break;
        }
        if (current_pos + n > buff_size)
        {
            buff_size *= 2;
            char *new_res_buf = new char[buff_size];
            memset(new_res_buf, 0, buff_size);
            memcpy(new_res_buf, res_buf, current_pos);
            delete[] res_buf;
            res_buf = new_res_buf;
        }
        memcpy(res_buf + current_pos, temp_buf, n);
        current_pos += n;
    }
    fclose(fp);
    result_str = std::string(res_buf, current_pos);
    delete[] res_buf;
    return result_str;
}

bool Utils::isAssetResUrl(const std::string &asset_res)
{
    return asset_res.size() >= 2 && asset_res[0] == 'G' && asset_res[1] == ':';
}

std::string Utils::convertSecToTimeLabel(int secRemain, const std::string &fmt)
{
    int sec       = secRemain % 60;
    int totalMin  = secRemain / 60;
    int totalHour = totalMin / 60;
    int showMin   = totalMin % 60;

    // Simple printf-style replacement for %1%, %2%, %3%
    std::string result = fmt;
    auto replace = [&result](const std::string &token, int value) {
        size_t pos = result.find(token);
        if (pos != std::string::npos) {
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

std::string tr(const std::string &str_src)
{
    if (sys::Application::getLanguageType() == sys::Application::Chinese)
    {
        return trTranslateImpl::getInstance(trTranslateImpl::English)->translate(str_src);
    } else if (sys::Application::getLanguageType() == sys::Application::English)
    {
        return trTranslateImpl::getInstance(trTranslateImpl::Chinese)->translate(str_src);
    }
    return str_src;
}
} // namespace tools
} // namespace lvglpp
