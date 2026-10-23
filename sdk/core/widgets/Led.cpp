#include "Led.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Led::Led(Widget *parent)
    : Widget(tools::LvObjFactory::createLvglLed, parent)
{}

void Led::on()
{
    lv_led_on(lvObj());
}

void Led::off()
{
    lv_led_off(lvObj());
}

void Led::setBrightness(uint8_t brightness)
{
    lv_led_set_brightness(lvObj(), brightness);
}

uint8_t Led::brightness() const
{
    return lv_led_get_brightness(lvObj());
}

void Led::toggle()
{
    lv_led_toggle(lvObj());
}

bool Led::isOn() const
{
    return brightness() > 0;
}

} // namespace lvglpp::widgets
