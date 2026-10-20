#ifndef LVGLPP_SPINNER_H
#define LVGLPP_SPINNER_H

#include "BaseItem.h"

namespace lvglpp::widgets {

class Spinner : public BaseItem
{
public:
    explicit Spinner(BaseItem *parent = nullptr);

    /// durationMs: one full turn; arcAngle: visible arc angle in degrees.
    Spinner &setAnimParams(uint32_t durationMs = 1000, uint32_t arcAngle = 60);
};

} // namespace lvglpp::widgets

#endif // LVGLPP_SPINNER_H
