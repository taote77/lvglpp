#include "TextButton.h"

namespace lvglpp::widgets {

TextButton::TextButton(const std::string &strMsg, uint32_t clr, Widget *parent, TextStyle style, FontType type) : Label(strMsg, clr, parent, style, type)
{
    setFocusable(true);
}

TextButton::~TextButton()
{
    //
    //
}

} // namespace lvglpp::widgets
