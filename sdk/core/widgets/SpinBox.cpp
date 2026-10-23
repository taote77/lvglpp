#include "core/base/LvObjFactory.h"

#include "SpinBox.h"

namespace lvglpp {
namespace widgets {

SpinBox::SpinBox(Widget *parent) : Widget(tools::LvObjFactory::createLvglSpinBox, parent)
{
    lv_spinbox_step_prev(lvObj());
    lv_style_init(&font_style_);
    //            lv_style_set_text_font(&font_style_,
    //                                   tools::TrueFontManager::getInstance()->getFontInfo(tools::TrueFontManager::DMSans,
    //                                                                                      32,
    //                                                                                      FT_FONT_STYLE_BOLD).font);
    lv_style_set_text_align(&font_style_, LV_TEXT_ALIGN_AUTO);
    lv_obj_add_style(lvObj(), &font_style_, LV_STATE_DEFAULT);
}

void SpinBox::setRange(int min, int max)
{
    lv_spinbox_set_range(lvObj(), min, max);
}

void SpinBox::setDigitFormat(int total, int intNum)
{
    lv_spinbox_set_digit_format(lvObj(), total, intNum);
}

void SpinBox::setVal(int val)
{
    lv_spinbox_set_value(lvObj(), val);
}

int SpinBox::getVal() const
{
    return lv_spinbox_get_value(lvObj());
}
} // namespace widgets
} // namespace lvglpp