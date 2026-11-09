#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <codecvt>
#include <cstdlib>
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
     * 转换成xh xmin xs
     * @param sec
     * @return
     */
    static std::string convertSecToTimeLabel(int sec, const std::string &fmt = "%1%h %2%min %3%s");

    /**
     * 时间戳转换为对应格式时间
     * @param ts
     * @param fmt
     * @return
     */
    static std::string convertTimeStampToTimeStr(time_t ts, const std::string &fmt = "%Y/%m/%d    %H:%M:%S");

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
        size_t      len = input.length();
        for (size_t i = 0; i < len; ++i)
        {
            if (input[i] == '\\' && i < len - 3 && input[i + 1] == 'x')
            {
                char hex[3] = {input[i + 2], input[i + 3], '\0'};
                output += (char)strtol(hex, NULL, 16);
                i += 3;
            } else
            {
                output += input[i];
            }
        }
        return output;
    }

    static std::string Unicode2CN(std::string ssid)
    {
        try
        {
            std::string output = convertString(ssid);
            // 将Hex编码为Unicode
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring                                           unicodeString = converter.from_bytes(output);

            // 将Unicode转换为UTF-8编码的中文字符串
            std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_converter;
            std::string                                      utf8String = utf8_converter.to_bytes(unicodeString);
            //   cout << "utf8String: " << utf8String << endl;
            return utf8String;
        }
        catch (std::exception &e)
        {
            return ssid;
        }
    }
};

} // namespace tools
} // namespace lvglpp

#endif // UI_UTILS_H
