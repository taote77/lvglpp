/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-03-20

Class:${CLASS}

Description:全局工具类

**************************************************************************/

#ifndef HEYGEARS_LVGL_DEMO_UTILS_H
#define HEYGEARS_LVGL_DEMO_UTILS_H

#include <codecvt>
#include <locale>
#include <string>

namespace lvglpp {
namespace tools {
class Utils
{
public:
    Utils() = delete;

    Utils(const Utils &) = delete;

    Utils operator=(const Utils &) = delete;

    /**
     * 获取资源访问的根路径
     * @return
     */
    static std::string getAssetDirRoot();

    /**
     * 把资源系统路径转化为系统路径
     * @param asset_res
     * @return
     */
    static std::string convertAssetResToGlobal(const std::string &asset_res);

    /**
     * 读取文件全部内容
     * @param path
     * @return
     */
    static std::string readAllText(const std::string &path);

    /**
     * 判读是否是资源URL
     * @param asset_res
     * @return
     */
    static bool isAssetResUrl(const std::string &asset_res);

    static char getAssetetter() { return g_asset_letter; }

    static char getMemAssetLetter() { return g_mem_asset_letter; }

    /**
     * 转换成xh xmin xs
     * @param sec
     * @return
     */
    static std::string convertSecToTimeLabel(int sec,
                                             const std::string &boost_fmt = "%1%h %2%min %3%s");

    /**
     * 时间戳转换为对应格式时间
     * @param ts
     * @param fmt
     * @return
     */
    static std::string convertTimeStampToTimeStr(time_t ts,
                                                 const std::string &fmt = "%Y/%m/%d    %H:%M:%S");

    /**
     * std::string to std::wstring
     * @param s
     * @return
     */
    static std::wstring s2ws(const std::string &s);

    /**
     * std::wstring to std::string
     * @param ws
     * @return
     */
    static std::string ws2s(const std::wstring &ws);

    static bool base64Decode(const std::string &input, std::string &output);

    static bool base64Encode(const std::string &input, std::string &output);

    static std::string convertString(const std::string &input)
    {
        std::string output;
        size_t len = input.length();
        for (size_t i = 0; i < len; ++i) {
            if (input[i] == '\\' && i < len - 3 && input[i + 1] == 'x') {
                char hex[3] = { input[i + 2], input[i + 3], '\0' };
                output += (char)strtol(hex, NULL, 16);
                i += 3;
            } else {
                output += input[i];
            }
        }
        return output;
    }

    static std::string Unicode2CN(std::string ssid)
    {
        try {
            std::string output = convertString(ssid);
            // 将Hex编码为Unicode
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring unicodeString = converter.from_bytes(output);

            // 将Unicode转换为UTF-8编码的中文字符串
            std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_converter;
            std::string utf8String = utf8_converter.to_bytes(unicodeString);
            //   cout << "utf8String: " << utf8String << endl;
            return utf8String;
        } catch (std::exception &e) {
            return ssid;
        }
    }

private:
    static char g_asset_letter;
    static char g_mem_asset_letter;
};

std::string tr(const std::string &str_src);
} // namespace tools
} // namespace lvglpp

#endif // HEYGEARS_LVGL_DEMO_UTILS_H
