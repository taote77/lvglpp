#include "Keyboard.h"
#include "TextArea.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Keyboard::Keyboard(Widget *parent)
    : Widget(tools::LvObjFactory::createLvglKeyboard, parent)
{}

void Keyboard::attach(TextArea *textarea)
{
    if (textarea) {
        lv_keyboard_set_textarea(lvObj(), textarea->lvObj());
    }
}

void Keyboard::setMode(lv_keyboard_mode_t mode)
{
    lv_keyboard_set_mode(lvObj(), mode);
}

lv_keyboard_mode_t Keyboard::mode() const
{
    return lv_keyboard_get_mode(lvObj());
}

void Keyboard::setTextArea(lv_obj_t *textarea)
{
    lv_keyboard_set_textarea(lvObj(), textarea);
}

} // namespace lvglpp::widgets
