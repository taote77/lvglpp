#ifndef QGL_EVENTS_EVENT_H
#define QGL_EVENTS_EVENT_H

#include "../core/Types.h"
#include "../core/Variant.h"
#include "../core/Object.h"
#include <memory>
#include <chrono>

namespace QGL {

// 前向声明
class EventHandler;
class EventFilter;

/**
 * @brief 事件类型枚举
 */
enum class EventType : uint32_t {
    // 系统事件 (0-999)
    None = 0,
    System = 1,
    Application = 2,
    
    // 输入事件 (1000-1999) 
    Input = 1000,
    MousePress = 1001,
    MouseRelease = 1002,
    MouseMove = 1003,
    MouseClick = 1004,
    MouseDoubleClick = 1005,
    KeyPress = 1010,
    KeyRelease = 1011,
    TouchPress = 1020,
    TouchRelease = 1021,
    TouchMove = 1022,
    
    // 界面事件 (2000-2999)
    Widget = 2000,
    Paint = 2001,
    Resize = 2002,
    Move = 2003,
    Show = 2004,
    Hide = 2005,
    Focus = 2006,
    FocusOut = 2007,
    
    // 页面事件 (3000-3999)
    Page = 3000,
    PageCreated = 3001,
    PageInitialized = 3002,
    PageCompleted = 3003,
    PageActivated = 3004,
    PageDeactivated = 3005,
    PageDestroying = 3006,
    PageDestroyed = 3007,
    
    // 应用事件 (4000-4999)
    ApplicationStarted = 4001,
    ApplicationStopping = 4002,
    ApplicationStopped = 4003,
    ApplicationSleep = 4004,
    ApplicationWakeUp = 4005,
    
    // LVGL 事件 (5000-5999)
    LvglEvent = 5000,
    
    // 自定义事件 (10000+)
    Custom = 10000,
    UserDefined = 10001,
    BusinessLogic = 10002,
    DomainSpecific = 10003,
    WorkflowEvent = 10004
};

/**
 * @brief 事件优先级
 */
enum class EventPriority : int {
    Critical = 0,   // 关键事件，最高优先级
    High = 1,       // 高优先级
    Normal = 2,     // 普通优先级（默认）
    Low = 3,        // 低优先级
    Background = 4  // 后台事件，最低优先级
};

/**
 * @brief 事件处理状态
 */
enum class EventHandled {
    NotHandled = 0,    // 未处理
    Handled = 1,       // 已处理
    Consumed = 2       // 已消费（阻止进一步传播）
};

/**
 * @brief 事件基类
 * 
 * 所有事件的基类，提供统一的事件接口和通用功能。
 * 支持自定义事件类型、事件载荷、时间戳等特性。
 */
class Event {
public:
    explicit Event(EventType type = EventType::None);
    explicit Event(EventType type, const Variant& payload);
    virtual ~Event() = default;
    
    // 禁用拷贝，允许移动
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;
    Event(Event&&) = default;
    Event& operator=(Event&&) = default;
    
    // 基本属性
    EventType type() const { return m_type; }
    void setType(EventType type) { m_type = type; }
    
    uint32_t eventId() const { return static_cast<uint32_t>(m_type); }
    
    String eventName() const { return m_eventName; }
    void setEventName(const String& name) { m_eventName = name; }
    
    // 事件载荷
    const Variant& payload() const { return m_payload; }
    void setPayload(const Variant& payload) { m_payload = payload; }
    
    template<typename T>
    T payloadAs() const { return m_payload.getValue<T>(); }
    
    // 时间戳
    uint64_t timestamp() const { return m_timestamp; }
    void setTimestamp(uint64_t timestamp) { m_timestamp = timestamp; }
    
    // 事件源
    Object* source() const { return m_source; }
    void setSource(Object* source) { m_source = source; }
    
    // 优先级
    EventPriority priority() const { return m_priority; }
    void setPriority(EventPriority priority) { m_priority = priority; }
    
    // 处理状态
    EventHandled handled() const { return m_handled; }
    void setHandled(EventHandled handled) { m_handled = handled; }
    
    bool isHandled() const { return m_handled != EventHandled::NotHandled; }
    bool isConsumed() const { return m_handled == EventHandled::Consumed; }
    
    void accept() { m_handled = EventHandled::Handled; }
    void consume() { m_handled = EventHandled::Consumed; }
    void ignore() { m_handled = EventHandled::NotHandled; }
    
    // 事件属性
    void setAttribute(const String& key, const Variant& value);
    Variant getAttribute(const String& key) const;
    bool hasAttribute(const String& key) const;
    void removeAttribute(const String& key);
    void clearAttributes();
    
