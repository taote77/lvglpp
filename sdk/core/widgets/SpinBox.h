
#ifndef LVPP_SPINBOX_H
#define LVPP_SPINBOX_H

#include "BaseItem.h"

namespace lvglpp {
namespace widgets {

class SpinBox : public BaseItem
{
public:
    explicit SpinBox(BaseItem *parent = nullptr);

    void setRange(int min, int max);

    void setDigitFormat(int total, int intNum);

    void setVal(int val);

    int getVal() const;

private:
    lv_style_t font_style_{};
};

} // namespace widgets
} // namespace lvglpp

#endif // LVPP_SPINBOX_H
