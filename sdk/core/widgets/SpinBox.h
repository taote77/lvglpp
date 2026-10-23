
#ifndef LVGLPP_SPINBOX_H
#define LVGLPP_SPINBOX_H

#include "Widget.h"

namespace lvglpp {
namespace widgets {

class SpinBox : public Widget
{
public:
    explicit SpinBox(Widget *parent = nullptr);

    void setRange(int min, int max);

    void setDigitFormat(int total, int intNum);

    void setVal(int val);

    int getVal() const;

private:
    lv_style_t font_style_{};
};

} // namespace widgets
} // namespace lvglpp

#endif // LVGLPP_SPINBOX_H
