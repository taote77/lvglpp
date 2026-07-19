#ifndef PANEL_LCD_H
#define PANEL_LCD_H

#include "components/progress_bar.h"
#include "core/widgets/BaseDialog.h"
#include "core/widgets/LvText.h"
#include "core/widgets/Image.h"
#include "core/widgets/MouseArea.h"
#include "core/widgets/PushButton.h"
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

class LCDInfoCard : public BaseDialog
{
public:
    explicit LCDInfoCard(const double expose_time, const int32_t status);

    ~LCDInfoCard() override;

    void OnCancelClicked(HandleCancel cb)
    {
        _cancel_cb = std::move(cb);
    }

    void UpdateLCDInfo(const double expose_time, const int32_t status);

    void open() override;

    void close() override;

protected:
private:
    std::shared_ptr<BaseItem> _dialog_bg_item;

    std::shared_ptr<ConsumeText> _title_label;
    std::shared_ptr<ConsumeText> _lcd_state_label;

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

class PanelLCD : public BaseItem
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

    void updateScreenInfo(const double progress);

    explicit PanelLCD(BaseItem *parent = nullptr);

private:
    double _last_progress{-1.0F};

    std::shared_ptr<widgets::Image> _icon1; // row1 图标
    std::shared_ptr<widgets::Image> _icon2; // row2 图标
    std::shared_ptr<ConsumeText>    _text;  // row1 txt
    std::shared_ptr<ConsumeText>    _text1; // row2 txt

    std::shared_ptr<MouseArea>      _touch_area;
    std::shared_ptr<widgets::Image> _arrow_right;

    bool switch_enable{false};

    std::shared_ptr<wrapper::widgets::ProgressBar> _progress;

    std::shared_ptr<LCDInfoCard> _info_card;
};

} // namespace lvglpp::ui::page

#endif // PANEL_LCD_H
