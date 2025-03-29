#ifndef _LVPP_BIND_TEXT_H_
#define _LVPP_BIND_TEXT_H_

#include "core/widgets/LvText.h"

namespace lvglpp {
namespace widgets {

class Text : public LvText
{
public:
    Text(const std::string &strMsg, uint32_t clr, lvglpp::widgets::BaseItem *parent, TextStyle style = TextStyle::FontSize20, FontType type = FontType::Auto);

private:
};

} // namespace widgets
} // namespace lvglpp

#endif // _LVPP_BIND_TEXT_H_