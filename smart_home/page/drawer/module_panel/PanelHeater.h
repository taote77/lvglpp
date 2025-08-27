#ifndef PANEL_HEATER_H
#define PANEL_HEATER_H

#include "boost/progress_bar.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/Image.h"
#include "core/widgets/MouseArea.h"
#include <boost/format.hpp>
#include <boost/signals2/signal.hpp>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lvglpp::ui::page {
using namespace widgets;

class PanelHeater : public BaseItem
{
public:
    static std::string double_to_one_digit_str(const double x, const short precision = 1)
    {
        const double EPSILON = 1e-9;
        if (std::abs(x) < EPSILON)
        {
            return std::string("0.0");
        }
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(precision) << x;
        return oss.str();
    }

    void SwitchState(bool state)
    {
        switch_enable = state;
    }

    void updateHeatStatus(int32_t scraperstate, int32_t scrapertype, int32_t resin_temp, bool resin_sensor, bool heater_equipped);

    explicit PanelHeater(BaseItem *parent = nullptr);

private:
    bool switch_enable{false};

    int32_t _last_scraperstate{-1};
    int32_t _last_scrapertype{-1};
    int32_t _last_resin_temp{-1};
    bool    _resin_sensor{false};
    bool    _last_heater_equipped{false};

    std::shared_ptr<widgets::Image> _icon1;             // row1 图标
    std::shared_ptr<ConsumeText>    _label_mod_scraper; // row1 txt

    // std::shared_ptr<widgets::Image> _icon2;// row2 图标
    std::shared_ptr<ConsumeText> _label_scraper_state; // row2 txt

    std::shared_ptr<MouseArea>      _touch_area;
    std::shared_ptr<widgets::Image> _arrow_right;

    std::shared_ptr<wrapper::widgets::ProgressBar> _progress;
};

} // namespace lvglpp::ui::page

#endif // PANEL_HEATER_H
