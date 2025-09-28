#include "MouseEvent.h"
#include <cmath>

namespace QGL {

// MouseEvent 实现
MouseEvent::MouseEvent(EventType type, const Point& position, MouseButton button, ModifierKey modifiers)
    : Event(type)
    , m_position(position)
    , m_lastPosition(position)
    , m_globalPosition(position) 
    , m_button(button)
    , m_buttons(button)
    , m_modifiers(modifiers) {
}

MouseEvent::MouseEvent(const Point& position, const Point& lastPosition, ModifierKey modifiers)
    : Event(EventType::MouseMove)
    , m_position(position)
    , m_lastPosition(lastPosition)
    , m_globalPosition(position)
    , m_button(MouseButton::NoButton)
    , m_buttons(MouseButton::NoButton)
    , m_modifiers(modifiers) {
}

double MouseEvent::distance() const {
    Point delta = m_position - m_lastPosition;
    return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}

double MouseEvent::angle() const {
    Point delta = m_position - m_lastPosition;
    if (delta.x == 0 && delta.y == 0) {
        return 0.0;
    }
    return std::atan2(delta.y, delta.x);
}

std::unique_ptr<MouseEvent> MouseEvent::createPressEvent(const Point& pos, MouseButton button, ModifierKey modifiers) {
    return std::make_unique<MouseEvent>(EventType::MousePress, pos, button, modifiers);
}

std::unique_ptr<MouseEvent> MouseEvent::createReleaseEvent(const Point& pos, MouseButton button, ModifierKey modifiers) {
    return std::make_unique<MouseEvent>(EventType::MouseRelease, pos, button, modifiers);
}

std::unique_ptr<MouseEvent> MouseEvent::createClickEvent(const Point& pos, MouseButton button, int clickCount, ModifierKey modifiers) {
    auto event = std::make_unique<MouseEvent>(EventType::MouseClick, pos, button, modifiers);
    event->setClickCount(clickCount);
    return event;
}

std::unique_ptr<MouseEvent> MouseEvent::createDoubleClickEvent(const Point& pos, MouseButton button, ModifierKey modifiers) {
    auto event = std::make_unique<MouseEvent>(EventType::MouseDoubleClick, pos, button, modifiers);
    event->setClickCount(2);
    return event;
}

std::unique_ptr<MouseEvent> MouseEvent::createMoveEvent(const Point& pos, const Point& lastPos, MouseButton buttons, ModifierKey modifiers) {
    auto event = std::make_unique<MouseEvent>(pos, lastPos, modifiers);
    event->setButtons(buttons);
    return event;
}

std::unique_ptr<MouseEvent> MouseEvent::createWheelEvent(const Point& pos, const Point& wheelDelta, ModifierKey modifiers) {
    auto event = std::make_unique<MouseEvent>(EventType::Input, pos, MouseButton::NoButton, modifiers);
    event->setWheelDelta(wheelDelta);
    return event;
}

std::unique_ptr<MouseEvent> MouseEvent::createDragEvent(const Point& pos, const Point& startPos, MouseButton button, ModifierKey modifiers) {
    auto event = std::make_unique<MouseEvent>(EventType::MouseMove, pos, button, modifiers);
    event->setDragEvent(true);
    event->setDragStartPosition(startPos);
    return event;
}

std::unique_ptr<Event> MouseEvent::clone() const {
    auto cloned = std::make_unique<MouseEvent>(type(), m_position, m_button, m_modifiers);
    // 复制所有属性
    cloned->m_lastPosition = m_lastPosition;
    cloned->m_globalPosition = m_globalPosition;
    cloned->m_buttons = m_buttons;
    cloned->m_clickCount = m_clickCount;
    cloned->m_clickTime = m_clickTime;
    cloned->m_isDragEvent = m_isDragEvent;
    cloned->m_dragStartPosition = m_dragStartPosition;
    cloned->m_wheelDelta = m_wheelDelta;
    cloned->m_isTouchEvent = m_isTouchEvent;
    cloned->m_touchId = m_touchId;
    cloned->m_pressure = m_pressure;
    cloned->m_tilt = m_tilt;
    
    // 复制基类属性
    cloned->setPayload(payload());
    cloned->setTimestamp(timestamp());
    cloned->setSource(source());
    cloned->setPriority(priority());
    
    return std::unique_ptr<Event>(cloned.release());
}

String MouseEvent::toString() const {
    return String("MouseEvent(type=") + std::to_string(static_cast<int>(type())) +
           ", pos=(" + std::to_string(m_position.x) + "," + std::to_string(m_position.y) + ")" +
           ", button=" + std::to_string(static_cast<int>(m_button)) + ")";
}

std::map<String, Variant> MouseEvent::serialize() const {
    auto data = Event::serialize();
    
    data["position_x"] = Variant::fromInt(m_position.x);
    data["position_y"] = Variant::fromInt(m_position.y);
    data["last_position_x"] = Variant::fromInt(m_lastPosition.x);
    data["last_position_y"] = Variant::fromInt(m_lastPosition.y);
    data["global_position_x"] = Variant::fromInt(m_globalPosition.x);
    data["global_position_y"] = Variant::fromInt(m_globalPosition.y);
    data["button"] = Variant::fromInt(static_cast<int>(m_button));
    data["buttons"] = Variant::fromInt(static_cast<int>(m_buttons));
    data["modifiers"] = Variant::fromInt(static_cast<int>(m_modifiers));
    data["click_count"] = Variant::fromInt(m_clickCount);
    data["is_drag_event"] = Variant::fromBool(m_isDragEvent);
    data["wheel_delta_x"] = Variant::fromInt(m_wheelDelta.x);
    data["wheel_delta_y"] = Variant::fromInt(m_wheelDelta.y);
    data["is_touch_event"] = Variant::fromBool(m_isTouchEvent);
    data["touch_id"] = Variant::fromInt(m_touchId);
    data["pressure"] = Variant::fromFloat(m_pressure);
    
    return data;
}

void MouseEvent::deserialize(const std::map<String, Variant>& data) {
    Event::deserialize(data);
    
    auto getValue = [&data](const String& key, auto defaultValue) {
        auto it = data.find(key);
        if (it != data.end()) {
            if constexpr (std::is_same_v<decltype(defaultValue), int>) {
                return it->second.toInt();
            } else if constexpr (std::is_same_v<decltype(defaultValue), bool>) {
                return it->second.toBool();
            } else if constexpr (std::is_same_v<decltype(defaultValue), float>) {
                return it->second.toFloat();
            }
        }
        return defaultValue;
    };
    
    m_position.x = getValue("position_x", 0);
    m_position.y = getValue("position_y", 0);
    m_lastPosition.x = getValue("last_position_x", 0);
    m_lastPosition.y = getValue("last_position_y", 0);
    m_globalPosition.x = getValue("global_position_x", 0);
    m_globalPosition.y = getValue("global_position_y", 0);
    m_button = static_cast<MouseButton>(getValue("button", 0));
    m_buttons = static_cast<MouseButton>(getValue("buttons", 0));
    m_modifiers = static_cast<ModifierKey>(getValue("modifiers", 0));
    m_clickCount = getValue("click_count", 0);
    m_isDragEvent = getValue("is_drag_event", false);
    m_wheelDelta.x = getValue("wheel_delta_x", 0);
    m_wheelDelta.y = getValue("wheel_delta_y", 0);
    m_isTouchEvent = getValue("is_touch_event", false);
    m_touchId = getValue("touch_id", -1);
    m_pressure = getValue("pressure", 1.0f);
}

} // namespace QGL