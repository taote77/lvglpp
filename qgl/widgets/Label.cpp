#include "Label.h"
#include <algorithm>

namespace QGL {

Label::Label(Widget* parent) : Widget(parent) {
    initializeTextStyle();
    
    // 设置默认属性
    setProperty("text", Variant::fromString(""));
    setProperty("textColor", Variant::fromColor(Color::Black));
    setProperty("fontSize", Variant::fromInt(14));
    setProperty("textAlignment", Variant::fromInt(static_cast<int>(TextAlignment::Left)));
}

Label::Label(const String& text, Widget* parent) : Widget(parent), m_text(text) {
    initializeTextStyle();
    
    // 设置默认属性
    setProperty("text", Variant::fromString(text));
    setProperty("textColor", Variant::fromColor(Color::Black));
    setProperty("fontSize", Variant::fromInt(14));
    setProperty("textAlignment", Variant::fromInt(static_cast<int>(TextAlignment::Left)));
    
    updateText();
}

Label::~Label() {
    cleanupTextStyle();
}

lv_obj_t* Label::createLvglObject(lv_obj_t* parent) {
    return lv_label_create(parent);
}

void Label::setText(const String& text) {
    if (m_text != text) {
        String oldText = m_text;
        m_text = text;
        setProperty("text", Variant::fromString(text));
        
        updateText();
        invalidateTextSize();
        
        textChanged.emit(text);
    }
}

String Label::getText() const {
    return m_text;
}

void Label::setTextAlignment(TextAlignment alignment) {
    if (m_textAlignment != alignment) {
        m_textAlignment = alignment;
        setProperty("textAlignment", Variant::fromInt(static_cast<int>(alignment)));
        updateTextAlignment();
    }
}

TextAlignment Label::getTextAlignment() const {
    return m_textAlignment;
}

void Label::setTextColor(const Color& color) {
    if (m_textColor.rgba() != color.rgba()) {
        m_textColor = color;
        setProperty("textColor", Variant::fromColor(color));
        updateTextStyle();
    }
}

Color Label::getTextColor() const {
    return m_textColor;
}

void Label::setFontSize(int size) {
    if (m_fontSize != size) {
        m_fontSize = size;
        setProperty("fontSize", Variant::fromInt(size));
        updateTextStyle();
        invalidateTextSize();
    }
}

int Label::getFontSize() const {
    return m_fontSize;
}

void Label::setFontFamily(const String& family) {
    if (m_fontFamily != family) {
        m_fontFamily = family;
        setProperty("fontFamily", Variant::fromString(family));
        updateTextStyle();
        invalidateTextSize();
    }
}

String Label::getFontFamily() const {
    return m_fontFamily;
}

void Label::setTextWrapMode(TextWrapMode mode) {
    if (m_wrapMode != mode) {
        m_wrapMode = mode;
        setProperty("textWrapMode", Variant::fromInt(static_cast<int>(mode)));
        updateText();
        invalidateTextSize();
    }
}

TextWrapMode Label::getTextWrapMode() const {
    return m_wrapMode;
}

void Label::setLineSpacing(int spacing) {
    if (m_lineSpacing != spacing) {
        m_lineSpacing = spacing;
        setProperty("lineSpacing", Variant::fromInt(spacing));
        updateTextStyle();
    }
}

int Label::getLineSpacing() const {
    return m_lineSpacing;
}

void Label::setTextMargins(int left, int top, int right, int bottom) {
    Rect newMargins(left, top, right, bottom);
    if (m_textMargins.x != newMargins.x || m_textMargins.y != newMargins.y ||
        m_textMargins.width != newMargins.width || m_textMargins.height != newMargins.height) {
        m_textMargins = newMargins;
        setProperty("textMargins", Variant::fromRect(newMargins));
        updateText();
        invalidateTextSize();
    }
}

Rect Label::getTextMargins() const {
    return m_textMargins;
}

void Label::setMaxLines(int lines) {
    if (m_maxLines != lines) {
        m_maxLines = lines;
        setProperty("maxLines", Variant::fromInt(lines));
        updateText();
        invalidateTextSize();
    }
}

int Label::getMaxLines() const {
    return m_maxLines;
}

void Label::setRichText(bool enabled) {
    if (m_richText != enabled) {
        m_richText = enabled;
        setProperty("richText", Variant::fromBool(enabled));
        updateText();
    }
}

bool Label::isRichText() const {
    return m_richText;
}

void Label::setTextSelectable(bool selectable) {
    if (m_textSelectable != selectable) {
        m_textSelectable = selectable;
        setProperty("textSelectable", Variant::fromBool(selectable));
        
        if (lvglObject()) {
            if (selectable) {
                lv_obj_add_flag(lvglObject(), LV_OBJ_FLAG_CLICKABLE);
            } else {
                lv_obj_clear_flag(lvglObject(), LV_OBJ_FLAG_CLICKABLE);
            }
        }
    }
}

bool Label::isTextSelectable() const {
    return m_textSelectable;
}

Size Label::getTextSize() const {
    if (m_textSizeDirty) {
        m_cachedTextSize = calculateTextSize();
        m_textSizeDirty = false;
    }
    return m_cachedTextSize;
}

Size Label::getIdealSize() const {
    Size textSize = getTextSize();
    return Size(
        textSize.width + m_textMargins.x + m_textMargins.width,
        textSize.height + m_textMargins.y + m_textMargins.height
    );
}

int Label::getLineCount() const {
    if (m_text.empty()) return 0;
    
    // 简化实现：计算换行符数量 + 1
    int lines = 1;
    for (char c : m_text) {
        if (c == '\n') {
            lines++;
        }
    }
    
    if (m_maxLines > 0) {
        lines = std::min(lines, m_maxLines);
    }
    
    return lines;
}

void Label::setOpenExternalLinks(bool open) {
    m_openExternalLinks = open;
    setProperty("openExternalLinks", Variant::fromBool(open));
}

bool Label::getOpenExternalLinks() const {
    return m_openExternalLinks;
}

void Label::paintEvent() {
    Widget::paintEvent();
    // 额外的自定义绘制可以在这里添加
}

void Label::resizeEvent(const Size& newSize, const Size& oldSize) {
    Widget::resizeEvent(newSize, oldSize);
    
    // 当尺寸改变时，可能需要重新计算文本布局
    if (m_wrapMode != TextWrapMode::NoWrap) {
        invalidateTextSize();
        updateText();
    }
}

void Label::onPropertyChanged(const String& name, const Variant& value) {
    Widget::onPropertyChanged(name, value);
    
    if (name == "text") {
        m_text = value.toString();
        updateText();
        invalidateTextSize();
    } else if (name == "textColor") {
        m_textColor = value.getValue<Color>();
        updateTextStyle();
    } else if (name == "fontSize") {
        m_fontSize = value.toInt();
        updateTextStyle();
        invalidateTextSize();
    } else if (name == "textAlignment") {
        m_textAlignment = static_cast<TextAlignment>(value.toInt());
        updateTextAlignment();
    }
}

void Label::updateText() {
    if (!lvglObject()) return;
    
    String displayText = m_text;
    
    // 处理富文本
    if (m_richText) {
        displayText = processRichText(displayText);
    }
    
    // 设置文本到 LVGL 标签
    lv_label_set_text(lvglObject(), displayText.c_str());
    
    // 设置换行模式
    switch (m_wrapMode) {
    case TextWrapMode::NoWrap:
        lv_label_set_long_mode(lvglObject(), LV_LABEL_LONG_CLIP);
        break;
    case TextWrapMode::WordWrap:
        lv_label_set_long_mode(lvglObject(), LV_LABEL_LONG_WRAP);
        break;
    case TextWrapMode::CharWrap:
        lv_label_set_long_mode(lvglObject(), LV_LABEL_LONG_WRAP);
        break;
    case TextWrapMode::Ellipsis:
        lv_label_set_long_mode(lvglObject(), LV_LABEL_LONG_DOT);
        break;
    }
}

void Label::updateTextStyle() {
    if (!lvglObject() || !m_styleInitialized) return;
    
    // 设置文本颜色
    lv_style_set_text_color(&m_textStyle, lv_color_hex(m_textColor.rgba() >> 8));
    
    // 设置字体大小（需要 LVGL 字体支持）
    // 这里使用默认字体，实际应用中需要根据字体大小选择合适的字体
    
    // 设置行间距
    if (m_lineSpacing > 0) {
        lv_style_set_text_line_space(&m_textStyle, m_lineSpacing);
    }
    
    // 应用样式
    lv_obj_add_style(lvglObject(), &m_textStyle, 0);
}

void Label::updateTextAlignment() {
    if (!lvglObject()) return;
    
    lv_text_align_t lvAlign = LV_TEXT_ALIGN_LEFT;
    
    if (static_cast<int>(m_textAlignment) & static_cast<int>(TextAlignment::Right)) {
        lvAlign = LV_TEXT_ALIGN_RIGHT;
    } else if (static_cast<int>(m_textAlignment) & static_cast<int>(TextAlignment::HCenter)) {
        lvAlign = LV_TEXT_ALIGN_CENTER;
    }
    
    lv_obj_set_style_text_align(lvglObject(), lvAlign, 0);
}

String Label::processRichText(const String& text) {
    // 简化的富文本处理
    // 实际实现中可以支持 HTML 标签或自定义标记
    return text;
}

void Label::initializeTextStyle() {
    if (!m_styleInitialized) {
        lv_style_init(&m_textStyle);
        m_styleInitialized = true;
    }
}

void Label::cleanupTextStyle() {
    if (m_styleInitialized) {
        lv_style_reset(&m_textStyle);
        m_styleInitialized = false;
    }
}

Size Label::calculateTextSize() const {
    if (!lvglObject() || m_text.empty()) {
        return Size(0, 0);
    }
    
    // 使用 LVGL 的文本度量功能
    lv_point_t size;
    lv_txt_get_size(&size, m_text.c_str(), lv_obj_get_style_text_font(lvglObject(), 0),
                    lv_obj_get_style_text_letter_space(lvglObject(), 0),
                    lv_obj_get_style_text_line_space(lvglObject(), 0),
                    getWidth() - m_textMargins.x - m_textMargins.width,
                    LV_TEXT_FLAG_NONE);
    
    return Size(size.x, size.y);
}

} // namespace QGL