#ifndef LVGLPP_KEYBOARD_H
#define LVGLPP_KEYBOARD_H

#include "Widget.h"

namespace lvglpp::widgets {

class TextArea; // forward declaration

class Keyboard : public Widget
{
public:
    explicit Keyboard(Widget *parent = nullptr);

    /// Attach this keyboard to a TextArea (shows/hides automatically)
    void attach(TextArea *textarea);

    /// Set keyboard mode: LV_KEYBOARD_MODE_TEXT_LOWER, TEXT_UPPER, SPECIAL, NUMBER, USER_1-4
    void setMode(lv_keyboard_mode_t mode);

    /// Get keyboard mode
    lv_keyboard_mode_t mode() const;

    /// Set the textarea to type into
    void setTextArea(lv_obj_t *textarea);
};

} // namespace lvglpp::widgets

#endif // LVGLPP_KEYBOARD_H
