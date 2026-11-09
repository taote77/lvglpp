#include "KeyEvent.h"

#include "lvgl.h"

namespace lvglpp::widgets {

Key keyFromLvgl(uint32_t raw)
{
    switch (raw)
    {
    case LV_KEY_UP:
        return Key::Up;
    case LV_KEY_DOWN:
        return Key::Down;
    case LV_KEY_LEFT:
        return Key::Left;
    case LV_KEY_RIGHT:
        return Key::Right;
    case LV_KEY_ENTER:
        return Key::Enter;
    case LV_KEY_ESC:
        return Key::Escape;
    case LV_KEY_BACKSPACE:
        return Key::Backspace;
    case LV_KEY_DEL:
        return Key::Delete;
    case LV_KEY_NEXT:
        return Key::Next;
    case LV_KEY_PREV:
        return Key::Prev;
    case LV_KEY_HOME:
        return Key::Home;
    case LV_KEY_END:
        return Key::End;
    default:
        break;
    }

    if (raw >= 32 && raw <= 0x10FFFF)
    {
        return Key::Character;
    }
    return Key::Unknown;
}

uint32_t lvglKey(Key key)
{
    switch (key)
    {
    case Key::Up:
        return LV_KEY_UP;
    case Key::Down:
        return LV_KEY_DOWN;
    case Key::Left:
        return LV_KEY_LEFT;
    case Key::Right:
        return LV_KEY_RIGHT;
    case Key::Enter:
        return LV_KEY_ENTER;
    case Key::Escape:
        return LV_KEY_ESC;
    case Key::Backspace:
        return LV_KEY_BACKSPACE;
    case Key::Delete:
        return LV_KEY_DEL;
    case Key::Tab:
    case Key::Next:
        return LV_KEY_NEXT;
    case Key::Prev:
        return LV_KEY_PREV;
    case Key::Home:
        return LV_KEY_HOME;
    case Key::End:
        return LV_KEY_END;
    case Key::Character:
    case Key::Unknown:
    default:
        return 0;
    }
}

} // namespace lvglpp::widgets
