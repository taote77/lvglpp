
#ifndef LVFONTMANAGER_H
#define LVFONTMANAGER_H

#include "FontProfile.h"

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

    /**
     * Select a generated font profile (gen_fonts_profile.py --emit-table).
     * The profile takes precedence over the compiled-in desktop table.
     * Pass (nullptr, 0) to restore the default table.
     */
    static void setProfile(const LvglppFontFace *faces, unsigned count);

    /// Default inherited font for widgets that do not use Label (roller,
    /// dropdown, button internals, ...). Covers Arabic presentation forms.
    static const lv_font_t *getDefaultFont();

    static void enableFreetype(bool enable);

private:
    static bool kEnableFreetype;
};

// Modern alias
using FontManager = LvFontManager;

} // namespace lvglpp::tools

#endif // LVFONTMANAGER_H
