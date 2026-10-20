#include "LvButton.h"

namespace lvglpp::widgets {

LvButton::LvButton(const std::string &strMsg, uint32_t clr, BaseItem *parent, TextStyle style, FontType type) : LvText(strMsg, clr, parent, style, type)
{
    setFocusable(true);
}

LvButton::~LvButton()
{
    //
    //
}

} // namespace lvglpp::widgets