    // 事件分类检查
    bool isSystemEvent() const { return eventId() < 1000; }
    bool isInputEvent() const { return eventId() >= 1000 && eventId() < 2000; }
    bool isWidgetEvent() const { return eventId() >= 2000 && eventId() < 3000; }
    bool isPageEvent() const { return eventId() >= 3000 && eventId() < 4000; }
    bool isApplicationEvent() const { return eventId() >= 4000 && eventId() < 5000; }
    bool isLvglEvent() const { return eventId() >= 5000 && eventId() < 6000; }
    bool isCustomEvent() const { return eventId() >= 10000; }
    
    // 事件克隆
    virtual std::unique_ptr<Event> clone() const;
    
    // 事件序列化（用于日志、调试等）
    virtual String toString() const;
    virtual std::map<String, Variant> serialize() const;
    virtual void deserialize(const std::map<String, Variant>& data);
    
    // 类型信息
    virtual String className() const { return "Event"; }
    
    // 便捷创建方法
    static std::unique_ptr<Event> create(EventType type);
    static std::unique_ptr<Event> create(EventType type, const Variant& payload);
    
    // 当前时间戳（毫秒）- 改为public
    static uint64_t getCurrentTimestamp();
    
protected:
    // 属性访问
    const std::map<String, Variant>& getAttributes() const { return m_attributes; }

private:
    EventType m_type;
    String m_eventName;
    Variant m_payload;
    uint64_t m_timestamp;
    Object* m_source = nullptr;
    EventPriority m_priority = EventPriority::Normal;
    EventHandled m_handled = EventHandled::NotHandled;
    std::map<String, Variant> m_attributes;
};

/**
 * @brief 系统事件基类
 */
class SystemEvent : public Event {
public:
    explicit SystemEvent(EventType type = EventType::System) 
        : Event(type) {}
    
    String className() const override { return "SystemEvent"; }
};

/**
 * @brief 输入事件基类
 */
class InputEvent : public Event {
public:
    explicit InputEvent(EventType type = EventType::Input) 
        : Event(type) {}
    
    // 输入设备信息
    int deviceId() const { return getAttribute("deviceId").toInt(); }
    void setDeviceId(int id) { setAttribute("deviceId", id); }
    
    String className() const override { return "InputEvent"; }
};

// 前向声明 - 实际定义在各自的头文件中
class MouseEvent;
class KeyEvent;
class FocusEvent;

/**
 * @brief 控件事件
 */
class WidgetEvent : public Event {
public:
    explicit WidgetEvent(EventType type = EventType::Widget)
        : Event(type) {}
    
    String className() const override { return "WidgetEvent"; }
};

/**
 * @brief 页面事件
 */
class PageEvent : public Event {
public:
    explicit PageEvent(EventType type = EventType::Page)
        : Event(type) {}
    
    String pageId() const { return getAttribute("pageId").toString(); }
    void setPageId(const String& id) { setAttribute("pageId", id); }
    
    String className() const override { return "PageEvent"; }
};

/**
 * @brief 应用程序事件
 */
class ApplicationEvent : public Event {
public:
    explicit ApplicationEvent(EventType type = EventType::Application)
        : Event(type) {}
    
    String className() const override { return "ApplicationEvent"; }
};

/**
 * @brief 自定义事件基类
 */
class CustomEvent : public Event {
public:
    explicit CustomEvent(uint32_t customEventId = 10000, const String& eventName = String())
        : Event(static_cast<EventType>(customEventId)) {
        if (!eventName.empty()) {
            setEventName(eventName);
        }
    }
    
    explicit CustomEvent(const String& eventName, const Variant& payload = Variant())
        : Event(EventType::Custom, payload) {
        setEventName(eventName);
    }
    
    uint32_t customEventId() const { return eventId(); }
    
    String className() const override { return "CustomEvent"; }
};

// 便捷宏定义
#define QGL_DEFINE_EVENT(EventClass, eventId, eventName) \
class EventClass : public QGL::CustomEvent { \
public: \
    EventClass() : CustomEvent(eventId, eventName) {} \
    explicit EventClass(const QGL::Variant& payload) : CustomEvent(eventId, eventName) { setPayload(payload); } \
    QGL::String className() const override { return #EventClass; } \
}

#define QGL_DEFINE_COMPLEX_EVENT(EventClass, eventId, eventName) \
class EventClass : public QGL::CustomEvent { \
    QGL_OBJECT(EventClass) \
public: \
    EventClass() : CustomEvent(eventId, eventName) {}

} // namespace QGL

#endif // QGL_EVENTS_EVENT_H