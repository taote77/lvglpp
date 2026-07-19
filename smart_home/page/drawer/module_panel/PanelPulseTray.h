#ifndef MODULE_PULSE_TRAY_H
#define MODULE_PULSE_TRAY_H

// #include "DetailPulseTrayDialog.h"
#include "boost/progress_bar.h"
#include "core/widgets/BaseDialog.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/Image.h"
#include "core/widgets/MouseArea.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/SwitchButton.h"
#include <boost/format.hpp>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace lvglpp::ui::page {
using namespace widgets;
using namespace lvglpp::widgets;
using namespace wrapper::widgets;

using ChangeDirCB  = std::function<void(const std::string &)>;
using HandleCancel = std::function<void(const std::string &)>;

class ModuleDetailPulseTrayDialog : public BaseDialog
{
public:
    explicit ModuleDetailPulseTrayDialog(const double progress, const int status);

    ~ModuleDetailPulseTrayDialog() override;

    void OnCancelClicked(HandleCancel cb)
    {
        _cancel_cb = std::move(cb);
    }

    void UpdatePulseTrayInfo(const double progress, const int32_t resin_level, const int32_t tank_type, const int tank_state, const bool sensor_resin, const int32_t remain_resin,
                             const int32_t resin_temp);

    void open() override;

    void close() override;

protected:
private:
    std::shared_ptr<BaseItem> _dialog_bg_item;

    std::shared_ptr<ConsumeText> _title_label;
    std::shared_ptr<ConsumeText> _tray_state_label;

    // ---
    std::shared_ptr<ConsumeText> _progress_label;
    std::shared_ptr<ProgressBar> _progress_bar;
    std::shared_ptr<ConsumeText> _progress_bar_replace;
    std::shared_ptr<ConsumeText> _warn_lbl;

    // ---
    std::shared_ptr<ConsumeText> _status_label;
    std::shared_ptr<ConsumeText> _resin_remain_label;

    // 温度
    std::shared_ptr<ConsumeText> _temperature_tag_label;
    std::shared_ptr<ConsumeText> _temperature_value_label;

    // ---
    std::shared_ptr<BaseItem>    _warn_bar_bg;
    std::shared_ptr<Image>       _warn_icon;
    std::shared_ptr<ConsumeText> _warn_bar_0;
    std::shared_ptr<ConsumeText> _warn_bar_1;
    // ---
    std::shared_ptr<RoundedButton> _close_button;

    ChangeDirCB _change_dir_cb;

    HandleCancel _cancel_cb;
};

class PanelPulseTray : public BaseItem
{
public:
    static std::string double_to_one_digit_str(const double val, const short precision = 1)
    {
        const double EPSILON = 1e-9;
        if (std::abs(val) < EPSILON)
        {
            return std::string("0.0");
        }
        std::ostringstream oss;
        oss << std::scientific << std::setprecision(precision) << val;
        return oss.str();
    }

    void updateTrayInfo(const int32_t peeled_count, const int32_t resin_level, const int32_t tank_type, const int32_t tank_state, const bool sensor_resin,
                        const int32_t remain_resin, const int32_t resin_temperature);

    explicit PanelPulseTray(BaseItem *parent = nullptr);

private:
    int32_t _last_peeled_count{-1};
    int32_t _last_tank_type{-1};
    int32_t _last_tank_state{-1};
    int32_t _last_resin_level{-1};
    int32_t _last_resin_remain{-1};
    int32_t _last_resin_temperature{-1};

    bool _last_sensor_resin{false};

    double _lastest_percent{0.0};

    std::shared_ptr<widgets::Image> _icon_tray;        // row1 图标
    std::shared_ptr<widgets::Image> _icon2;            // row2 图标
    std::shared_ptr<ConsumeText>    _label_tray_text;  // row1 txt
    std::shared_ptr<ConsumeText>    _label_mod_status; // row2 txt

    std::shared_ptr<MouseArea>      _touch_area;
    std::shared_ptr<widgets::Image> _arrow_right;

    std::shared_ptr<ModuleDetailPulseTrayDialog> _pulse_tray_detail_dialog;

    bool switch_enable{false};

    std::shared_ptr<wrapper::widgets::ProgressBar> _progress;
};

} // namespace lvglpp::ui::page

#endif // MODULE_PULSE_TRAY_H
