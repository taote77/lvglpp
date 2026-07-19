
#ifndef LVFONTMANAGER_H
#define LVFONTMANAGER_H

#include "lvgl.h"
#include <map>
#include <string>

namespace lvglpp::tools {
class LvFontManager
{
public:
    enum FontName { SansSC, DMSans, Oswald };
    enum FontStyle { Normal, Bold };

    LvFontManager() = delete;

    LvFontManager(const LvFontManager &) = delete;

    LvFontManager operator=(const LvFontManager &) = delete;

    static const lv_font_t *getFontInfo(FontName name, int size, FontStyle style);

    static void enableFreetype(bool enable);

private:
    static bool kEnableFreetype;
};

// Modern alias
using FontManager = LvFontManager;

} // namespace lvglpp::tools

#endif // LVFONTMANAGER_H
