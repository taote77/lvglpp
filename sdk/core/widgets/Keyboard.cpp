#include "Keyboard.h"
#include "TextArea.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Keyboard::Keyboard(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglKeyboard, parent)
{}

void Keyboard::attach(TextArea *textarea)
{
    if (textarea) {
        lv_keyboard_set_textarea(getLvglItem(), textarea->getLvglItem());
    }
}

void Keyboard::setMode(lv_keyboard_mode_t mode)
{
    lv_keyboard_set_mode(getLvglItem(), mode);
}

lv_keyboard_mode_t Keyboard::mode() const
{
    return lv_keyboard_get_mode(getLvglItem());
}

void Keyboard::setTextArea(lv_obj_t *textarea)
{
    lv_keyboard_set_textarea(getLvglItem(), textarea);
}

} // namespace lvglpp::widgets
