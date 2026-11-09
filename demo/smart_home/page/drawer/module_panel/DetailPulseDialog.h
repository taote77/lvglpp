#ifndef MODULE_DETAIL_PULSE_DIALOG_H
#define MODULE_DETAIL_PULSE_DIALOG_H

#include "components/progress_bar.h"
#include "core/widgets/Dialog.h"
#include "core/widgets/Label.h"
#include "core/widgets/PushButton.h"

namespace lvglpp::ui::page {
using namespace lvglpp::widgets;
using namespace wrapper::widgets;

using ChangeDirCB  = std::function<void(const std::string &)>;
using HandleCancel = std::function<void(const std::string &)>;

class ModuleDetailPulseDialog : public Dialog
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
    std::shared_ptr<Widget> _dialog_bg_item;

    std::shared_ptr<Label> _title_label;

    std::shared_ptr<Label> _title_status_label;

    std::shared_ptr<Label> _progress_label;

    std::shared_ptr<ProgressBar> _progress_bar;

    std::shared_ptr<Label> _status_label;

    std::shared_ptr<Label> _status_value_label;

    std::shared_ptr<PushButton> _close_button;

    ChangeDirCB _change_dir_cb;

    HandleCancel _cancel_cb;
};

} // namespace lvglpp::ui::page

#endif // MODULE_DETAIL_PULSE_DIALOG_H
