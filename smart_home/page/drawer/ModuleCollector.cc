#include "ModuleCollector.h"
#include "core/log/log.h"

namespace lvglpp::ui::page {
static constexpr int container_width  = 672;
static constexpr int container_height = 368;
ModuleCollector::ModuleCollector(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(container_width, container_height);
    this->setOpacity(1);

    lv_obj_set_flex_flow(this->getLvglItem(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_column(this->getLvglItem(), 8, LV_STATE_DEFAULT);

    init();
}

ModuleCollector::~ModuleCollector()
{
    _slots_fill_status.disconnect();
}

void ModuleCollector::updateDeviceInfo(void *data)
{}

void ModuleCollector::init()
{
    _panel_sensor = std::make_shared<PanelTempHumi>(this);

    _panel_resin = std::make_shared<PanelResinBottle>(this);

    _panel_pulse_tray = std::make_shared<PanelPulseTray>(this);

    _panel_pulse_mod = std::make_shared<PanelPusleMod>(this);

    _panel_heat = std::make_shared<PanelHeater>(this);

    _panel_lcd_screen = std::make_shared<PanelLCD>(this);
}

} // namespace lvglpp::ui::page
