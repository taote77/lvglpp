/**************************************************************************


**************************************************************************/

#ifndef HEYGEARS_LVGL_DEMO_TRENGLISHIMPL_H
#define HEYGEARS_LVGL_DEMO_TRENGLISHIMPL_H

#include <string>
#include <map>

namespace lvglpp {
namespace tools {
class trTranslateImpl
{
public:
    enum LanguageType { English, Chinese };

    trTranslateImpl(const trTranslateImpl &) = delete;

    trTranslateImpl &operator=(const trTranslateImpl &) = delete;

    static trTranslateImpl *getInstance(LanguageType type);

    /**
     * 语言转换
     * @param src_text
     * @return
     */
    std::string translate(const std::string &src_text);

private:
    trTranslateImpl(LanguageType type);

    void init(LanguageType type);

    static std::map<LanguageType, trTranslateImpl *> instance_;
    std::map<std::string, std::string> translate_map_;
};
} // namespace tools
} // namespace lvglpp

#endif // HEYGEARS_LVGL_DEMO_TRENGLISHIMPL_H
