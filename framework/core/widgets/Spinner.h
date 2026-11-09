#ifndef LVGLPP_SPINNER_H
#define LVGLPP_SPINNER_H

#include "Widget.h"

namespace lvglpp::widgets {

class Spinner : public Widget
{
public:
    explicit Spinner(Widget *parent = nullptr);

    /// durationMs: one full turn; arcAngle: visible arc angle in degrees.
    Spinner &setAnimParams(uint32_t durationMs = 1000, uint32_t arcAngle = 60);
};

} // namespace lvglpp::widgets

#endif // LVGLPP_SPINNER_H
