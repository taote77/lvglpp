#ifndef QGL_EVENTS_MOUSEEVENT_H
#define QGL_EVENTS_MOUSEEVENT_H

#include "Event.h"
#include "../core/Types.h"

namespace QGL {

/**
 * @brief 鼠标按钮枚举
 */
enum class MouseButton : int {
    NoButton = 0,      // 无按钮
    LeftButton = 1,    // 左键
    RightButton = 2,   // 右键
    MiddleButton = 4,  // 中键
    BackButton = 8,    // 后退键
    ForwardButton = 16 // 前进键
};

/**
 * @brief 修饰键枚举
 */
enum class ModifierKey : int {
    NoModifier = 0,      // 无修饰键
    ShiftModifier = 1,   // Shift键
    ControlModifier = 2, // Ctrl键
    AltModifier = 4,     // Alt键
    MetaModifier = 8     // Meta键（Windows键/Cmd键）
};

// 位运算支持
inline MouseButton operator|(MouseButton a, MouseButton b) {
    return static_cast<MouseButton>(static_cast<int>(a) | static_cast<int>(b));
}

inline MouseButton operator&(MouseButton a, MouseButton b) {
    return static_cast<MouseButton>(static_cast<int>(a) & static_cast<int>(b));
}

inline ModifierKey operator|(ModifierKey a, ModifierKey b) {
    return static_cast<ModifierKey>(static_cast<int>(a) | static_cast<int>(b));
}

inline ModifierKey operator&(ModifierKey a, ModifierKey b) {
    return static_cast<ModifierKey>(static_cast<int>(a) & static_cast<int>(b));
}

/**
 * @brief 鼠标事件类
 * 
 * 处理鼠标相关的输入事件，包括点击、移动、拖拽等操作。
 * 提供位置信息、按钮状态、修饰键状态等详细信息。
 */
class MouseEvent : public Event {
public:
    /**
     * @brief 构造鼠标事件
     * @param type 事件类型
     * @param position 鼠标位置
     * @param button 鼠标按钮
     * @param modifiers 修饰键状态
     */
    MouseEvent(EventType type, const Point& position, MouseButton button = MouseButton::NoButton, 
               ModifierKey modifiers = ModifierKey::NoModifier);
    
    /**
     * @brief 构造鼠标移动事件
     * @param position 当前位置
     * @param lastPosition 上次位置
     * @param modifiers 修饰键状态
     */
    MouseEvent(const Point& position, const Point& lastPosition, ModifierKey modifiers = ModifierKey::NoModifier);
    
    virtual ~MouseEvent() = default;
    
    // 基本属性
    Point position() const { return m_position; }
    void setPosition(const Point& position) { m_position = position; }
    
    Point lastPosition() const { return m_lastPosition; }
    void setLastPosition(const Point& lastPosition) { m_lastPosition = lastPosition; }
    
    MouseButton button() const { return m_button; }
    void setButton(MouseButton button) { m_button = button; }
    
    MouseButton buttons() const { return m_buttons; }
    void setButtons(MouseButton buttons) { m_buttons = buttons; }
    
    ModifierKey modifiers() const { return m_modifiers; }
    void setModifiers(ModifierKey modifiers) { m_modifiers = modifiers; }
    
    // 位置相关方法
    int x() const { return m_position.x; }
    int y() const { return m_position.y; }
    
    Point globalPosition() const { return m_globalPosition; }
    void setGlobalPosition(const Point& globalPos) { m_globalPosition = globalPos; }
    
    int globalX() const { return m_globalPosition.x; }
    int globalY() const { return m_globalPosition.y; }
    
    // 移动相关
    Point delta() const { return m_position - m_lastPosition; }
    int deltaX() const { return m_position.x - m_lastPosition.x; }
    int deltaY() const { return m_position.y - m_lastPosition.y; }
    
    double distance() const;
    double angle() const;
    
    // 按钮状态检查
    bool hasButton(MouseButton button) const {
        return (static_cast<int>(m_buttons) & static_cast<int>(button)) != 0;
    }
    
    bool isLeftButton() const { return hasButton(MouseButton::LeftButton); }
    bool isRightButton() const { return hasButton(MouseButton::RightButton); }
    bool isMiddleButton() const { return hasButton(MouseButton::MiddleButton); }
    
    // 修饰键状态检查
    bool hasModifier(ModifierKey modifier) const {
        return (static_cast<int>(m_modifiers) & static_cast<int>(modifier)) != 0;
    }
    
    bool shiftPressed() const { return hasModifier(ModifierKey::ShiftModifier); }
    bool ctrlPressed() const { return hasModifier(ModifierKey::ControlModifier); }
    bool altPressed() const { return hasModifier(ModifierKey::AltModifier); }
    bool metaPressed() const { return hasModifier(ModifierKey::MetaModifier); }
    
    // 点击信息
    int clickCount() const { return m_clickCount; }
    void setClickCount(int count) { m_clickCount = count; }
    
    uint64_t clickTime() const { return m_clickTime; }
    void setClickTime(uint64_t time) { m_clickTime = time; }
    
    // 拖拽支持
    bool isDragEvent() const { return m_isDragEvent; }
    void setDragEvent(bool isDrag) { m_isDragEvent = isDrag; }
    
    Point dragStartPosition() const { return m_dragStartPosition; }
    void setDragStartPosition(const Point& startPos) { m_dragStartPosition = startPos; }
    
    // 滚轮支持（对于滚轮事件）
    Point wheelDelta() const { return m_wheelDelta; }
    void setWheelDelta(const Point& delta) { m_wheelDelta = delta; }
    
    int wheelDeltaX() const { return m_wheelDelta.x; }
    int wheelDeltaY() const { return m_wheelDelta.y; }
    
