#ifndef PANEL_TEMP_HUMI_H
#define PANEL_TEMP_HUMI_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/Image.h"
#include <boost/format.hpp>
#include <boost/signals2/signal.hpp>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lvglpp::ui::page {
using namespace widgets;

class PanelTempHumi : public BaseItem
{
public:
    void updateSensor(int32_t senser_state, int32_t temp, int32_t humi);

    explicit PanelTempHumi(BaseItem *parent = nullptr);

private:
    std::shared_ptr<widgets::Image> _icon1; // row1 图标
    std::shared_ptr<widgets::Image> _icon2; // row2 图标
    std::shared_ptr<ConsumeText>    _text;  // row1 txt
    std::shared_ptr<ConsumeText>    _text1; // row2 txt

    int32_t last_sensor{-1};
    int32_t temp_last{-1};
    int32_t humi_last{-1};
};

} // namespace lvglpp::ui::page

#endif // PANEL_TEMP_HUMI_H
