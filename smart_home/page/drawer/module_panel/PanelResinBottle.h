#ifndef MODULE_RESIN_BOTTLE_H
#define MODULE_RESIN_BOTTLE_H

#include "DetailResinDialog.h"
#include "components/progress_bar.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/Image.h"
#include "core/widgets/MouseArea.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace lvglpp::ui::page {
using namespace widgets;

class PanelResinBottle : public BaseItem
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

    void SwitchState(bool state, bool equipped);

    void updateBottleInfo(const std::string &material, const int32_t tank_size, const int32_t remaining, const int32_t bottlestate, const int32_t printerthsensorstate,
                          bool fill_equipped);

    explicit PanelResinBottle(BaseItem *parent = nullptr);

private:
    bool _switch_enable{false};
    bool _fill_mod_equip{false};

    std::string _last_material;
    int32_t     _last_remaining{-1};
    int32_t     _last_tank_size{-1};
    int32_t     _last_bottlestate{-1};
    int32_t     _last_printerthsensorstate{-1};
    bool        _last_fill_equipped{false};

    std::shared_ptr<widgets::Image> _icon_bottle;       // row1 图标
    std::shared_ptr<widgets::Image> _icon2;             // row2 图标
    std::shared_ptr<ConsumeText>    _lbl_material_name; // row1 txt
    std::shared_ptr<ConsumeText>    _lbl_bottle_status; // row2 txt

    std::shared_ptr<MouseArea>      _touch_area;
    std::shared_ptr<widgets::Image> _arrow_right;

    std::shared_ptr<ModuleDetailResinDialog> _resin_detail_dialog;

    std::shared_ptr<wrapper::widgets::ProgressBar> _progress;

    bool _is_pro{true};
};

} // namespace lvglpp::ui::page

#endif // MODULE_RESIN_BOTTLE_H
