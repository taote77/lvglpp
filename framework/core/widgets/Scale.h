#ifndef LVGLPP_SCALE_H
#define LVGLPP_SCALE_H

#include "Widget.h"

namespace lvglpp::widgets {

class Scale : public Widget
{
public:
    explicit Scale(Widget *parent = nullptr);

    Scale &setRange(int32_t min, int32_t max);

    Scale &setTotalTickCount(uint32_t count);

    Scale &setMajorTickEvery(uint32_t every);

    Scale &setLabelShow(bool show);

    Scale &setAngleRange(uint32_t degrees);

    Scale &setRotation(int32_t degrees);

    Scale &setMode(lv_scale_mode_t mode);
};

} // namespace lvglpp::widgets

#endif // LVGLPP_SCALE_H
