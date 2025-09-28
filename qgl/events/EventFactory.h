#ifndef QGL_EVENTS_EVENT_FACTORY_H
#define QGL_EVENTS_EVENT_FACTORY_H

#include "Event.h"
#include <functional>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace QGL {

/**
 * @brief 事件创建器函数类型
 */
using EventCreator = std::function<std::unique_ptr<Event>(const Variant& data)>;

/**
 * @brief 事件注册表
 * 
 * 管理自定义事件类型的注册和查找，支持运行时事件类型注册。
 */
class EventRegistry {
public:
    // 获取单例实例
    static EventRegistry* instance();
    
    // 注册事件类型
    template<typename EventT>
    bool registerEvent(const String& eventName, uint32_t eventId = 0);
    
    // 注册事件创建器
    bool registerEvent(const String& eventName, uint32_t eventId, EventCreator creator);
    
    // 取消注册事件
    bool unregisterEvent(const String& eventName);
    bool unregisterEvent(uint32_t eventId);
    
    // 检查事件是否已注册
    bool isRegistered(const String& eventName) const;
    bool isRegistered(uint32_t eventId) const;
    
    // 获取事件信息
    uint32_t getEventId(const String& eventName) const;
    String getEventName(uint32_t eventId) const;
    EventCreator getEventCreator(const String& eventName) const;
    EventCreator getEventCreator(uint32_t eventId) const;
    
    // 获取所有注册的事件
    std::vector<String> getAllEventNames() const;
    std::vector<uint32_t> getAllEventIds() const;
    
    // 清除所有注册
    void clearAll();
    
    // 获取下一个可用的自定义事件 ID
    uint32_t getNextCustomEventId() const;

private:
    EventRegistry() = default;
    ~EventRegistry() = default;
    
    struct EventInfo {
        String name;
        uint32_t id;
        EventCreator creator;
        
        EventInfo() = default;
        EventInfo(const String& n, uint32_t i, EventCreator c)
            : name(n), id(i), creator(std::move(c)) {}
    };
    
    mutable std::mutex m_mutex;
    std::unordered_map<String, EventInfo> m_nameToInfo;
    std::unordered_map<uint32_t, EventInfo> m_idToInfo;
    
    static EventRegistry* s_instance;
    static std::mutex s_instanceMutex;
};

/**
 * @brief 事件工厂
 * 
 * 负责创建各种类型的事件对象，支持基础事件类型和自定义事件类型。
 */
class EventFactory {
public:
    // 创建基础事件
    static std::unique_ptr<Event> createEvent(EventType type);
    static std::unique_ptr<Event> createEvent(EventType type, const Variant& payload);
    
    // 通过名称创建事件
    static std::unique_ptr<Event> createEvent(const String& eventName);
    static std::unique_ptr<Event> createEvent(const String& eventName, const Variant& data);
    
    // 通过事件 ID 创建事件
    static std::unique_ptr<Event> createEvent(uint32_t eventId);
    static std::unique_ptr<Event> createEvent(uint32_t eventId, const Variant& data);
    
    // 创建特定类型的事件
    template<typename EventT, typename... Args>
    static std::unique_ptr<EventT> createTypedEvent(Args&&... args);
    
    // 创建输入事件
    static std::unique_ptr<MouseEvent> createMouseEvent(EventType type, const Point& pos, int button = 0);
    static std::unique_ptr<KeyEvent> createKeyEvent(EventType type, int key, const String& text = String());
    
    // 创建系统事件
    static std::unique_ptr<SystemEvent> createSystemEvent(EventType type);
    static std::unique_ptr<ApplicationEvent> createApplicationEvent(EventType type);
    static std::unique_ptr<PageEvent> createPageEvent(EventType type, const String& pageId = String());
    static std::unique_ptr<WidgetEvent> createWidgetEvent(EventType type);
    
    // 创建自定义事件
    static std::unique_ptr<CustomEvent> createCustomEvent(const String& eventName);
    static std::unique_ptr<CustomEvent> createCustomEvent(const String& eventName, const Variant& payload);
    static std::unique_ptr<CustomEvent> createCustomEvent(uint32_t eventId, const String& eventName = String());
    
    // 克隆事件
    static std::unique_ptr<Event> cloneEvent(const Event& event);
    
