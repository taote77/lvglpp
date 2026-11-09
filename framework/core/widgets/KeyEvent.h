#ifndef LVGLPP_KEYEVENT_H
#define LVGLPP_KEYEVENT_H

#include <cstdint>

namespace lvglpp::widgets {

enum class Key {
    Unknown,
    Up,
    Down,
    Left,
    Right,
    Enter,
    Escape,
    Backspace,
    Delete,
    Tab,
    Next,
    Prev,
    Home,
    End,
    Character,
};

enum class KeyAction {
    Press,
    Release,
    Repeat,
    LongPress,
};

struct KeyEvent {
    Key       key      = Key::Unknown;
    uint32_t  raw      = 0;
    KeyAction action   = KeyAction::Press;
    bool      consumed = false;

    bool isCharacter() const
    {
        return key == Key::Character;
    }

    char32_t character() const
    {
        return static_cast<char32_t>(raw);
    }

    void consume()
    {
        consumed = true;
    }
};

Key keyFromLvgl(uint32_t raw);

uint32_t lvglKey(Key key);

} // namespace lvglpp::widgets

#endif // LVGLPP_KEYEVENT_H
