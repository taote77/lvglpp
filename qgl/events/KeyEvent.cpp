#include "KeyEvent.h"
#include <unordered_map>

namespace QGL {

// KeyEvent 实现
KeyEvent::KeyEvent(EventType type, Key key, ModifierKey modifiers, const String& text)
    : Event(type)
    , m_key(key)
    , m_modifiers(modifiers)
    , m_text(text) {
    initializeKeyProperties();
}

KeyEvent::KeyEvent(const String& text, ModifierKey modifiers)
    : Event(EventType::Input)
    , m_key(Key::Key_Unknown)
    , m_modifiers(modifiers)
    , m_text(text) {
    initializeKeyProperties();
}

void KeyEvent::initializeKeyProperties() {
    // 构建键序列字符串
    String sequence;
    if (hasModifier(ModifierKey::ControlModifier)) sequence += "Ctrl+";
    if (hasModifier(ModifierKey::AltModifier)) sequence += "Alt+";
    if (hasModifier(ModifierKey::ShiftModifier)) sequence += "Shift+";
    if (hasModifier(ModifierKey::MetaModifier)) sequence += "Meta+";
    
    sequence += keyToString(m_key);
    m_keySequence = sequence;
}

bool KeyEvent::isModifierKey() const {
    return (m_key >= Key::Key_Shift && m_key <= Key::Key_ScrollLock);
}

bool KeyEvent::isFunctionKey() const {
    return (m_key >= Key::Key_F1 && m_key <= Key::Key_F24);
}

bool KeyEvent::isNavigationKey() const {
    return (m_key >= Key::Key_Home && m_key <= Key::Key_PageDown);
}

bool KeyEvent::isNumericKey() const {
    return (m_key >= Key::Key_0 && m_key <= Key::Key_9) ||
           (m_key >= Key::Key_NumPad0 && m_key <= Key::Key_NumPad9);
}

bool KeyEvent::isAlphabeticKey() const {
    return (m_key >= Key::Key_A && m_key <= Key::Key_Z);
}

bool KeyEvent::isPunctuationKey() const {
    return (m_key >= Key::Key_Exclam && m_key <= Key::Key_AsciiTilde);
}

bool KeyEvent::isWhitespaceKey() const {
    return (m_key == Key::Key_Space || m_key == Key::Key_Tab || 
            m_key == Key::Key_Return || m_key == Key::Key_Enter);
}

bool KeyEvent::isMultimediaKey() const {
    return (m_key >= Key::Key_VolumeDown && m_key <= Key::Key_MediaTogglePlayPause);
}

bool KeyEvent::matches(Key key, ModifierKey modifiers) const {
    return (m_key == key && m_modifiers == modifiers);
}

bool KeyEvent::matches(const String& keySequence) const {
    return m_keySequence == keySequence;
}

String KeyEvent::keySequence() const {
    return m_keySequence;
}

std::unique_ptr<KeyEvent> KeyEvent::createPressEvent(Key key, ModifierKey modifiers, const String& text) {
    return std::make_unique<KeyEvent>(EventType::KeyPress, key, modifiers, text);
}

std::unique_ptr<KeyEvent> KeyEvent::createReleaseEvent(Key key, ModifierKey modifiers, const String& text) {
    return std::make_unique<KeyEvent>(EventType::KeyRelease, key, modifiers, text);
}

std::unique_ptr<KeyEvent> KeyEvent::createTextEvent(const String& text, ModifierKey modifiers) {
    return std::make_unique<KeyEvent>(text, modifiers);
}

std::unique_ptr<KeyEvent> KeyEvent::createShortcutEvent(Key key, ModifierKey modifiers) {
    return std::make_unique<KeyEvent>(EventType::KeyPress, key, modifiers);
}

String KeyEvent::keyToString(Key key) {
    static std::unordered_map<Key, String> keyNames = {
        {Key::Key_Unknown, "Unknown"},
        {Key::Key_Escape, "Escape"},
        {Key::Key_Tab, "Tab"},
        {Key::Key_Backspace, "Backspace"},
        {Key::Key_Return, "Return"},
        {Key::Key_Enter, "Enter"},
        {Key::Key_Space, "Space"},
        {Key::Key_Left, "Left"},
        {Key::Key_Up, "Up"},
        {Key::Key_Right, "Right"},
        {Key::Key_Down, "Down"},
        {Key::Key_Home, "Home"},
        {Key::Key_End, "End"},
        {Key::Key_PageUp, "PageUp"},
        {Key::Key_PageDown, "PageDown"},
        {Key::Key_Shift, "Shift"},
        {Key::Key_Control, "Control"},
        {Key::Key_Alt, "Alt"},
        {Key::Key_Meta, "Meta"},
        // 添加更多键名映射...
    };
    
    auto it = keyNames.find(key);
    if (it != keyNames.end()) {
        return it->second;
    }
    
    // 对于字母和数字，直接转换
    if (key >= Key::Key_A && key <= Key::Key_Z) {
        return String(1, 'A' + (static_cast<int>(key) - static_cast<int>(Key::Key_A)));
    }
    if (key >= Key::Key_0 && key <= Key::Key_9) {
        return String(1, '0' + (static_cast<int>(key) - static_cast<int>(Key::Key_0)));
    }
    
    return "Key_" + std::to_string(static_cast<int>(key));
}

Key KeyEvent::stringToKey(const String& keyString) {
    static std::unordered_map<String, Key> stringToKeyMap = {
        {"Unknown", Key::Key_Unknown},
        {"Escape", Key::Key_Escape},
        {"Tab", Key::Key_Tab},
        {"Backspace", Key::Key_Backspace},
        {"Return", Key::Key_Return},
        {"Enter", Key::Key_Enter},
        {"Space", Key::Key_Space},
        {"Left", Key::Key_Left},
        {"Up", Key::Key_Up},
        {"Right", Key::Key_Right},
        {"Down", Key::Key_Down},
        {"Home", Key::Key_Home},
        {"End", Key::Key_End},
        {"PageUp", Key::Key_PageUp},
        {"PageDown", Key::Key_PageDown},
        {"Shift", Key::Key_Shift},
        {"Control", Key::Key_Control},
        {"Alt", Key::Key_Alt},
        {"Meta", Key::Key_Meta},
        // 添加更多映射...
    };
    
    auto it = stringToKeyMap.find(keyString);
    if (it != stringToKeyMap.end()) {
        return it->second;
    }
    
    // 处理单个字符
    if (keyString.length() == 1) {
        char c = keyString[0];
        if (c >= 'A' && c <= 'Z') {
            return static_cast<Key>(static_cast<int>(Key::Key_A) + (c - 'A'));
        }
        if (c >= '0' && c <= '9') {
            return static_cast<Key>(static_cast<int>(Key::Key_0) + (c - '0'));
        }
    }
    
    return Key::Key_Unknown;
}

String KeyEvent::modifiersToString(ModifierKey modifiers) {
    String result;
    if (static_cast<int>(modifiers) & static_cast<int>(ModifierKey::ControlModifier)) {
        result += "Ctrl+";
    }
    if (static_cast<int>(modifiers) & static_cast<int>(ModifierKey::AltModifier)) {
        result += "Alt+";
    }
    if (static_cast<int>(modifiers) & static_cast<int>(ModifierKey::ShiftModifier)) {
        result += "Shift+";
    }
    if (static_cast<int>(modifiers) & static_cast<int>(ModifierKey::MetaModifier)) {
        result += "Meta+";
    }
    
    // 移除末尾的+号
    if (!result.empty() && result.back() == '+') {
        result.pop_back();
    }
    
    return result;
}

ModifierKey KeyEvent::stringToModifiers(const String& modifierString) {
    int modifiers = 0;
    
    if (modifierString.find("Ctrl") != String::npos) {
        modifiers |= static_cast<int>(ModifierKey::ControlModifier);
    }
    if (modifierString.find("Alt") != String::npos) {
        modifiers |= static_cast<int>(ModifierKey::AltModifier);
    }
    if (modifierString.find("Shift") != String::npos) {
        modifiers |= static_cast<int>(ModifierKey::ShiftModifier);
    }
    if (modifierString.find("Meta") != String::npos) {
        modifiers |= static_cast<int>(ModifierKey::MetaModifier);
    }
    
    return static_cast<ModifierKey>(modifiers);
}

std::unique_ptr<Event> KeyEvent::clone() const {
    auto cloned = std::make_unique<KeyEvent>(type(), m_key, m_modifiers, m_text);
    
    // 复制所有属性
    cloned->m_keySequence = m_keySequence;
    cloned->m_nativeKeyCode = m_nativeKeyCode;
    cloned->m_nativeScanCode = m_nativeScanCode;
    cloned->m_nativeVirtualKey = m_nativeVirtualKey;
    cloned->m_autoRepeat = m_autoRepeat;
    cloned->m_repeatCount = m_repeatCount;
    cloned->m_hasIMEComposition = m_hasIMEComposition;
    cloned->m_compositionText = m_compositionText;
    cloned->m_compositionStart = m_compositionStart;
    cloned->m_compositionLength = m_compositionLength;
    cloned->m_isDeadKey = m_isDeadKey;
    
    // 复制基类属性
    cloned->setPayload(payload());
    cloned->setTimestamp(timestamp());
    cloned->setSource(source());
    cloned->setPriority(priority());
    
    return std::unique_ptr<Event>(cloned.release());
}

String KeyEvent::toString() const {
    return String("KeyEvent(type=") + std::to_string(static_cast<int>(type())) +
           ", key=" + keyToString(m_key) +
           ", text=\"" + m_text + "\")";
}

std::map<String, Variant> KeyEvent::serialize() const {
    auto data = Event::serialize();
    
    data["key"] = Variant::fromInt(static_cast<int>(m_key));
    data["modifiers"] = Variant::fromInt(static_cast<int>(m_modifiers));
    data["text"] = Variant::fromString(m_text);
    data["key_sequence"] = Variant::fromString(m_keySequence);
    data["native_key_code"] = Variant::fromInt(m_nativeKeyCode);
    data["native_scan_code"] = Variant::fromInt(m_nativeScanCode);
    data["native_virtual_key"] = Variant::fromInt(m_nativeVirtualKey);
    data["auto_repeat"] = Variant::fromBool(m_autoRepeat);
    data["repeat_count"] = Variant::fromInt(m_repeatCount);
    data["has_ime_composition"] = Variant::fromBool(m_hasIMEComposition);
    data["composition_text"] = Variant::fromString(m_compositionText);
    data["composition_start"] = Variant::fromInt(m_compositionStart);
    data["composition_length"] = Variant::fromInt(m_compositionLength);
    data["is_dead_key"] = Variant::fromBool(m_isDeadKey);
    
    return data;
}

void KeyEvent::deserialize(const std::map<String, Variant>& data) {
    Event::deserialize(data);
    
    auto getValue = [&data](const String& key, auto defaultValue) {
        auto it = data.find(key);
        if (it != data.end()) {
            if constexpr (std::is_same_v<decltype(defaultValue), int>) {
                return it->second.toInt();
            } else if constexpr (std::is_same_v<decltype(defaultValue), bool>) {
                return it->second.toBool();
            } else if constexpr (std::is_same_v<decltype(defaultValue), String>) {
                return it->second.toString();
            }
        }
        return defaultValue;
    };
    
    m_key = static_cast<Key>(getValue("key", 0));
    m_modifiers = static_cast<ModifierKey>(getValue("modifiers", 0));
    m_text = getValue("text", String());
    m_keySequence = getValue("key_sequence", String());
    m_nativeKeyCode = getValue("native_key_code", 0);
    m_nativeScanCode = getValue("native_scan_code", 0);
    m_nativeVirtualKey = getValue("native_virtual_key", 0);
    m_autoRepeat = getValue("auto_repeat", false);
    m_repeatCount = getValue("repeat_count", 0);
    m_hasIMEComposition = getValue("has_ime_composition", false);
    m_compositionText = getValue("composition_text", String());
    m_compositionStart = getValue("composition_start", 0);
    m_compositionLength = getValue("composition_length", 0);
    m_isDeadKey = getValue("is_dead_key", false);
}

} // namespace QGL