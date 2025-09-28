#ifndef QGL_EVENTS_FOCUSEVENT_H
#define QGL_EVENTS_FOCUSEVENT_H

#include "Event.h"
#include "../core/Types.h"

namespace QGL {

// 前向声明
class Widget;

/**
 * @brief 焦点变化原因枚举
 */
enum class FocusReason {
    Mouse,          // 鼠标点击
    Tab,            // Tab键导航
    Backtab,        // Shift+Tab键导航
    ActiveWindow,   // 窗口激活
    Popup,          // 弹出窗口
    Shortcut,       // 快捷键
    MenuBar,        // 菜单栏
    Other,          // 其他原因
    NoFocusReason   // 无特定原因
};

/**
 * @brief 焦点事件类
 * 
 * 处理组件焦点获取和失去的事件，提供焦点变化的详细信息。
 * 包括获得焦点的组件、失去焦点的组件以及焦点变化的原因。
 */
class FocusEvent : public Event {
public:
    /**
     * @brief 构造焦点事件
     * @param type 事件类型（通常是 Focus 或 FocusOut）
     * @param oldFocus 之前拥有焦点的组件
     * @param newFocus 新获得焦点的组件
     * @param reason 焦点变化原因
     */
    FocusEvent(EventType type, Widget* oldFocus, Widget* newFocus, FocusReason reason = FocusReason::Other);
    
    /**
     * @brief 构造焦点获得事件
     * @param newFocus 获得焦点的组件
     * @param reason 焦点变化原因
     */
    FocusEvent(Widget* newFocus, FocusReason reason = FocusReason::Other);
    
    virtual ~FocusEvent() = default;
    
    // 基本属性
    Widget* oldFocusWidget() const { return m_oldFocusWidget; }
    void setOldFocusWidget(Widget* widget) { m_oldFocusWidget = widget; }
    
    Widget* newFocusWidget() const { return m_newFocusWidget; }
    void setNewFocusWidget(Widget* widget) { m_newFocusWidget = widget; }
    
    FocusReason reason() const { return m_reason; }
    void setReason(FocusReason reason) { m_reason = reason; }
    
    // 焦点方向
    bool gotFocus() const { return type() == EventType::Focus; }
    bool lostFocus() const { return type() == EventType::FocusOut; }
    
    // 焦点变化检查
    bool hasFocusChange() const { return m_oldFocusWidget != m_newFocusWidget; }
    bool isTemporary() const { return m_isTemporary; }
    void setTemporary(bool temporary) { m_isTemporary = temporary; }
    
    // 焦点导航信息
    bool isTabFocus() const { return m_reason == FocusReason::Tab || m_reason == FocusReason::Backtab; }
    bool isMouseFocus() const { return m_reason == FocusReason::Mouse; }
    bool isKeyboardFocus() const { return isTabFocus() || m_reason == FocusReason::Shortcut; }
    
    // 导航方向
    bool isForwardTab() const { return m_reason == FocusReason::Tab; }
    bool isBackwardTab() const { return m_reason == FocusReason::Backtab; }
    
    // 焦点策略支持
    enum class FocusPolicy {
        NoFocus,        // 不接受焦点
        TabFocus,       // 仅通过Tab键接受焦点
        ClickFocus,     // 仅通过鼠标点击接受焦点
        StrongFocus,    // 接受Tab键和鼠标点击焦点
        WheelFocus      // 接受滚轮焦点
    };
    
    FocusPolicy focusPolicy() const { return m_focusPolicy; }
    void setFocusPolicy(FocusPolicy policy) { m_focusPolicy = policy; }
    
    // 焦点链信息
    Widget* nextInFocusChain() const { return m_nextInFocusChain; }
    void setNextInFocusChain(Widget* widget) { m_nextInFocusChain = widget; }
    
    Widget* previousInFocusChain() const { return m_previousInFocusChain; }
    void setPreviousInFocusChain(Widget* widget) { m_previousInFocusChain = widget; }
    
    // 焦点组信息
    String focusGroup() const { return m_focusGroup; }
    void setFocusGroup(const String& group) { m_focusGroup = group; }
    
    int focusOrder() const { return m_focusOrder; }
    void setFocusOrder(int order) { m_focusOrder = order; }
    
    // 焦点区域信息（用于部分焦点）
    Rect focusRect() const { return m_focusRect; }
    void setFocusRect(const Rect& rect) { m_focusRect = rect; }
    
