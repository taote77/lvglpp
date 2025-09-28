#include "Event.h"
#include <sstream>
#include <iomanip>

namespace QGL {

Event::Event(EventType type) 
    : m_type(type)
    , m_timestamp(getCurrentTimestamp()) {
}

Event::Event(EventType type, const Variant& payload)
    : m_type(type)
    , m_payload(payload) 
    , m_timestamp(getCurrentTimestamp()) {
}

void Event::setAttribute(const String& key, const Variant& value) {
    m_attributes[key] = value;
}

Variant Event::getAttribute(const String& key) const {
    auto it = m_attributes.find(key);
    return (it != m_attributes.end()) ? it->second : Variant();
}

bool Event::hasAttribute(const String& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

void Event::removeAttribute(const String& key) {
    m_attributes.erase(key);
}

void Event::clearAttributes() {
    m_attributes.clear();
}

std::unique_ptr<Event> Event::clone() const {
    auto cloned = std::make_unique<Event>(m_type, m_payload);
    cloned->m_eventName = m_eventName;
    cloned->m_timestamp = m_timestamp;
    cloned->m_source = m_source;
    cloned->m_priority = m_priority;
    cloned->m_handled = EventHandled::NotHandled; // 重置处理状态
    cloned->m_attributes = m_attributes;
    return cloned;
}

String Event::toString() const {
    std::ostringstream oss;
    oss << "Event{type=" << static_cast<uint32_t>(m_type);
    
    if (!m_eventName.empty()) {
        oss << ", name=" << m_eventName;
    }
    
    oss << ", timestamp=" << m_timestamp;
    
    if (m_source) {
        oss << ", source=" << m_source->getObjectName();
    }
    
    oss << ", priority=" << static_cast<int>(m_priority);
    oss << ", handled=" << static_cast<int>(m_handled);
    
    if (m_payload.isValid()) {
        oss << ", payload=" << m_payload.toString();
    }
    
    if (!m_attributes.empty()) {
        oss << ", attributes={";
        bool first = true;
        for (const auto& attr : m_attributes) {
            if (!first) oss << ", ";
            oss << attr.first << "=" << attr.second.toString();
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    return oss.str();
}

std::map<String, Variant> Event::serialize() const {
    std::map<String, Variant> data;
    data["type"] = static_cast<uint32_t>(m_type);
    data["eventName"] = m_eventName;
    data["payload"] = m_payload;
    data["timestamp"] = static_cast<uint64_t>(m_timestamp);
    data["priority"] = static_cast<int>(m_priority);
    data["handled"] = static_cast<int>(m_handled);
    
    // 序列化属性
    for (const auto& attr : m_attributes) {
        data["attr_" + attr.first] = attr.second;
    }
    
    return data;
}

void Event::deserialize(const std::map<String, Variant>& data) {
    auto it = data.find("type");
    if (it != data.end()) {
        m_type = static_cast<EventType>(static_cast<uint32_t>(it->second.toInt()));
    }
    
    it = data.find("eventName");
    if (it != data.end()) {
        m_eventName = it->second.toString();
    }
    
    it = data.find("payload");
    if (it != data.end()) {
        m_payload = it->second;
    }
    
    it = data.find("timestamp");
    if (it != data.end()) {
        m_timestamp = static_cast<uint64_t>(it->second.toInt64());
    }
    
    it = data.find("priority");
    if (it != data.end()) {
        m_priority = static_cast<EventPriority>(it->second.toInt());
    }
    
    it = data.find("handled");
    if (it != data.end()) {
        m_handled = static_cast<EventHandled>(it->second.toInt());
    }
    
    // 反序列化属性
    m_attributes.clear();
    for (const auto& item : data) {
        if (item.first.substr(0, 5) == "attr_") {
            String attrName = item.first.substr(5);
            m_attributes[attrName] = item.second;
        }
    }
}

std::unique_ptr<Event> Event::create(EventType type) {
    return std::make_unique<Event>(type);
}

std::unique_ptr<Event> Event::create(EventType type, const Variant& payload) {
    return std::make_unique<Event>(type, payload);
}

uint64_t Event::getCurrentTimestamp() {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

} // namespace QGL