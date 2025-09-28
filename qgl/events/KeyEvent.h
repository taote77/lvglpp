#ifndef QGL_EVENTS_KEYEVENT_H
#define QGL_EVENTS_KEYEVENT_H

#include "Event.h"
#include "MouseEvent.h" // 为了使用ModifierKey
#include "../core/Types.h"

namespace QGL {

/**
 * @brief 键码枚举
 * 基于标准键盘布局定义的键码
 */
enum class Key : int {
    // 特殊键
    Key_Unknown = 0,
    Key_Escape = 1,
    Key_Tab = 2,
    Key_Backtab = 3,
    Key_Backspace = 4,
    Key_Return = 5,
    Key_Enter = 6,
    Key_Insert = 7,
    Key_Delete = 8,
    Key_Pause = 9,
    Key_Print = 10,
    Key_SysReq = 11,
    Key_Clear = 12,
    
    // 导航键
    Key_Home = 20,
    Key_End = 21,
    Key_Left = 22,
    Key_Up = 23,
    Key_Right = 24,
    Key_Down = 25,
    Key_PageUp = 26,
    Key_PageDown = 27,
    
    // 修饰键
    Key_Shift = 30,
    Key_Control = 31,
    Key_Meta = 32,
    Key_Alt = 33,
    Key_AltGr = 34,
    Key_CapsLock = 35,
    Key_NumLock = 36,
    Key_ScrollLock = 37,
    
    // 功能键
    Key_F1 = 40,
    Key_F2 = 41,
    Key_F3 = 42,
    Key_F4 = 43,
    Key_F5 = 44,
    Key_F6 = 45,
    Key_F7 = 46,
    Key_F8 = 47,
    Key_F9 = 48,
    Key_F10 = 49,
    Key_F11 = 50,
    Key_F12 = 51,
    Key_F13 = 52,
    Key_F14 = 53,
    Key_F15 = 54,
    Key_F16 = 55,
    Key_F17 = 56,
    Key_F18 = 57,
    Key_F19 = 58,
    Key_F20 = 59,
    Key_F21 = 60,
    Key_F22 = 61,
    Key_F23 = 62,
    Key_F24 = 63,
    
    // 数字键 (主键盘区)
    Key_0 = 70,
    Key_1 = 71,
    Key_2 = 72,
    Key_3 = 73,
    Key_4 = 74,
    Key_5 = 75,
    Key_6 = 76,
    Key_7 = 77,
    Key_8 = 78,
    Key_9 = 79,
    
    // 字母键
    Key_A = 80,
    Key_B = 81,
    Key_C = 82,
    Key_D = 83,
    Key_E = 84,
    Key_F = 85,
    Key_G = 86,
    Key_H = 87,
    Key_I = 88,
    Key_J = 89,
    Key_K = 90,
    Key_L = 91,
    Key_M = 92,
    Key_N = 93,
    Key_O = 94,
    Key_P = 95,
    Key_Q = 96,
    Key_R = 97,
    Key_S = 98,
    Key_T = 99,
    Key_U = 100,
    Key_V = 101,
    Key_W = 102,
    Key_X = 103,
    Key_Y = 104,
    Key_Z = 105,
    
    // 标点符号键
    Key_Space = 110,
    Key_Exclam = 111,      // !
    Key_QuoteDbl = 112,    // "
    Key_NumberSign = 113,  // #
    Key_Dollar = 114,      // $
    Key_Percent = 115,     // %
    Key_Ampersand = 116,   // &
    Key_Apostrophe = 117,  // '
    Key_ParenLeft = 118,   // (
    Key_ParenRight = 119,  // )
    Key_Asterisk = 120,    // *
    Key_Plus = 121,        // +
    Key_Comma = 122,       // ,
    Key_Minus = 123,       // -
    Key_Period = 124,      // .
    Key_Slash = 125,       // /
    Key_Colon = 126,       // :
    Key_Semicolon = 127,   // ;
    Key_Less = 128,        // <
    Key_Equal = 129,       // =
    Key_Greater = 130,     // >
    Key_Question = 131,    // ?
    Key_At = 132,          // @
    Key_BracketLeft = 133, // [
    Key_Backslash = 134,   // \
    Key_BracketRight = 135,// ]
    Key_AsciiCircum = 136, // ^
    Key_Underscore = 137,  // _
    Key_QuoteLeft = 138,   // `
    Key_BraceLeft = 139,   // {
    Key_Bar = 140,         // |
    Key_BraceRight = 141,  // }
    Key_AsciiTilde = 142,  // ~
    
