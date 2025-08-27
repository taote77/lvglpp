
#ifndef MODULE_COLLECTOR_H
#define MODULE_COLLECTOR_H

#include "core/widgets/BaseItem.h"
#include "module_panel/PanelHeater.h"
#include "module_panel/PanelLCD.h"
#include "module_panel/PanelPulseMod.h"
#include "module_panel/PanelPulseTray.h"
#include "module_panel/PanelResinBottle.h"
#include "module_panel/PanelTempHumi.h"
#include <boost/format.hpp>

namespace lvglpp::ui::page {
using namespace widgets;

class ModuleCollector : public BaseItem
{
public:
    explicit ModuleCollector(BaseItem *parent = nullptr);

    ~ModuleCollector();

    void updateDeviceInfo(void *data);

protected:
    void init();

private:
    boost::signals2::connection _slots_fill_status;

    bool _fill_switch_status{false};

    bool _heat_switch_status{false};

    std::shared_ptr<PanelTempHumi> _panel_sensor;

    std::shared_ptr<PanelResinBottle> _panel_resin;

    std::shared_ptr<PanelPulseTray> _panel_pulse_tray;

    std::shared_ptr<PanelPusleMod> _panel_pulse_mod;

    std::shared_ptr<PanelHeater> _panel_heat;

    std::shared_ptr<PanelLCD> _panel_lcd_screen;
};

} // namespace lvglpp::ui::page

#endif // MODULE_COLLECTOR_H
