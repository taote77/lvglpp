#ifndef LV_TRANSLATOR_H
#define LV_TRANSLATOR_H

#include <string>
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

} // namespace lvglpp

#endif // LV_TRANSLATOR_H