    // 数字键盘
    Key_NumPad0 = 150,
    Key_NumPad1 = 151,
    Key_NumPad2 = 152,
    Key_NumPad3 = 153,
    Key_NumPad4 = 154,
    Key_NumPad5 = 155,
    Key_NumPad6 = 156,
    Key_NumPad7 = 157,
    Key_NumPad8 = 158,
    Key_NumPad9 = 159,
    Key_NumPadMultiply = 160,  // *
    Key_NumPadAdd = 161,       // +
    Key_NumPadSeparator = 162, // ,
    Key_NumPadSubtract = 163,  // -
    Key_NumPadDecimal = 164,   // .
    Key_NumPadDivide = 165,    // /
    Key_NumPadEnter = 166,
    Key_NumPadEqual = 167,     // =
    
    // 多媒体键
    Key_VolumeDown = 180,
    Key_VolumeUp = 181,
    Key_VolumeMute = 182,
    Key_MediaPlay = 183,
    Key_MediaStop = 184,
    Key_MediaPrevious = 185,
    Key_MediaNext = 186,
    Key_MediaRecord = 187,
    Key_MediaPause = 188,
    Key_MediaTogglePlayPause = 189,
    
    // 浏览器和应用键
    Key_HomePage = 200,
    Key_Favorites = 201,
    Key_Search = 202,
    Key_Standby = 203,
    Key_OpenUrl = 204,
    Key_LaunchMail = 205,
    Key_LaunchMedia = 206,
    Key_Launch0 = 207,
    Key_Launch1 = 208,
    Key_Launch2 = 209,
    Key_Launch3 = 210,
    Key_Launch4 = 211,
    Key_Launch5 = 212,
    Key_Launch6 = 213,
    Key_Launch7 = 214,
    Key_Launch8 = 215,
    Key_Launch9 = 216,
    
    // 其他特殊键
    Key_Menu = 230,
    Key_Help = 231,
    Key_Back = 232,
    Key_Forward = 233,
    Key_Stop = 234,
    Key_Refresh = 235,
    Key_PowerOff = 236,
    Key_WakeUp = 237,
    Key_Eject = 238,
    Key_ScreenSaver = 239,
    Key_Calculator = 240,
    Key_Memo = 241,
    Key_ToDoList = 242,
    Key_Calendar = 243,
    Key_PowerDown = 244,
    Key_ContrastAdjust = 245,
    Key_BrightnessAdjust = 246,
    
    // 自定义键范围
    Key_User = 1000
};

/**
 * @brief 键盘事件类
 * 
 * 处理键盘输入事件，包括按键按下、释放、文本输入等。
 * 提供键码、文本、修饰键状态等详细信息。
 */
class KeyEvent : public Event {
public:
    /**
     * @brief 构造键盘事件
     * @param type 事件类型
     * @param key 键码
     * @param modifiers 修饰键状态
     * @param text 输入的文本
     */
    KeyEvent(EventType type, Key key, ModifierKey modifiers = ModifierKey::NoModifier, const String& text = String());
    
    /**
     * @brief 构造文本输入事件
     * @param text 输入的文本
     * @param modifiers 修饰键状态
     */
    explicit KeyEvent(const String& text, ModifierKey modifiers = ModifierKey::NoModifier);
    
    virtual ~KeyEvent() = default;
    
    // 基本属性
    Key key() const { return m_key; }
    void setKey(Key key) { m_key = key; }
    
    int keyCode() const { return static_cast<int>(m_key); }
    
    ModifierKey modifiers() const { return m_modifiers; }
    void setModifiers(ModifierKey modifiers) { m_modifiers = modifiers; }
    
    String text() const { return m_text; }
    void setText(const String& text) { m_text = text; }
    
    // 原生键码支持（平台相关）
    int nativeKeyCode() const { return m_nativeKeyCode; }
    void setNativeKeyCode(int nativeCode) { m_nativeKeyCode = nativeCode; }
    
    int nativeScanCode() const { return m_nativeScanCode; }
    void setNativeScanCode(int scanCode) { m_nativeScanCode = scanCode; }
    
    int nativeVirtualKey() const { return m_nativeVirtualKey; }
    void setNativeVirtualKey(int virtualKey) { m_nativeVirtualKey = virtualKey; }
    
    // 修饰键状态检查
    bool hasModifier(ModifierKey modifier) const {
        return (static_cast<int>(m_modifiers) & static_cast<int>(modifier)) != 0;
    }
    
    bool shiftPressed() const { return hasModifier(ModifierKey::ShiftModifier); }
    bool ctrlPressed() const { return hasModifier(ModifierKey::ControlModifier); }
    bool altPressed() const { return hasModifier(ModifierKey::AltModifier); }
    bool metaPressed() const { return hasModifier(ModifierKey::MetaModifier); }
    
    // 键类型判断
    bool isModifierKey() const;
    bool isFunctionKey() const;
    bool isNavigationKey() const;
    bool isNumericKey() const;
    bool isAlphabeticKey() const;
    bool isPunctuationKey() const;
    bool isWhitespaceKey() const;
    bool isMultimediaKey() const;
    
