#include "LvButton.h"

namespace lvglpp {

namespace widgets {

LvButton::LvButton(const std::string &strMsg, uint32_t clr, BaseItem *parent, TextStyle style,
                   FontType type)
    : LvText(strMsg, clr, parent, style, type)
{
    ///
    //
}

LvButton::~LvButton()
{
    //
    //
}

} // namespace widgets
} // namespace lvglpp
