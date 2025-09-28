#ifndef QGL_WIDGETS_LABEL_H
#define QGL_WIDGETS_LABEL_H

#include "Widget.h"

namespace QGL {

// 文本对齐方式
enum class TextAlignment {
    Left = 0x01,
    Right = 0x02,
    HCenter = 0x04,
    Top = 0x08,
    Bottom = 0x10,
    VCenter = 0x20,
    Center = HCenter | VCenter
};

// 文本换行模式
enum class TextWrapMode {
    NoWrap,        // 不换行
    WordWrap,      // 按单词换行
    CharWrap,      // 按字符换行
    Ellipsis       // 省略号
};

// Label 控件
class Label : public Widget {
    QGL_OBJECT(Label)
    
public:
    explicit Label(Widget* parent = nullptr);
    explicit Label(const String& text, Widget* parent = nullptr);
    ~Label() override;
    
    // 文本内容
    void setText(const String& text);
    String getText() const;
    
    // 文本对齐
    void setTextAlignment(TextAlignment alignment);
    TextAlignment getTextAlignment() const;
    
    // 文本颜色
    void setTextColor(const Color& color);
    Color getTextColor() const;
    
    // 字体设置
    void setFontSize(int size);
    int getFontSize() const;
    
    void setFontFamily(const String& family);
    String getFontFamily() const;
    
    // 文本换行
    void setTextWrapMode(TextWrapMode mode);
    TextWrapMode getTextWrapMode() const;
    
    // 行间距
    void setLineSpacing(int spacing);
    int getLineSpacing() const;
    
    // 文本边距
    void setTextMargins(int left, int top, int right, int bottom);
    void setTextMargins(int margin) { setTextMargins(margin, margin, margin, margin); }
    Rect getTextMargins() const;
    
    // 多行文本支持
    void setMaxLines(int lines); // -1 为无限制
    int getMaxLines() const;
    
    // 富文本支持
    void setRichText(bool enabled);
    bool isRichText() const;
    
    // 可选择文本
    void setTextSelectable(bool selectable);
    bool isTextSelectable() const;
    
    // 文本度量
    Size getTextSize() const;
    Size getIdealSize() const;
    int getLineCount() const;
    
    // 链接支持
    void setOpenExternalLinks(bool open);
    bool getOpenExternalLinks() const;
    
    // 信号
    SIGNAL(textChanged, const String&);
    SIGNAL(linkActivated, const String&);
    SIGNAL(linkHovered, const String&);
    
protected:
    // Widget 重写
    lv_obj_t* createLvglObject(lv_obj_t* parent) override;
    void paintEvent() override;
    void resizeEvent(const Size& newSize, const Size& oldSize) override;
    
    // 属性变化处理
    void onPropertyChanged(const String& name, const Variant& value) override;
    
    // 文本处理
    void updateText();
    void updateTextStyle();
    void updateTextAlignment();
    String processRichText(const String& text);
    
private:
    String m_text;
    TextAlignment m_textAlignment = TextAlignment::Left;
    Color m_textColor = Color::Black;
    int m_fontSize = 14;
    String m_fontFamily;
    TextWrapMode m_wrapMode = TextWrapMode::WordWrap;
    int m_lineSpacing = 0;
    Rect m_textMargins{0, 0, 0, 0};
    int m_maxLines = -1;
    bool m_richText = false;
    bool m_textSelectable = false;
    bool m_openExternalLinks = false;
    
    // LVGL 样式对象
    lv_style_t m_textStyle;
    bool m_styleInitialized = false;
    
    void initializeTextStyle();
    void cleanupTextStyle();
    
    // 文本度量缓存
    mutable Size m_cachedTextSize;
    mutable bool m_textSizeDirty = true;
    
    Size calculateTextSize() const;
    void invalidateTextSize() { m_textSizeDirty = true; }
};

} // namespace QGL

#endif // QGL_WIDGETS_LABEL_H