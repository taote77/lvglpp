#ifndef INTEXT_H
#define INTEXT_H

#include "core/widgets/LvText.h"

namespace lvglpp::widgets {

using namespace lvglpp::widgets;

class I18nText : public LvText
{
public:
    I18nText(const std::string &strMsg, uint32_t clr, BaseItem *parent, TextStyle style = TextStyle::FontSize20, FontType type = FontType::Auto);

    ~I18nText() override;

    void langTrigerred();
};

} // namespace lvglpp::widgets

#endif // INTEXT_H