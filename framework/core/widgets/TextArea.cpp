#include "TextArea.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

TextArea::TextArea(Widget *parent)
    : Widget(tools::LvObjFactory::createLvglTextArea, parent)
{
    setFocusable(true);
}

void TextArea::setText(const std::string &text)
{
    lv_textarea_set_text(lvObj(), text.c_str());
}

std::string TextArea::text() const
{
    const char *t = lv_textarea_get_text(lvObj());
    return t ? std::string(t) : std::string();
}

void TextArea::setPlaceholder(const std::string &text)
{
    lv_textarea_set_placeholder_text(lvObj(), text.c_str());
}

std::string TextArea::placeholder() const
{
    const char *t = lv_textarea_get_placeholder_text(lvObj());
    return t ? std::string(t) : std::string();
}

void TextArea::setCursorPos(int32_t pos)
{
    lv_textarea_set_cursor_pos(lvObj(), pos);
}

uint32_t TextArea::cursorPos() const
{
    return lv_textarea_get_cursor_pos(lvObj());
}

void TextArea::setOneLine(bool oneLine)
{
    lv_textarea_set_one_line(lvObj(), oneLine);
}

void TextArea::setPasswordMode(bool passwordMode)
{
    lv_textarea_set_password_mode(lvObj(), passwordMode);
}

void TextArea::setPasswordBullet(const std::string &bullet)
{
    lv_textarea_set_password_bullet(lvObj(), bullet.c_str());
}

void TextArea::setMaxLength(uint32_t maxLen)
{
    lv_textarea_set_max_length(lvObj(), maxLen);
}

void TextArea::setAcceptedChars(const std::string &chars)
{
    lv_textarea_set_accepted_chars(lvObj(), chars.c_str());
}

void TextArea::setAlignment(lv_text_align_t align)
{
    lv_textarea_set_align(lvObj(), align);
}

lv_obj_t *TextArea::labelObj() const
{
    return lv_textarea_get_label(lvObj());
}

} // namespace lvglpp::widgets
