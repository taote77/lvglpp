#ifndef LVGLPP_LED_H
#define LVGLPP_LED_H

#include "Widget.h"

namespace lvglpp::widgets {

class Led : public Widget
{
public:
    explicit Led(Widget *parent = nullptr);

    /// Turn LED on (full brightness)
    void on();

    /// Turn LED off
    void off();

    /// Set brightness (0-255)
    void setBrightness(uint8_t brightness);

    /// Get brightness
    uint8_t brightness() const;

    /// Toggle LED state
    void toggle();

    /// Check if LED is on (brightness > 0)
    bool isOn() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_LED_H
