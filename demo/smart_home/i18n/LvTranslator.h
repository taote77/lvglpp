#ifndef LV_TRANSLATOR_H
#define LV_TRANSLATOR_H

#include <string>

#if defined(__cplusplus) && __cplusplus >= 202002L && defined(__has_include)
#if __has_include(<format>)
#include <format>
#define LVGLPP_HAS_STD_FORMAT 1
#endif
#endif

namespace lvglpp {

enum LANGUAGE {
    CHINESE  = 0, // 中文
    ENGLISH  = 1, // 英文
    JAPANESE = 2, // 日文
    RUSSIAN  = 3, // 俄文
    AR       = 4, // 阿拉伯
    LOCAL,

};

class LvTranslator
{
public:
    static LvTranslator &Instance();

    static void init();

    static void load(LANGUAGE lang);

    static const char *tr(const char *text);

    static const char *getI18nText(const char *text);

protected:
    LvTranslator();
};

const char *lvTr(const char *text);

const std::string lvTrStr(const char *text);

#ifdef LVGLPP_HAS_STD_FORMAT
/**
 * Translate a strid containing std::format placeholders and format it.
 * Example: lvTrFmt("common_device_count", 3) with "{} devices".
 */
template <typename... Args>
inline std::string lvTrFmt(const char *strid, Args &&...args)
{
    const char *fmt = lvTr(strid);
    try
    {
        return std::vformat(fmt, std::make_format_args(args...));
    }
    catch (const std::format_error &)
    {
        // A bad translation must not take down the UI; fall back to the raw
        // translated string (still better than the strid).
        return fmt;
    }
}
#endif

} // namespace lvglpp

#endif // LV_TRANSLATOR_H
