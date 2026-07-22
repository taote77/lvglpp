#include "Led.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

Led::Led(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglLed, parent)
{}

void Led::on()
{
    lv_led_on(getLvglItem());
}

void Led::off()
{
    lv_led_off(getLvglItem());
}

void Led::setBrightness(uint8_t brightness)
{
    lv_led_set_brightness(getLvglItem(), brightness);
}

uint8_t Led::brightness() const
{
    return lv_led_get_brightness(getLvglItem());
}

void Led::toggle()
{
    lv_led_toggle(getLvglItem());
}

bool Led::isOn() const
{
    return brightness() > 0;
}

} // namespace lvglpp::widgets
