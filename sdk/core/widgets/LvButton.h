#ifndef _LVPP_BUTTON_H
#define _LVPP_BUTTON_H

#include "core/widgets/LvText.h"
#include <string>

namespace lvglpp {

namespace widgets {

class LvButton : public LvText
{
public:
    LvButton(const std::string &strMsg, uint32_t clr, BaseItem *parent, TextStyle style = TextStyle::FontSize20, FontType type = FontType::Auto);

    ~LvButton();
};

} // namespace widgets
} // namespace lvglpp

#endif // _LVPP_BUTTON_H