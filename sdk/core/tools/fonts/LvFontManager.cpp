#include "LvFontManager.h"

#if LV_USE_FREETYPE
#include "core/tools/AssetManager.h"
#include <map>
#include <string>
#include <tuple>
#endif

LV_FONT_DECLARE(NotoSC_20_Normal)
LV_FONT_DECLARE(NotoSC_20_Bold)
LV_FONT_DECLARE(NotoSC_22_Normal)
LV_FONT_DECLARE(NotoSC_22_Bold)
LV_FONT_DECLARE(NotoSC_24_Normal)
LV_FONT_DECLARE(NotoSC_24_Bold)
LV_FONT_DECLARE(NotoSC_28_Normal)
LV_FONT_DECLARE(NotoSC_28_Bold)
LV_FONT_DECLARE(NotoSC_32_Normal)
LV_FONT_DECLARE(NotoSC_32_Bold)
LV_FONT_DECLARE(NotoSC_36_Normal)
LV_FONT_DECLARE(NotoSC_36_Bold)

namespace {

struct BitmapFontEntry
{
    int             size;
    const lv_font_t *normal;
    const lv_font_t *bold;
};

// Compiled-in bitmap fonts are the default. Keep this table in sync with the
// generated files under sdk/core/tools/fonts/.
const BitmapFontEntry kBitmapFonts[] = {
    {20, &NotoSC_20_Normal, &NotoSC_20_Bold},
    {22, &NotoSC_22_Normal, &NotoSC_22_Bold},
    {24, &NotoSC_24_Normal, &NotoSC_24_Bold},
    {28, &NotoSC_28_Normal, &NotoSC_28_Bold},
    {32, &NotoSC_32_Normal, &NotoSC_32_Bold},
    {36, &NotoSC_36_Normal, &NotoSC_36_Bold},
};

const int kBitmapFontCount = static_cast<int>(sizeof(kBitmapFonts) / sizeof(kBitmapFonts[0]));

int bitmapIndexForSize(int size)
{
    for (int i = 0; i < kBitmapFontCount; ++i)
    {
        if (kBitmapFonts[i].size == size)
        {
            return i;
        }
    }

    // Sizes without a dedicated bitmap face fall back to the nearest smaller
    // face, with 48/64 intentionally mapped to 36 Bold by the caller.
    if (size <= kBitmapFonts[0].size)
    {
        return 0;
    }
    for (int i = kBitmapFontCount - 1; i >= 0; --i)
    {
        if (kBitmapFonts[i].size <= size)
        {
            return i;
        }
    }
    return 0;
}

#if LV_USE_FREETYPE

std::string resolveFontPath(const char *filename)
{
    std::string path;
    if (lvglpp::tools::AssetManager::getInstance().materialize(std::string("fonts/") + filename, path))
    {
        return path;
    }
    // Development fallback when assets are staged as plain files.
    return std::string("assets/common/fonts/") + filename;
}

struct FreeTypeFontKey
{
    int                         size;
    lvglpp::tools::LvFontManager::FontStyle style;

    bool operator<(const FreeTypeFontKey &other) const
    {
        return std::tie(size, style) < std::tie(other.size, other.style);
    }
};

std::map<FreeTypeFontKey, lv_font_t *> &freeTypeFontCache()
{
    static std::map<FreeTypeFontKey, lv_font_t *> cache;
    return cache;
}

// Returns nullptr when the face cannot be created; the caller then falls back
// to the compiled-in bitmap font.
const lv_font_t *getFreeTypeFont(int size, lvglpp::tools::LvFontManager::FontStyle style)
{
    if (size <= 0)
    {
        size = 20;
    }

    const FreeTypeFontKey key{size, style};
    auto                 &cache = freeTypeFontCache();
    auto                  it    = cache.find(key);
    if (it != cache.end())
    {
        return it->second;
    }

    const std::string path = resolveFontPath("NotoSansSC-AR-Regular.ttf");
    const auto        renderMode = LV_FREETYPE_FONT_RENDER_MODE_BITMAP;
    const auto        fontStyle  = style == lvglpp::tools::LvFontManager::Bold ? LV_FREETYPE_FONT_STYLE_BOLD
                                                                               : LV_FREETYPE_FONT_STYLE_NORMAL;
    lv_font_t        *font = lv_freetype_font_create(path.c_str(), renderMode, static_cast<uint32_t>(size), fontStyle);

    if (!font)
    {
        LV_LOG_WARN("Failed to create FreeType font '%s' at size %d; using bitmap fonts", path.c_str(), size);
    }
    cache[key] = font;
    return font;
}

#endif // LV_USE_FREETYPE

const lv_font_t *getBitmapFont(int size, lvglpp::tools::LvFontManager::FontStyle style)
{
    const BitmapFontEntry &entry = kBitmapFonts[bitmapIndexForSize(size)];
    return style == lvglpp::tools::LvFontManager::Bold ? entry.bold : entry.normal;
}

} // namespace

namespace lvglpp::tools {

bool LvFontManager::kEnableFreetype = false;

void LvFontManager::enableFreetype(bool enable)
{
#if LV_USE_FREETYPE
    LvFontManager::kEnableFreetype = enable;
#else
    if (enable)
    {
        LV_LOG_WARN("FreeType support is not compiled in; rebuild with -DLVGLPP_ENABLE_FREETYPE=ON");
    }
    LvFontManager::kEnableFreetype = false;
#endif
}

const lv_font_t *LvFontManager::getDefaultFont()
{
    return getFontInfo(SansSC, 20, Normal);
}

const lv_font_t *LvFontManager::getFontInfo(LvFontManager::FontName name, int size, FontStyle style)
{
    (void)name; // Only the SansSC bitmap family is compiled in; DMSans/Oswald alias it.

#if LV_USE_FREETYPE
    if (LvFontManager::kEnableFreetype)
    {
        if (const lv_font_t *font = getFreeTypeFont(size, style))
        {
            return font;
        }
        // Fall through to the bitmap font if FreeType failed.
    }
#endif

    // 48/64 are declared by Label but have no bitmap faces; use 36 Bold.
    if (size == 48 || size == 64)
    {
        return getBitmapFont(36, Bold);
    }
    return getBitmapFont(size, style);
}

} // namespace lvglpp::tools
