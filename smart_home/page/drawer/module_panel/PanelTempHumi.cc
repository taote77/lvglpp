#include "PanelTempHumi.h"
#include "core/log/log.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {
using namespace lvglpp;
using namespace form::design;

void PanelTempHumi::updateSensor(int32_t senser_state, int32_t temp, int32_t humi)
{
    if (last_sensor != senser_state || temp_last != temp || humi_last != humi)
    {
        if (last_sensor == 1)
        {
            char buf[64] = {};

            if (temp != temp_last)
            {
                sprintf(buf, "%.1f", static_cast<double>(temp) / 10000.0);
                _text->setText(std::string(buf) + "℃");
            }

            if (humi_last != humi)
            {
                sprintf(buf, "%.1f", static_cast<double>(humi) / 100.0);
                _text1->setText(std::string(buf) + "RH%");
                humi_last = humi;
            }
        } else
        {
            _text->setText(lvTrStr("EMPTY_INFO") + "℃");
            _text1->setText(lvTrStr("EMPTY_INFO") + "RH%");
        }

        last_sensor = senser_state;
        humi_last   = humi;
        temp_last   = temp;
    }
}

PanelTempHumi::PanelTempHumi(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(331, 115);
    this->setOpacity(1);
    this->setBgColor(ThemeColor(CLR_SURFACE_CONTAINER_HIGHEST));
    this->setRadius(8);

    constexpr short offset{40};
    constexpr short mod_item_edge_x{16};
    constexpr short mod_item_edge_row2_y{60};

    _text  = std::make_shared<ConsumeText>("", CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
    _text1 = std::make_shared<ConsumeText>("", CLR_ON_SURFACE, this, ConsumeText::FontSize22Bold);
    _text->setPos(mod_item_edge_x + offset, 16);
    _text1->setPos(mod_item_edge_x + offset, 60);

    _icon1 = std::make_shared<widgets::Image>(":/image/32x32/mod_temperature.png", this);
    _icon1->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(18));

    _icon2 = std::make_shared<widgets::Image>(":/image/32x32/mod_humidity.png", this);
    _icon2->setPos(lv_coord_t(mod_item_edge_x), lv_coord_t(mod_item_edge_row2_y + 5));

    updateSensor(0, 0, 0);
}

} // namespace lvglpp::ui::page
