#ifndef LVGLPP_TEXT_H
#define LVGLPP_TEXT_H

#include "core/widgets/LvText.h"

namespace lvglpp::widgets {

class Text : public LvText
{
public:
    Text(const std::string         &strMsg,                        //
         uint32_t                   clr,                           //
         lvglpp::widgets::BaseItem *parent,                        //
         TextStyle                  style = TextStyle::FontSize20, //
         FontType                   type  = FontType::Auto);

    void setText(const std::string &text);

private:
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TEXT_H