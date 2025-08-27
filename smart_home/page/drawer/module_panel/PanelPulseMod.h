#ifndef MODULE_PULSE_MOD_H
#define MODULE_PULSE_MOD_H

#include "DetailPulseDialog.h"
#include "boost/progress_bar.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/Image.h"
#include "core/widgets/MouseArea.h"
#include <boost/format.hpp>
#include <boost/signals2/signal.hpp>
#include <cstdint>

namespace lvglpp::ui::page {
using namespace widgets;

class PanelPusleMod : public BaseItem
{
public:
    bool mod_switch{false};

    void updatePulseFilmLife(double inflator_worktime, int32_t tanktype, int32_t airmod_state);

    explicit PanelPusleMod(BaseItem *parent = nullptr);

private:
    double  _last_usesdtime{-1.0F};
    int32_t _last_tanktype{-1};
    int32_t _last_airmod_state{-1};

    std::shared_ptr<widgets::Image> _icon1_mod_pulse; // row1 图标
    std::shared_ptr<ConsumeText>    _label_mod_pulse; // row1 txt

    std::shared_ptr<widgets::Image> _icon2;        // row2 图标
    std::shared_ptr<ConsumeText>    _label_status; // row2 txt

    std::shared_ptr<MouseArea>      _touch_area;
    std::shared_ptr<widgets::Image> _arrow_right;

    std::shared_ptr<ModuleDetailPulseDialog> _pulse_detail_dialog;

    std::shared_ptr<wrapper::widgets::ProgressBar> _progress;
};

} // namespace lvglpp::ui::page

#endif // MODULE_PULSE_MOD_H
