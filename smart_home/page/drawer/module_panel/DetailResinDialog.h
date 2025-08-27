#ifndef MODULE_DETAIL_RESIN_DIALOG_H
#define MODULE_DETAIL_RESIN_DIALOG_H

#include "core/widgets/BaseDialog.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/RoundedButton.h"
#include "core/widgets/SwitchButton.h"
#include <boost/signals2/signal.hpp>

namespace lvglpp::ui::page {
using namespace lvglpp::widgets;

using ChangeDirCB  = std::function<void(const std::string &)>;
using HandleCancel = std::function<void(const std::string &)>;

class ModuleDetailResinDialog : public BaseDialog
{
public:
    explicit ModuleDetailResinDialog(const std::string &resin_name, bool mod_switch);

    ~ModuleDetailResinDialog() override;

    void setSwitchStatus(bool mod_switch, bool equipped);

    void UpdateBottleInfo(const std::string &resin_name, const int32_t remaining, const int32_t bottlestate, bool fillmod_equipped);

    void OnCancelClicked(HandleCancel cb)
    {
        _cancel_cb = std::move(cb);
    }

    void open() override;

    void close() override;

private:
    // boost::signals2::connection _slots_fill_heat_status;

    std::shared_ptr<BaseItem> _dialog_bg_item;

    std::shared_ptr<ConsumeText> _material_name_label;
    std::shared_ptr<ConsumeText> _status_label;
    std::shared_ptr<ConsumeText> _remaing_info;

    std::shared_ptr<ConsumeText>           _btn_label;
    std::shared_ptr<widgets::SwitchButton> _auto_fill_switch;

    std::shared_ptr<RoundedButton> _close_button;

    bool _fill_mod_equipped{false};

    ChangeDirCB _change_dir_cb;

    HandleCancel _cancel_cb;
};

} // namespace lvglpp::ui::page

#endif // MODULE_DETAIL_RESIN_DIALOG_H
