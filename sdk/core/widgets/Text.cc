#include "Text.h"

namespace lvglpp {
namespace widgets {

Text::Text(const std::string &strMsg, uint32_t clr, BaseItem *parent, LvText::TextStyle style, FontType type) : lvglpp::widgets::LvText(strMsg, clr, parent, style, type)
{}

} // namespace widgets
} // namespace lvglpp