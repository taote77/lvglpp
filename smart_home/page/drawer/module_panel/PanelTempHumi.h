#ifndef PANEL_TEMP_HUMI_H
#define PANEL_TEMP_HUMI_H

#include "core/widgets/Widget.h"
#include "core/widgets/Label.h"
#include "core/widgets/Image.h"
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lvglpp::ui::page {
using namespace widgets;

class PanelTempHumi : public Widget
{
public:
    void updateSensor(int32_t senser_state, int32_t temp, int32_t humi);

    explicit PanelTempHumi(Widget *parent = nullptr);

private:
    std::shared_ptr<widgets::Image> _icon1; // row1 图标
    std::shared_ptr<widgets::Image> _icon2; // row2 图标
    std::shared_ptr<Label>    _text;  // row1 txt
    std::shared_ptr<Label>    _text1; // row2 txt

    int32_t last_sensor{-1};
    int32_t temp_last{-1};
    int32_t humi_last{-1};
};

} // namespace lvglpp::ui::page

#endif // PANEL_TEMP_HUMI_H