    // 自动重复检测
    bool isAutoRepeat() const { return m_autoRepeat; }
    void setAutoRepeat(bool autoRepeat) { m_autoRepeat = autoRepeat; }
    
    int repeatCount() const { return m_repeatCount; }
    void setRepeatCount(int count) { m_repeatCount = count; }
    
    // 按键组合检测
    bool matches(Key key, ModifierKey modifiers = ModifierKey::NoModifier) const;
    bool matches(const String& keySequence) const;
    
    // 键序列支持
    String keySequence() const;
    void setKeySequence(const String& sequence) { m_keySequence = sequence; }
    
    // IME（输入法）支持
    bool hasIMEComposition() const { return m_hasIMEComposition; }
    void setIMEComposition(bool hasComposition) { m_hasIMEComposition = hasComposition; }
    
    String compositionText() const { return m_compositionText; }
    void setCompositionText(const String& text) { m_compositionText = text; }
    
    int compositionStart() const { return m_compositionStart; }
    void setCompositionStart(int start) { m_compositionStart = start; }
    
    int compositionLength() const { return m_compositionLength; }
    void setCompositionLength(int length) { m_compositionLength = length; }
    
    // 死键支持（Dead Key）
    bool isDeadKey() const { return m_isDeadKey; }
    void setDeadKey(bool isDeadKey) { m_isDeadKey = isDeadKey; }
    
    // 事件创建便捷方法
    static std::unique_ptr<KeyEvent> createPressEvent(Key key, ModifierKey modifiers = ModifierKey::NoModifier, const String& text = String());
    static std::unique_ptr<KeyEvent> createReleaseEvent(Key key, ModifierKey modifiers = ModifierKey::NoModifier, const String& text = String());
    static std::unique_ptr<KeyEvent> createTextEvent(const String& text, ModifierKey modifiers = ModifierKey::NoModifier);
    static std::unique_ptr<KeyEvent> createShortcutEvent(Key key, ModifierKey modifiers);
    
    // 快捷键支持
    static String keyToString(Key key);
    static Key stringToKey(const String& keyString);
    static String modifiersToString(ModifierKey modifiers);
    static ModifierKey stringToModifiers(const String& modifierString);
    
    // 重写基类方法
    std::unique_ptr<Event> clone() const override;
    String toString() const override;
    std::map<String, Variant> serialize() const override;
    void deserialize(const std::map<String, Variant>& data) override;
    String className() const override { return "KeyEvent"; }

private:
    Key m_key;                      // 键码
    ModifierKey m_modifiers;        // 修饰键状态
    String m_text;                  // 输入的文本
    String m_keySequence;           // 键序列
    
    // 平台相关的原生键码
    int m_nativeKeyCode = 0;        // 原生键码
    int m_nativeScanCode = 0;       // 扫描码
    int m_nativeVirtualKey = 0;     // 虚拟键码
    
    // 自动重复
    bool m_autoRepeat = false;      // 是否为自动重复
    int m_repeatCount = 0;          // 重复次数
    
    // IME支持
    bool m_hasIMEComposition = false;  // 是否有IME组合
    String m_compositionText;          // 组合文本
    int m_compositionStart = 0;        // 组合开始位置
    int m_compositionLength = 0;       // 组合长度
    
    // 死键支持
    bool m_isDeadKey = false;       // 是否为死键
    
    // 辅助方法
    void initializeKeyProperties();
};

/**
 * @brief 按键按下事件
 */
class KeyPressEvent : public KeyEvent {
public:
    KeyPressEvent(Key key, ModifierKey modifiers = ModifierKey::NoModifier, const String& text = String())
        : KeyEvent(EventType::KeyPress, key, modifiers, text) {}
        
    String className() const override { return "KeyPressEvent"; }
};

/**
 * @brief 按键释放事件
 */
class KeyReleaseEvent : public KeyEvent {
public:
    KeyReleaseEvent(Key key, ModifierKey modifiers = ModifierKey::NoModifier, const String& text = String())
        : KeyEvent(EventType::KeyRelease, key, modifiers, text) {}
        
    String className() const override { return "KeyReleaseEvent"; }
};

/**
 * @brief 文本输入事件
 */
class TextInputEvent : public KeyEvent {
public:
    explicit TextInputEvent(const String& text, ModifierKey modifiers = ModifierKey::NoModifier)
        : KeyEvent(text, modifiers) {}
        
    String className() const override { return "TextInputEvent"; }
};

/**
 * @brief IME组合事件
 */
class IMECompositionEvent : public KeyEvent {
public:
    IMECompositionEvent(const String& compositionText, int start, int length)
        : KeyEvent(EventType::Input, Key::Key_Unknown) {
        setIMEComposition(true);
        setCompositionText(compositionText);
        setCompositionStart(start);
        setCompositionLength(length);
    }
    
    String className() const override { return "IMECompositionEvent"; }
};

} // namespace QGL

#endif // QGL_EVENTS_KEYEVENT_H