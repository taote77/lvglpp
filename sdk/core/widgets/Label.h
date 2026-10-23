#ifndef LVGLPP_LABEL_H
#define LVGLPP_LABEL_H

#include "Widget.h"
#include "lvgl.h"
#include <string>

namespace lvglpp::widgets {

class Label : public Widget
{
public:
    enum TextStyle {
        FontSize20,
        FontSize22,
        FontSize24,
        FontSize28,
        FontSize32,
        FontSize36,
        FontSize20Bold,
        FontSize22Bold,
        FontSize24Bold,
        FontSize28Bold,
        FontSize32Bold,
        FontSize36Bold,
        FontSize48Bold,
        FontSize64Bold
    };

    enum FontType { Auto, DMSans, SansSC, Oswald };

    enum LongMode { None, AutoEllipsis, MiddleEllipsis };

    Label(const std::string &strMsg, uint32_t clr, Widget *parent, TextStyle style = TextStyle::FontSize20, FontType type = FontType::Auto);

    ~Label() override;

    /// Set the source text. The displayed text may be truncated by
    /// setMaxLength() or setLongMode(); getText() still returns the original
    /// string.
    void setText(const std::string &strMsg);

    const std::string &getText() const
    {
        return text_;
    }

    void setTextColor(uint32_t clr);

    [[deprecated("use setTextColor()")]]
    void setTextClr(uint32_t clr)
    {
        setTextColor(clr);
    }

    void setTextAlign(lv_text_align_t align);

    void setTextOpacity(lv_opa_t opa);

    [[deprecated("use setTextOpacity()")]]
    void setTextOpa(lv_opa_t opa)
    {
        setTextOpacity(opa);
    }

    /// Enable LVGL recolor markup (#RRGGBB ... #). Automatic truncation is
    /// disabled while recolor is enabled so markup is never cut in half.
    void setRecolorEnabled(bool enable);

    [[deprecated("use setRecolorEnabled()")]]
    void setReColorEnable(bool enable)
    {
        setRecolorEnabled(enable);
    }

    /**
     * Limit the displayed text to n user-perceived characters, inserting
     * "..." in the middle when it does not fit. n <= 0 disables the limit.
     */
    void setMaxLength(int n);

    [[deprecated("use setMaxLength()")]]
    void setMaxLen(int n)
    {
        setMaxLength(n);
    }

    /// Width of the text currently rendered (maximum line width).
    int getDisplayTextWidth() const;

    /// Pixel-width based ellipsis. Applies per line and keeps the rendered
    /// text within maxWidth whenever possible.
    void setLongMode(int maxWidth, LongMode longMode);

    /// Set the native LVGL label long mode (wrap/dots/scroll/...).
    void setLongMode(lv_label_long_mode_t mode);

private:
    void             refreshDisplayText();
    std::string      buildDisplayText(const std::string &text) const;
    std::string      applyCharLimit(const std::string &text, int maxClusters) const;
    std::string      applyPixelLimit(const std::string &text) const;
    std::string      ellipsizeLine(const std::string &line, LongMode mode) const;
    int              measure(const char *text, uint32_t bytes) const;
    int              measureLine(const std::string &line) const;
    int              letterSpace() const;
    const lv_font_t *measurementFont() const;

    std::string      text_;
    int              max_len_         = -1;
    LongMode         long_mode_       = LongMode::None;
    int              long_mode_width_ = 0;
    const lv_font_t *font_            = nullptr;
    bool             recolor_         = false;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_LABEL_H
