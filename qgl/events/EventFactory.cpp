#include "EventFactory.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "PageEvents.h"
#include <algorithm>

namespace QGL {

// EventRegistry 静态成员
EventRegistry* EventRegistry::s_instance = nullptr;
std::mutex EventRegistry::s_instanceMutex;

EventRegistry* EventRegistry::instance() {
    std::lock_guard<std::mutex> lock(s_instanceMutex);
    if (!s_instance) {
        s_instance = new EventRegistry();
    }
    return s_instance;
}

bool EventRegistry::registerEvent(const String& eventName, uint32_t eventId, EventCreator creator) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // 检查名称和ID是否已被使用
    if (m_nameToInfo.find(eventName) != m_nameToInfo.end()) {
        return false; // 名称已存在
    }
    
    if (m_idToInfo.find(eventId) != m_idToInfo.end()) {
        return false; // ID已存在
    }
    
    // 验证自定义事件ID范围
    if (eventId < 10000) {
        return false; // 自定义事件ID必须 >= 10000
    }
    
    // 创建事件信息并注册
    EventInfo info(eventName, eventId, std::move(creator));
    m_nameToInfo[eventName] = info;
    m_idToInfo[eventId] = info;
    
    return true;
}

bool EventRegistry::unregisterEvent(const String& eventName) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_nameToInfo.find(eventName);
    if (it != m_nameToInfo.end()) {
        uint32_t eventId = it->second.id;
        m_nameToInfo.erase(it);
        m_idToInfo.erase(eventId);
        return true;
    }
    
    return false;
}

bool EventRegistry::unregisterEvent(uint32_t eventId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_idToInfo.find(eventId);
    if (it != m_idToInfo.end()) {
        String eventName = it->second.name;
        m_idToInfo.erase(it);
        m_nameToInfo.erase(eventName);
        return true;
    }
    
    return false;
}

bool EventRegistry::isRegistered(const String& eventName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_nameToInfo.find(eventName) != m_nameToInfo.end();
}

bool EventRegistry::isRegistered(uint32_t eventId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_idToInfo.find(eventId) != m_idToInfo.end();
}

uint32_t EventRegistry::getEventId(const String& eventName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_nameToInfo.find(eventName);
    return (it != m_nameToInfo.end()) ? it->second.id : 0;
}

String EventRegistry::getEventName(uint32_t eventId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_idToInfo.find(eventId);
    return (it != m_idToInfo.end()) ? it->second.name : String();
}

EventCreator EventRegistry::getEventCreator(const String& eventName) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_nameToInfo.find(eventName);
    return (it != m_nameToInfo.end()) ? it->second.creator : EventCreator();
}

EventCreator EventRegistry::getEventCreator(uint32_t eventId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_idToInfo.find(eventId);
    return (it != m_idToInfo.end()) ? it->second.creator : EventCreator();
}

std::vector<String> EventRegistry::getAllEventNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<String> names;
    names.reserve(m_nameToInfo.size());
    
    for (const auto& pair : m_nameToInfo) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<uint32_t> EventRegistry::getAllEventIds() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<uint32_t> ids;
    ids.reserve(m_idToInfo.size());
    
    for (const auto& pair : m_idToInfo) {
        ids.push_back(pair.first);
    }
    
    return ids;
}

void EventRegistry::clearAll() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_nameToInfo.clear();
    m_idToInfo.clear();
}

uint32_t EventRegistry::getNextCustomEventId() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    uint32_t maxId = 10000; // 自定义事件起始ID
    
    for (const auto& pair : m_idToInfo) {
        if (pair.first >= 10000) {
            maxId = std::max(maxId, pair.first);
        }
    }
    
    return maxId + 1;
}

// EventFactory 实现

std::unique_ptr<Event> EventFactory::createEvent(EventType type) {
    if (!isValidEventType(type)) {
        return nullptr;
    }
    
    return std::make_unique<Event>(type);
}

std::unique_ptr<Event> EventFactory::createEvent(EventType type, const Variant& payload) {
    if (!isValidEventType(type)) {
        return nullptr;
    }
    
    return std::make_unique<Event>(type, payload);
}

std::unique_ptr<Event> EventFactory::createEvent(const String& eventName) {
    auto registry = EventRegistry::instance();
    auto creator = registry->getEventCreator(eventName);
    
    if (creator) {
        return creator(Variant());
    }
    
    // 如果没有注册的创建器，创建通用自定义事件
    return std::make_unique<CustomEvent>(eventName);
}

