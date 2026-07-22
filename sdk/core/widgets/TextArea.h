#ifndef LVGLPP_TEXTAREA_H
#define LVGLPP_TEXTAREA_H

#include "BaseItem.h"
#include <string>

namespace lvglpp::widgets {

class TextArea : public BaseItem
{
public:
    explicit TextArea(BaseItem *parent = nullptr);

    /// Set the text content
    void setText(const std::string &text);

    /// Get the text content
    std::string text() const;

    /// Set placeholder text (shown when empty)
    void setPlaceholder(const std::string &text);

    /// Get placeholder text
    std::string placeholder() const;

    /// Set cursor position
    void setCursorPos(int32_t pos);

    /// Get cursor position
    uint32_t cursorPos() const;

    /// Set single-line mode
    void setOneLine(bool oneLine);

    /// Set password mode (characters hidden)
    void setPasswordMode(bool passwordMode);

    /// Set the bullet character for password mode
    void setPasswordBullet(const std::string &bullet);

    /// Set maximum text length in characters
    void setMaxLength(uint32_t maxLen);

    /// Set accepted characters (e.g. "0123456789." for numbers)
    void setAcceptedChars(const std::string &chars);

    /// Set text alignment
    void setAlignment(lv_text_align_t align);

    /// Get the label object inside the textarea
    lv_obj_t *labelObj() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TEXTAREA_H