    bool hasPartialFocus() const { return m_hasPartialFocus; }
    void setPartialFocus(bool partial) { m_hasPartialFocus = partial; }
    
    // 焦点上下文
    String focusContext() const { return m_focusContext; }
    void setFocusContext(const String& context) { m_focusContext = context; }
    
    // 焦点提示信息
    String focusHint() const { return m_focusHint; }
    void setFocusHint(const String& hint) { m_focusHint = hint; }
    
    // 焦点可见性
    bool shouldShowFocusIndicator() const { return m_showFocusIndicator; }
    void setShowFocusIndicator(bool show) { m_showFocusIndicator = show; }
    
    // 辅助功能支持
    String accessibilityText() const { return m_accessibilityText; }
    void setAccessibilityText(const String& text) { m_accessibilityText = text; }
    
    String accessibilityRole() const { return m_accessibilityRole; }
    void setAccessibilityRole(const String& role) { m_accessibilityRole = role; }
    
    // 事件创建便捷方法
    static std::unique_ptr<FocusEvent> createFocusInEvent(Widget* widget, FocusReason reason = FocusReason::Other);
    static std::unique_ptr<FocusEvent> createFocusOutEvent(Widget* widget, FocusReason reason = FocusReason::Other);
    static std::unique_ptr<FocusEvent> createFocusChangeEvent(Widget* oldFocus, Widget* newFocus, FocusReason reason = FocusReason::Other);
    static std::unique_ptr<FocusEvent> createTabFocusEvent(Widget* widget, bool forward = true);
    static std::unique_ptr<FocusEvent> createMouseFocusEvent(Widget* widget, const Point& clickPos);
    
    // 焦点原因转换
    static String focusReasonToString(FocusReason reason);
    static FocusReason stringToFocusReason(const String& reasonString);
    
    // 重写基类方法
    std::unique_ptr<Event> clone() const override;
    String toString() const override;
    std::map<String, Variant> serialize() const override;
    void deserialize(const std::map<String, Variant>& data) override;
    String className() const override { return "FocusEvent"; }

private:
    Widget* m_oldFocusWidget;       // 之前拥有焦点的组件
    Widget* m_newFocusWidget;       // 新获得焦点的组件
    FocusReason m_reason;           // 焦点变化原因
    
    bool m_isTemporary;             // 是否为临时焦点变化
    FocusPolicy m_focusPolicy;      // 焦点策略
    
    // 焦点链
    Widget* m_nextInFocusChain;     // 焦点链中的下一个组件
    Widget* m_previousInFocusChain; // 焦点链中的上一个组件
    
    // 焦点组
    String m_focusGroup;            // 焦点组名称
    int m_focusOrder;               // 焦点顺序
    
    // 焦点区域
    Rect m_focusRect;               // 焦点矩形区域
    bool m_hasPartialFocus;         // 是否有部分焦点
    
    // 焦点上下文
    String m_focusContext;          // 焦点上下文
    String m_focusHint;             // 焦点提示信息
    bool m_showFocusIndicator;      // 是否显示焦点指示器
    
    // 辅助功能
    String m_accessibilityText;     // 辅助功能文本
    String m_accessibilityRole;     // 辅助功能角色
};

/**
 * @brief 焦点获得事件
 */
class FocusInEvent : public FocusEvent {
public:
    FocusInEvent(Widget* widget, FocusReason reason = FocusReason::Other)
        : FocusEvent(EventType::Focus, nullptr, widget, reason) {}
        
    String className() const override { return "FocusInEvent"; }
};

/**
 * @brief 焦点失去事件
 */
class FocusOutEvent : public FocusEvent {
public:
    FocusOutEvent(Widget* widget, FocusReason reason = FocusReason::Other)
        : FocusEvent(EventType::FocusOut, widget, nullptr, reason) {}
        
    String className() const override { return "FocusOutEvent"; }
};

/**
 * @brief 焦点变化事件
 */
class FocusChangeEvent : public FocusEvent {
public:
    FocusChangeEvent(Widget* oldFocus, Widget* newFocus, FocusReason reason = FocusReason::Other)
        : FocusEvent(EventType::Focus, oldFocus, newFocus, reason) {}
        
    String className() const override { return "FocusChangeEvent"; }
};

} // namespace QGL

#endif // QGL_EVENTS_FOCUSEVENT_H