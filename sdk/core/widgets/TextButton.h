#ifndef LVGLPP_TEXTBUTTON_H
#define LVGLPP_TEXTBUTTON_H

#include "core/widgets/Label.h"
#include <string>

namespace lvglpp::widgets {

class TextButton : public Label
{
public:
    TextButton(const std::string &strMsg, uint32_t clr, Widget *parent, TextStyle style = TextStyle::FontSize20, FontType type = FontType::Auto);

    ~TextButton();
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TEXTBUTTON_H
