#include "core/base/LvObjFactory.h"

#include "Label.h"
#include "core/sys/Application.h"
#include "core/tools/fonts/LvFontManager.h"
#include <algorithm>
#include <cstdint>
#include <src/misc/lv_text_private.h>
#include <src/widgets/label/lv_label.h>
#include <string>
#include <vector>

namespace lvglpp::widgets {

namespace {

struct TextStyleInfo {
    int  size;
    bool bold;
};

// Keep in sync with Label::TextStyle.
constexpr TextStyleInfo kTextStyles[] = {
    {20, false}, {22, false}, {24, false}, {28, false}, {32, false}, {36, false}, {20, true}, {22, true}, {24, true}, {28, true}, {32, true}, {36, true}, {48, true}, {64, true},
};

static_assert(sizeof(kTextStyles) / sizeof(kTextStyles[0]) == Label::TextStyle::FontSize64Bold + 1, "kTextStyles must cover every Label::TextStyle value");

bool isCombiningMark(uint32_t codepoint)
{
    return (codepoint >= 0x0300 && codepoint <= 0x036F) || (codepoint >= 0x0610 && codepoint <= 0x061A) || (codepoint >= 0x064B && codepoint <= 0x065F) || codepoint == 0x0670
           || (codepoint >= 0x06D6 && codepoint <= 0x06DC) || (codepoint >= 0x06DF && codepoint <= 0x06E4) || (codepoint >= 0x06E7 && codepoint <= 0x06E8)
           || (codepoint >= 0x06EA && codepoint <= 0x06ED) || (codepoint >= 0x08D3 && codepoint <= 0x08FF) || (codepoint >= 0x1AB0 && codepoint <= 0x1AFF)
           || (codepoint >= 0x1DC0 && codepoint <= 0x1DFF) || (codepoint >= 0x20D0 && codepoint <= 0x20FF) || (codepoint >= 0xFE20 && codepoint <= 0xFE2F);
}

bool isClusterJoiner(uint32_t codepoint)
{
    // ZWNJ/ZWJ and variation selectors stay attached to the preceding glyph.
    return codepoint == 0x200C || codepoint == 0x200D || (codepoint >= 0xFE00 && codepoint <= 0xFE0F) || (codepoint >= 0xE0100 && codepoint <= 0xE01EF);
}

// Advance past one user-perceived character: a base codepoint plus any
// combining marks/variation selectors that follow it.
void advanceCluster(const char *text, uint32_t &index)
{
    if (text[index] == '\0')
    {
        ++index;
        return;
    }

    lv_text_encoded_next(text, &index);
    while (text[index] != '\0')
    {
        uint32_t       probe = index;
        const uint32_t next  = lv_text_encoded_next(text, &probe);
        if (!isCombiningMark(next) && !isClusterJoiner(next))
        {
            break;
        }
        index = probe;
    }
}

std::vector<uint32_t> clusterOffsets(const std::string &text)
{
    std::vector<uint32_t> offsets;
    uint32_t              index = 0;
    while (index < text.size())
    {
        offsets.push_back(index);
        advanceCluster(text.c_str(), index);
    }
    offsets.push_back(static_cast<uint32_t>(text.size()));
    return offsets;
}

std::vector<std::string> splitLines(const std::string &text)
{
    std::vector<std::string> lines;
    std::size_t              start = 0;
    while (true)
    {
        const std::size_t newline = text.find('\n', start);
        if (newline == std::string::npos)
        {
            lines.push_back(text.substr(start));
            break;
        }
        lines.push_back(text.substr(start, newline - start));
        start = newline + 1;
    }
    return lines;
}

const lv_font_t *fontForStyle(Label::TextStyle style, Label::FontType type)
{
    tools::LvFontManager::FontName fontName = tools::LvFontManager::DMSans;
    switch (type)
    {
    case Label::FontType::Auto:
        fontName = (sys::Application::getLanguageType() == sys::Application::Chinese || sys::Application::getLanguageType() == sys::Application::Arabic) ?
                       tools::LvFontManager::SansSC :
                       tools::LvFontManager::DMSans;
        break;
    case Label::FontType::DMSans:
        fontName = tools::LvFontManager::DMSans;
        break;
    case Label::FontType::SansSC:
        fontName = tools::LvFontManager::SansSC;
        break;
    case Label::FontType::Oswald:
        fontName = tools::LvFontManager::Oswald;
        break;
    }

    const std::size_t index = static_cast<std::size_t>(style);
    if (index >= sizeof(kTextStyles) / sizeof(kTextStyles[0]))
    {
        return tools::LvFontManager::getFontInfo(fontName, 20, tools::LvFontManager::Normal);
    }

    const TextStyleInfo &info = kTextStyles[index];
    return tools::LvFontManager::getFontInfo(fontName, info.size, info.bold ? tools::LvFontManager::Bold : tools::LvFontManager::Normal);
}

} // namespace

Label::Label(const std::string &strMsg, uint32_t clr, Widget *parent, TextStyle style, FontType type) : Widget(tools::LvObjFactory::createLvglLabel, parent), text_(strMsg)
{
    font_ = fontForStyle(style, type);
    if (font_ == nullptr)
    {
        font_ = lv_font_get_default();
    }

    lv_obj_set_style_text_font(lvObj(), font_, LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(lvObj(), LV_TEXT_ALIGN_AUTO, LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(lvObj(), 2, LV_STATE_DEFAULT);
    setTextColor(clr);
    refreshDisplayText();
}

Label::~Label() = default;

void Label::setText(const std::string &strMsg)
{
    text_ = strMsg;
    refreshDisplayText();
}

void Label::setTextColor(uint32_t clr)
{
    if (lvObj() != nullptr)
    {
        lv_obj_set_style_text_color(lvObj(), lv_color_hex(clr), LV_STATE_DEFAULT);
    }
}

void Label::setTextAlign(lv_text_align_t align)
{
    if (lvObj() != nullptr)
    {
        lv_obj_set_style_text_align(lvObj(), align, LV_STATE_DEFAULT);
    }
}

void Label::setTextOpacity(lv_opa_t opa)
{
    if (lvObj() != nullptr)
    {
        lv_obj_set_style_text_opa(lvObj(), opa, 0);
    }
}

void Label::setRecolorEnabled(bool enable)
{
    recolor_ = enable;
    if (lvObj() != nullptr)
    {
        lv_label_set_recolor(lvObj(), enable);
    }
    // Recolor changes how widths are measured, so rebuild the displayed text.
    refreshDisplayText();
}

void Label::setMaxLength(int n)
{
    max_len_ = n;
    refreshDisplayText();
}

int Label::getDisplayTextWidth() const
{
    std::string displayed;
    if (lvObj() != nullptr)
    {
        displayed = lv_label_get_text(lvObj());
    } else
    {
        displayed = buildDisplayText(text_);
    }

    int width = 0;
    for (const std::string &line : splitLines(displayed))
    {
        std::string body = line;
        if (!body.empty() && body.back() == '\r')
        {
            body.pop_back();
        }
        width = std::max(width, measureLine(body));
    }
    return width;
}

void Label::setLongMode(int maxWidth, LongMode longMode)
{
    long_mode_       = longMode;
    long_mode_width_ = maxWidth;
    refreshDisplayText();
}

void Label::setLongMode(lv_label_long_mode_t mode)
{
    if (lvObj() != nullptr)
    {
        lv_label_set_long_mode(lvObj(), mode);
    }
}

void Label::refreshDisplayText()
{
    if (lvObj() == nullptr)
    {
        return;
    }
    const std::string shown = buildDisplayText(text_);
    lv_label_set_text(lvObj(), shown.c_str());
}

std::string Label::buildDisplayText(const std::string &text) const
{
    // Truncating recolor markup would cut #RRGGBB sequences in half. Keep the
    // text intact and let LVGL handle overflow in that case.
    if (recolor_)
    {
        return text;
    }

    std::string shown = applyCharLimit(text, max_len_);
    return applyPixelLimit(shown);
}

std::string Label::applyCharLimit(const std::string &text, int maxClusters) const
{
    if (text.empty() || maxClusters <= 0)
    {
        return text;
    }

    const std::vector<uint32_t> offsets = clusterOffsets(text);
    const int                   count   = static_cast<int>(offsets.size()) - 1;
    if (count <= maxClusters)
    {
        return text;
    }

    if (maxClusters <= 3)
    {
        return text.substr(0, offsets[static_cast<std::size_t>(maxClusters)]);
    }

    const int keep       = maxClusters - 3;
    const int leftCount  = (keep + 1) / 2;
    const int rightCount = keep / 2;

    std::string out = text.substr(0, offsets[static_cast<std::size_t>(leftCount)]);
    out += "...";
    out += text.substr(offsets[static_cast<std::size_t>(count - rightCount)]);
    return out;
}

std::string Label::applyPixelLimit(const std::string &text) const
{
    if (long_mode_ == LongMode::None || long_mode_width_ <= 0 || text.empty())
    {
        return text;
    }

    const std::vector<std::string> lines = splitLines(text);
    std::string                    out;
    for (std::size_t i = 0; i < lines.size(); ++i)
    {
        std::string line = lines[i];
        std::string carriageReturn;
        if (!line.empty() && line.back() == '\r')
        {
            carriageReturn = "\r";
            line.pop_back();
        }

        out += ellipsizeLine(line, long_mode_);
        out += carriageReturn;
        if (i + 1 < lines.size())
        {
            out += '\n';
        }
    }
    return out;
}

std::string Label::ellipsizeLine(const std::string &line, LongMode mode) const
{
    if (line.empty())
    {
        return line;
    }

    const int limit = long_mode_width_;
    if (limit <= 0)
    {
        return std::string();
    }
    if (measureLine(line) <= limit)
    {
        return line;
    }

    static constexpr char kDots[]   = "...";
    const int             dotsWidth = measure(kDots, 3);
    if (dotsWidth > limit)
    {
        return std::string();
    }

    const std::vector<uint32_t> offsets = clusterOffsets(line);
    const int                   count   = static_cast<int>(offsets.size()) - 1;
    const int                   spacing = letterSpace();
    const std::string           dots    = kDots;

    if (mode == LongMode::MiddleEllipsis)
    {
        std::vector<int> prefix(static_cast<std::size_t>(count) + 1, 0);
        std::vector<int> suffix(static_cast<std::size_t>(count) + 1, 0);
        for (int k = 0; k <= count; ++k)
        {
            prefix[static_cast<std::size_t>(k)] = measure(line.c_str(), offsets[static_cast<std::size_t>(k)]);
            suffix[static_cast<std::size_t>(k)] =
                measure(line.c_str() + offsets[static_cast<std::size_t>(count - k)], offsets[static_cast<std::size_t>(count)] - offsets[static_cast<std::size_t>(count - k)]);
        }

        const int budget = limit - dotsWidth;
        int       left   = 0;
        while (left < count && prefix[static_cast<std::size_t>(left + 1)] <= budget / 2)
        {
            ++left;
        }
        int right = 0;
        while (right < count && suffix[static_cast<std::size_t>(right + 1)] <= budget / 2)
        {
            ++right;
        }
        if (left + right > count)
        {
            if (left > count)
            {
                left = count;
            }
            right = count - left;
        }

        const auto fits = [&](int l, int r) {
            return prefix[static_cast<std::size_t>(l)] + suffix[static_cast<std::size_t>(r)] + dotsWidth + (l > 0 ? spacing : 0) + (r > 0 ? spacing : 0) <= limit;
        };

        while (left + right > 0 && !fits(left, right))
        {
            if (left > right)
            {
                --left;
            } else
            {
                --right;
            }
        }

        while (left + right < count)
        {
            bool progress = false;
            if (fits(left + 1, right))
            {
                ++left;
                progress = true;
            }
            if (left + right < count && fits(left, right + 1))
            {
                ++right;
                progress = true;
            }
            if (!progress)
            {
                break;
            }
        }

        return line.substr(0, offsets[static_cast<std::size_t>(left)]) + dots + line.substr(offsets[static_cast<std::size_t>(count - right)]);
    }

    // AutoEllipsis: find the longest prefix that still fits with the dots.
    int low  = 0;
    int high = count;
    int best = 0;
    while (low <= high)
    {
        const int middle = low + (high - low) / 2;
        const int width  = measure(line.c_str(), offsets[static_cast<std::size_t>(middle)]) + dotsWidth + (middle > 0 ? spacing : 0);
        if (width <= limit)
        {
            best = middle;
            low  = middle + 1;
        } else
        {
            high = middle - 1;
        }
    }

    return line.substr(0, offsets[static_cast<std::size_t>(best)]) + dots;
}

int Label::measure(const char *text, uint32_t bytes) const
{
    if (text == nullptr || bytes == 0)
    {
        return 0;
    }

    const lv_text_flag_t flags = recolor_ ? LV_TEXT_FLAG_RECOLOR : LV_TEXT_FLAG_NONE;
    return lv_text_get_width_with_flags(text, bytes, measurementFont(), letterSpace(), flags);
}

int Label::measureLine(const std::string &line) const
{
    return measure(line.c_str(), static_cast<uint32_t>(line.size()));
}

int Label::letterSpace() const
{
    if (lvObj() == nullptr)
    {
        return 0;
    }
    return lv_obj_get_style_text_letter_space(lvObj(), LV_PART_MAIN);
}

const lv_font_t *Label::measurementFont() const
{
    if (font_ != nullptr)
    {
        return font_;
    }
    if (lvObj() != nullptr)
    {
        const lv_font_t *styleFont = lv_obj_get_style_text_font(lvObj(), LV_PART_MAIN);
        if (styleFont != nullptr)
        {
            return styleFont;
        }
    }
    return lv_font_get_default();
}

} // namespace lvglpp::widgets