std::unique_ptr<Event> EventFactory::createEvent(const String& eventName, const Variant& data) {
    auto registry = EventRegistry::instance();
    auto creator = registry->getEventCreator(eventName);
    
    if (creator) {
        return creator(data);
    }
    
    // 如果没有注册的创建器，创建通用自定义事件
    return std::make_unique<CustomEvent>(eventName, data);
}

std::unique_ptr<Event> EventFactory::createEvent(uint32_t eventId) {
    auto registry = EventRegistry::instance();
    auto creator = registry->getEventCreator(eventId);
    
    if (creator) {
        return creator(Variant());
    }
    
    // 如果在自定义事件范围内，创建通用自定义事件
    if (isValidCustomEventId(eventId)) {
        String eventName = registry->getEventName(eventId);
        return std::make_unique<CustomEvent>(eventId, eventName);
    }
    
    // 否则创建基础事件
    return createEvent(static_cast<EventType>(eventId));
}

std::unique_ptr<Event> EventFactory::createEvent(uint32_t eventId, const Variant& data) {
    auto registry = EventRegistry::instance();
    auto creator = registry->getEventCreator(eventId);
    
    if (creator) {
        return creator(data);
    }
    
    // 如果在自定义事件范围内，创建通用自定义事件
    if (isValidCustomEventId(eventId)) {
        String eventName = registry->getEventName(eventId);
        auto event = std::make_unique<CustomEvent>(eventId, eventName);
        event->setPayload(data);
        return std::unique_ptr<Event>(event.release());
    }
    
    // 否则创建基础事件
    return createEvent(static_cast<EventType>(eventId), data);
}

std::unique_ptr<MouseEvent> EventFactory::createMouseEvent(EventType type, const Point& pos, int button) {
    return std::make_unique<MouseEvent>(type, pos, static_cast<MouseButton>(button));
}

std::unique_ptr<KeyEvent> EventFactory::createKeyEvent(EventType type, int key, const String& text) {
    return std::make_unique<KeyEvent>(type, static_cast<Key>(key), ModifierKey::NoModifier, text);
}

std::unique_ptr<SystemEvent> EventFactory::createSystemEvent(EventType type) {
    return std::make_unique<SystemEvent>(type);
}

std::unique_ptr<ApplicationEvent> EventFactory::createApplicationEvent(EventType type) {
    return std::make_unique<ApplicationEvent>(type);
}

std::unique_ptr<PageEvent> EventFactory::createPageEvent(EventType type, const String& pageId) {
    auto event = std::make_unique<PageEvent>(type);
    if (!pageId.empty()) {
        event->setPageId(pageId);
    }
    return event;
}

std::unique_ptr<WidgetEvent> EventFactory::createWidgetEvent(EventType type) {
    return std::make_unique<WidgetEvent>(type);
}

std::unique_ptr<CustomEvent> EventFactory::createCustomEvent(const String& eventName) {
    return std::make_unique<CustomEvent>(eventName);
}

std::unique_ptr<CustomEvent> EventFactory::createCustomEvent(const String& eventName, const Variant& payload) {
    return std::make_unique<CustomEvent>(eventName, payload);
}

std::unique_ptr<CustomEvent> EventFactory::createCustomEvent(uint32_t eventId, const String& eventName) {
    return std::make_unique<CustomEvent>(eventId, eventName);
}

std::unique_ptr<Event> EventFactory::cloneEvent(const Event& event) {
    return event.clone();
}

bool EventFactory::isValidEventType(EventType type) {
    uint32_t typeValue = static_cast<uint32_t>(type);
    return typeValue > 0 && typeValue < 100000; // 合理的范围检查
}

bool EventFactory::isValidCustomEventId(uint32_t eventId) {
    return eventId >= 10000; // 自定义事件ID范围
}

// EventTypeChecker 实现

bool EventTypeChecker::isSystemEvent(const Event& event) {
    return event.isSystemEvent();
}

bool EventTypeChecker::isInputEvent(const Event& event) {
    return event.isInputEvent();
}

bool EventTypeChecker::isWidgetEvent(const Event& event) {
    return event.isWidgetEvent();
}

bool EventTypeChecker::isPageEvent(const Event& event) {
    return event.isPageEvent();
}

bool EventTypeChecker::isApplicationEvent(const Event& event) {
    return event.isApplicationEvent();
}

bool EventTypeChecker::isCustomEvent(const Event& event) {
    return event.isCustomEvent();
}

} // namespace QGL