    // 触摸支持（对于触摸转换的鼠标事件）
    bool isTouchEvent() const { return m_isTouchEvent; }
    void setTouchEvent(bool isTouch) { m_isTouchEvent = isTouch; }
    
    int touchId() const { return m_touchId; }
    void setTouchId(int id) { m_touchId = id; }
    
    // 压力感应（对于支持压力的输入设备）
    float pressure() const { return m_pressure; }
    void setPressure(float pressure) { m_pressure = pressure; }
    
    // 倾斜角度（对于数位笔等设备）
    Point tilt() const { return m_tilt; }
    void setTilt(const Point& tilt) { m_tilt = tilt; }
    
    // 事件创建便捷方法
    static std::unique_ptr<MouseEvent> createPressEvent(const Point& pos, MouseButton button, ModifierKey modifiers = ModifierKey::NoModifier);
    static std::unique_ptr<MouseEvent> createReleaseEvent(const Point& pos, MouseButton button, ModifierKey modifiers = ModifierKey::NoModifier);
    static std::unique_ptr<MouseEvent> createClickEvent(const Point& pos, MouseButton button, int clickCount = 1, ModifierKey modifiers = ModifierKey::NoModifier);
    static std::unique_ptr<MouseEvent> createDoubleClickEvent(const Point& pos, MouseButton button, ModifierKey modifiers = ModifierKey::NoModifier);
    static std::unique_ptr<MouseEvent> createMoveEvent(const Point& pos, const Point& lastPos, MouseButton buttons = MouseButton::NoButton, ModifierKey modifiers = ModifierKey::NoModifier);
    static std::unique_ptr<MouseEvent> createWheelEvent(const Point& pos, const Point& wheelDelta, ModifierKey modifiers = ModifierKey::NoModifier);
    static std::unique_ptr<MouseEvent> createDragEvent(const Point& pos, const Point& startPos, MouseButton button, ModifierKey modifiers = ModifierKey::NoModifier);
    
    // 重写基类方法
    std::unique_ptr<Event> clone() const override;
    String toString() const override;
    std::map<String, Variant> serialize() const override;
    void deserialize(const std::map<String, Variant>& data) override;
    String className() const override { return "MouseEvent"; }

private:
    Point m_position;           // 当前位置
    Point m_lastPosition;       // 上次位置
    Point m_globalPosition;     // 全局位置
    MouseButton m_button;       // 触发事件的按钮
    MouseButton m_buttons;      // 当前按下的所有按钮
    ModifierKey m_modifiers;    // 修饰键状态
    
    int m_clickCount = 0;       // 点击次数
    uint64_t m_clickTime = 0;   // 点击时间
    
    bool m_isDragEvent = false; // 是否为拖拽事件
    Point m_dragStartPosition;  // 拖拽开始位置
    
    Point m_wheelDelta;         // 滚轮增量
    
    bool m_isTouchEvent = false; // 是否为触摸转换事件
    int m_touchId = -1;         // 触摸ID
    
    float m_pressure = 1.0f;    // 压力值
    Point m_tilt;               // 倾斜角度
};

/**
 * @brief 鼠标按压事件
 */
class MousePressEvent : public MouseEvent {
public:
    MousePressEvent(const Point& position, MouseButton button, ModifierKey modifiers = ModifierKey::NoModifier)
        : MouseEvent(EventType::MousePress, position, button, modifiers) {}
        
    String className() const override { return "MousePressEvent"; }
};

/**
 * @brief 鼠标释放事件  
 */
class MouseReleaseEvent : public MouseEvent {
public:
    MouseReleaseEvent(const Point& position, MouseButton button, ModifierKey modifiers = ModifierKey::NoModifier)
        : MouseEvent(EventType::MouseRelease, position, button, modifiers) {}
        
    String className() const override { return "MouseReleaseEvent"; }
};

/**
 * @brief 鼠标移动事件
 */
class MouseMoveEvent : public MouseEvent {
public:
    MouseMoveEvent(const Point& position, const Point& lastPosition, MouseButton buttons = MouseButton::NoButton, ModifierKey modifiers = ModifierKey::NoModifier)
        : MouseEvent(EventType::MouseMove, position, MouseButton::NoButton, modifiers) {
        setLastPosition(lastPosition);
        setButtons(buttons);
    }
    
    String className() const override { return "MouseMoveEvent"; }
};

/**
 * @brief 鼠标点击事件
 */
class MouseClickEvent : public MouseEvent {
public:
    MouseClickEvent(const Point& position, MouseButton button, int clickCount = 1, ModifierKey modifiers = ModifierKey::NoModifier)
        : MouseEvent(EventType::MouseClick, position, button, modifiers) {
        setClickCount(clickCount);
    }
    
    String className() const override { return "MouseClickEvent"; }
};

/**
 * @brief 鼠标双击事件
 */
class MouseDoubleClickEvent : public MouseEvent {
public:
    MouseDoubleClickEvent(const Point& position, MouseButton button, ModifierKey modifiers = ModifierKey::NoModifier)
        : MouseEvent(EventType::MouseDoubleClick, position, button, modifiers) {
        setClickCount(2);
    }
    
    String className() const override { return "MouseDoubleClickEvent"; }
};

/**
 * @brief 鼠标滚轮事件
 */
class WheelEvent : public MouseEvent {
public:
    WheelEvent(const Point& position, const Point& wheelDelta, ModifierKey modifiers = ModifierKey::NoModifier)
        : MouseEvent(EventType::Input, position, MouseButton::NoButton, modifiers) {
        setWheelDelta(wheelDelta);
    }
    
    String className() const override { return "WheelEvent"; }
};

} // namespace QGL

#endif // QGL_EVENTS_MOUSEEVENT_H