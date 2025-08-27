#ifndef MODULE_DETAIL_PULSE_DIALOG_H
#define MODULE_DETAIL_PULSE_DIALOG_H

#include "boost/progress_bar.h"
#include "core/widgets/BaseDialog.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/RoundedButton.h"

namespace lvglpp::ui::page {
using namespace lvglpp::widgets;
using namespace wrapper::widgets;

using ChangeDirCB  = std::function<void(const std::string &)>;
using HandleCancel = std::function<void(const std::string &)>;

class ModuleDetailPulseDialog : public BaseDialog
{
public:
    explicit ModuleDetailPulseDialog(const int progress, const int status);

    ~ModuleDetailPulseDialog() override;

    void updatePulseFilmInfo(double usedtime, int32_t tank_type, int32_t airmod_state);

    void OnCancelClicked(HandleCancel cb)
    {
        _cancel_cb = std::move(cb);
    }

    void open() override;

    void close() override;

protected:
private:
    std::shared_ptr<BaseItem> _dialog_bg_item;

    std::shared_ptr<ConsumeText> _title_label;

    std::shared_ptr<ConsumeText> _title_status_label;

    std::shared_ptr<ConsumeText> _progress_label;

    std::shared_ptr<ProgressBar> _progress_bar;

    std::shared_ptr<ConsumeText> _status_label;

    std::shared_ptr<ConsumeText> _status_value_label;

    std::shared_ptr<RoundedButton> _close_button;

    ChangeDirCB _change_dir_cb;

    HandleCancel _cancel_cb;
};

} // namespace lvglpp::ui::page

#endif // MODULE_DETAIL_PULSE_DIALOG_H
