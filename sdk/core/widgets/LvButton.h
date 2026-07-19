#ifndef LVGLPP_LVBUTTON_H
#define LVGLPP_LVBUTTON_H

#include "core/widgets/LvText.h"
#include <string>

namespace lvglpp::widgets {

class LvButton : public LvText
{
public:
    LvButton(const std::string &strMsg, uint32_t clr, BaseItem *parent, TextStyle style = TextStyle::FontSize20, FontType type = FontType::Auto);

    ~LvButton();
};

// Modern alias (use PushButton/Button for styled buttons)
using TextButton = LvButton;

} // namespace lvglpp::widgets

#endif // LVGLPP_LVBUTTON_H