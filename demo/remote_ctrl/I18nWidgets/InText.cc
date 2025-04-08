#include "InText.h"

#include "LvTranslator.h"

namespace lvglpp::widgets {

I18nText::I18nText(const std::string &strMsg, uint32_t clr, BaseItem *parent, TextStyle style, FontType type) : LvText(strMsg, clr, parent, style, type)
{}

I18nText::~I18nText()
{}

void I18nText::langTrigerred()
{
    auto msg = lvglpp::LvTranslator::getI18nText(LvText::getText().c_str());

    setText(msg);
}

} // namespace lvglpp::widgets