    // 事件转换
    template<typename TargetEventT>
    static std::unique_ptr<TargetEventT> convertEvent(const Event& sourceEvent);

private:
    // 验证事件类型
    static bool isValidEventType(EventType type);
    static bool isValidCustomEventId(uint32_t eventId);
};

/**
 * @brief 事件类型检查器
 */
class EventTypeChecker {
public:
    // 检查事件类型
    template<typename EventT>
    static bool isType(const Event& event);
    
    // 安全转换
    template<typename EventT>
    static EventT* safeCast(Event* event);
    
    template<typename EventT>
    static const EventT* safeCast(const Event* event);
    
    // 事件类型验证
    static bool isSystemEvent(const Event& event);
    static bool isInputEvent(const Event& event);
    static bool isWidgetEvent(const Event& event);
    static bool isPageEvent(const Event& event);
    static bool isApplicationEvent(const Event& event);
    static bool isCustomEvent(const Event& event);
};

// 模板实现

template<typename EventT>
bool EventRegistry::registerEvent(const String& eventName, uint32_t eventId) {
    // 如果未指定 ID，则自动分配
    if (eventId == 0) {
        eventId = getNextCustomEventId();
    }
    
    // 创建默认创建器
    EventCreator creator = [](const Variant& data) -> std::unique_ptr<Event> {
        auto event = std::make_unique<EventT>();
        if (!data.isNull()) {
            event->setPayload(data);
        }
        return std::unique_ptr<Event>(event.release());
    };
    
    return registerEvent(eventName, eventId, creator);
}

template<typename EventT, typename... Args>
std::unique_ptr<EventT> EventFactory::createTypedEvent(Args&&... args) {
    return std::make_unique<EventT>(std::forward<Args>(args)...);
}

template<typename TargetEventT>
std::unique_ptr<TargetEventT> EventFactory::convertEvent(const Event& sourceEvent) {
    // 创建目标事件类型
    auto targetEvent = std::make_unique<TargetEventT>();
    
    // 复制基础属性
    targetEvent->setEventName(sourceEvent.eventName());
    targetEvent->setPayload(sourceEvent.payload());
    targetEvent->setTimestamp(sourceEvent.timestamp());
    targetEvent->setSource(sourceEvent.source());
    targetEvent->setPriority(sourceEvent.priority());
    
    // 复制属性（需要访问私有成员，可能需要调整设计）
    // 这里暂时使用序列化/反序列化方式
    auto serialized = sourceEvent.serialize();
    targetEvent->deserialize(serialized);
    
    return targetEvent;
}

template<typename EventT>
bool EventTypeChecker::isType(const Event& event) {
    return dynamic_cast<const EventT*>(&event) != nullptr;
}

template<typename EventT>
EventT* EventTypeChecker::safeCast(Event* event) {
    return dynamic_cast<EventT*>(event);
}

template<typename EventT>
const EventT* EventTypeChecker::safeCast(const Event* event) {
    return dynamic_cast<const EventT*>(event);
}

// 便捷宏定义

/**
 * @brief 注册自定义事件类型
 */
#define QGL_REGISTER_EVENT(EventClass, eventName) \
    QGL::EventRegistry::instance()->registerEvent<EventClass>(eventName)

#define QGL_REGISTER_EVENT_WITH_ID(EventClass, eventName, eventId) \
    QGL::EventRegistry::instance()->registerEvent<EventClass>(eventName, eventId)

/**
 * @brief 创建事件的便捷宏
 */
#define QGL_CREATE_EVENT(eventName, ...) \
    QGL::EventFactory::createEvent(eventName, ##__VA_ARGS__)

#define QGL_CREATE_TYPED_EVENT(EventType, ...) \
    QGL::EventFactory::createTypedEvent<EventType>(__VA_ARGS__)

/**
 * @brief 事件类型检查宏
 */
#define QGL_IS_EVENT_TYPE(event, EventType) \
    QGL::EventTypeChecker::isType<EventType>(event)

#define QGL_SAFE_CAST_EVENT(event, EventType) \
    QGL::EventTypeChecker::safeCast<EventType>(event)

} // namespace QGL

#endif // QGL_EVENTS_EVENT_FACTORY_H