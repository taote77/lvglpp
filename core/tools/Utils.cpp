/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-03-20

Class:${CLASS}

Description:

**************************************************************************/

#include "Utils.h"
#include "core/sys/Application.h"
#include "trTranslateImpl.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <codecvt>
#include <locale>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <sstream>
#include "base64.h"

namespace heygears {
namespace tools {
char Utils::g_asset_letter = 'G';
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
    if (isAssetResUrl(asset_res)) {
        return getAssetDirRoot() + "/" + asset_res.substr(2);
    }
    return asset_res;
}

std::string Utils::readAllText(const std::string &path)
{
    FILE *fp = fopen(path.c_str(), "rb");
    std::string result_str;
    if (fp == nullptr) {
        return result_str;
    }
    int buff_size = 256;
    const int read_size = 256;
    int current_pos = 0;
    char *res_buf = new char[buff_size];
    char temp_buf[read_size] = { 0 };
    memset(res_buf, 0, buff_size);
    while (1) {
        int n = fread(&temp_buf, 1, read_size, fp);
        if (n <= 0) {
            break;
        }
        if (current_pos + n > buff_size) {
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
    return boost::starts_with(asset_res, "G:");
}

std::string Utils::convertSecToTimeLabel(int secRemain, const std::string &boost_fmt)
{
    std::string str_reply;
    int sec = secRemain % 60;
    int totalMin = secRemain / 60;
    int totalHour = totalMin / 60;
    int showMin = totalMin % 60;
    boost::format fmt(boost_fmt);
    fmt % totalHour;
    fmt % showMin;
    fmt % sec;
    str_reply = fmt.str();
    return str_reply;
}

std::string Utils::convertTimeStampToTimeStr(time_t ts, const std::string &fmt)
{
    struct tm time_info = { 0 };
    localtime_r(&ts, &time_info);
    char buffer[256] = { 0 };
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
#if 0
            typedef boost::archive::iterators::transform_width<boost::archive::iterators::binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIterator;
            std::stringstream result;
            try
            {
                copy(Base64DecodeIterator(input.begin()), Base64DecodeIterator(input.end()),
                     std::ostream_iterator<char>(result));
            }
            catch (...)
            {
                return false;
            }
            output = result.str();
            return output.empty() == false;
#else
    output = base64_decode(input);
    return !output.empty();
#endif
}

bool Utils::base64Encode(const std::string &input, std::string &output)
{
    output = base64_encode(input);
    return !output.empty();
}

std::string tr(const std::string &str_src)
{
    if (sys::Application::getLanguageType() == sys::Application::English) {
        return trTranslateImpl::getInstance(trTranslateImpl::English)->translate(str_src);
    } else if (sys::Application::getLanguageType() == sys::Application::English2Chinese) {
        return trTranslateImpl::getInstance(trTranslateImpl::Chinese)->translate(str_src);
    }
    return str_src;
}
} // namespace tools
} // namespace heygears
