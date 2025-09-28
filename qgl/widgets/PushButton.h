#ifndef QGL_WIDGETS_PUSHBUTTON_H
#define QGL_WIDGETS_PUSHBUTTON_H

#include "Widget.h"

namespace QGL {

// 按钮状态
enum class ButtonState {
    Normal,    // 正常状态
    Hovered,   // 悬停状态
    Pressed,   // 按下状态
    Disabled,  // 禁用状态
    Focused    // 聚焦状态
};

// 按钮样式类型
enum class ButtonStyle {
    Default,   // 默认样式
    Flat,      // 扁平样式
    Outlined,  // 轮廓样式
    Raised,    // 浮起样式
    Text       // 文本样式
};

// PushButton 控件
class PushButton : public Widget {
    QGL_OBJECT(PushButton)
    
public:
    explicit PushButton(Widget* parent = nullptr);
    explicit PushButton(const String& text, Widget* parent = nullptr);
    ~PushButton() override;
    
    // 文本内容
    void setText(const String& text);
    String getText() const;
    
    // 图标设置
    void setIcon(const String& iconPath);
    String getIcon() const;
    void setIconSize(const Size& size);
    Size getIconSize() const;
    
    // 按钮样式
    void setButtonStyle(ButtonStyle style);
    ButtonStyle getButtonStyle() const;
    
    // 按钮状态
    ButtonState getButtonState() const;
    void setCheckable(bool checkable);
    bool isCheckable() const;
    void setChecked(bool checked);
    bool isChecked() const;
    
    // 扁平样式
    void setFlat(bool flat);
    bool isFlat() const;
    
    // 默认按钮
    void setDefault(bool isDefault);
    bool isDefault() const;
    
    // 自动重复点击
    void setAutoRepeat(bool autoRepeat);
    bool getAutoRepeat() const;
    void setAutoRepeatDelay(int delay);
    int getAutoRepeatDelay() const;
    void setAutoRepeatInterval(int interval);
    int getAutoRepeatInterval() const;
    
    // 按钮分组（用于单选）
    void setButtonGroup(const String& groupName);
    String getButtonGroup() const;
    
    // 快捷键
    void setShortcut(const String& shortcut);
    String getShortcut() const;
    
    // 颜色设置
    void setTextColor(const Color& color);
    Color getTextColor() const;
    void setTextColorPressed(const Color& color);
    Color getTextColorPressed() const;
    void setTextColorDisabled(const Color& color);
    Color getTextColorDisabled() const;
    
    void setBackgroundColorNormal(const Color& color);
    Color getBackgroundColorNormal() const;
    void setBackgroundColorHovered(const Color& color);
    Color getBackgroundColorHovered() const;
    void setBackgroundColorPressed(const Color& color);
    Color getBackgroundColorPressed() const;
    void setBackgroundColorDisabled(const Color& color);
    Color getBackgroundColorDisabled() const;
    
    // 边框设置
    void setBorderColorNormal(const Color& color);
    Color getBorderColorNormal() const;
    void setBorderColorHovered(const Color& color);
    Color getBorderColorHovered() const;
    void setBorderColorPressed(const Color& color);
    Color getBorderColorPressed() const;
    
    // 阴影效果
    void setShadowEnabled(bool enabled);
    bool isShadowEnabled() const;
    void setShadowColor(const Color& color);
    Color getShadowColor() const;
    void setShadowOffset(const Point& offset);
    Point getShadowOffset() const;
    void setShadowBlur(int blur);
    int getShadowBlur() const;
    
    // 动画效果
    void setAnimationEnabled(bool enabled);
    bool isAnimationEnabled() const;
    void setAnimationDuration(int duration);
    int getAnimationDuration() const;
    
    // 便捷方法
    void click(); // 程序化点击
    
    // 信号
    SIGNAL(clicked);
    SIGNAL(pressed);
    SIGNAL(released);
    SIGNAL(toggled, bool);
    SIGNAL(hovered);
    SIGNAL(stateChanged, ButtonState);
    
protected:
    // Widget 重写
    lv_obj_t* createLvglObject(lv_obj_t* parent) override;
    void paintEvent() override;
    void resizeEvent(const Size& newSize, const Size& oldSize) override;
    
    // 事件处理
    void mouseClickEvent(const Point& pos) override;
    void mousePressEvent(const Point& pos) override;
    void mouseReleaseEvent(const Point& pos) override;
    void mouseMoveEvent(const Point& pos) override;
    void focusInEvent() override;
    void focusOutEvent() override;
    
    // 属性变化处理
    void onPropertyChanged(const String& name, const Variant& value) override;
    
private:
    String m_text;
    String m_iconPath;
    Size m_iconSize{16, 16};
    ButtonStyle m_buttonStyle = ButtonStyle::Default;
    ButtonState m_buttonState = ButtonState::Normal;
    bool m_checkable = false;
    bool m_checked = false;
    bool m_flat = false;
    bool m_isDefault = false;
    bool m_autoRepeat = false;
    int m_autoRepeatDelay = 300;
    int m_autoRepeatInterval = 100;
    String m_buttonGroup;
    String m_shortcut;
    
    // 颜色设置
    Color m_textColor = Color::Black;
    Color m_textColorPressed = Color::Black;
    Color m_textColorDisabled = Color("#808080");
    
    Color m_backgroundColorNormal = Color("#f0f0f0");
    Color m_backgroundColorHovered = Color("#e0e0e0");
    Color m_backgroundColorPressed = Color("#d0d0d0");
    Color m_backgroundColorDisabled = Color("#f8f8f8");
    
    Color m_borderColorNormal = Color("#c0c0c0");
    Color m_borderColorHovered = Color("#a0a0a0");
    Color m_borderColorPressed = Color("#808080");
    
    // 阴影效果
    bool m_shadowEnabled = false;
    Color m_shadowColor = Color(0x00000080); // 半透明黑色
    Point m_shadowOffset{2, 2};
    int m_shadowBlur = 4;
    
    // 动画效果
    bool m_animationEnabled = true;
    int m_animationDuration = 150;
    
    // LVGL 样式对象
    lv_style_t m_normalStyle;
    lv_style_t m_pressedStyle;
    lv_style_t m_disabledStyle;
    bool m_stylesInitialized = false;
    
    // 自动重复定时器
    class Timer* m_autoRepeatTimer = nullptr;
    
    // 内部方法
    void initializeStyles();
    void cleanupStyles();
    void updateButtonState(ButtonState newState);
    void updateButtonStyle();
    void applyStateStyle();
    void createAutoRepeatTimer();
    void destroyAutoRepeatTimer();
    void handleAutoRepeat();
    
    // 按钮分组管理
    static std::map<String, std::vector<PushButton*>> s_buttonGroups;
    void addToButtonGroup();
    void removeFromButtonGroup();
    void uncheckOthersInGroup();
    
    // 动画相关
    void startClickAnimation();
    void animateToState(ButtonState targetState);
};

} // namespace QGL

#endif // QGL_WIDGETS_PUSHBUTTON_H