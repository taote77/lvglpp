#ifndef REBOOTCONFIRM_DIALOG_H
#define REBOOTCONFIRM_DIALOG_H

#include "core/widgets/Dialog.h"
#include "core/widgets/Label.h"
#include "core/widgets/PushButton.h"

namespace lvglpp::ui::page {
using namespace lvglpp::widgets;

using ChangeDirCB  = std::function<void(const std::string &)>;
using HandleCancel = std::function<void(const std::string &)>;

class RebootConfirmDialog : public Dialog
{
public:
    RebootConfirmDialog();

    ~RebootConfirmDialog() override;

    void OnCancelClicked(HandleCancel cb)
    {
        cancel_cb_ = cb;
    }

    void open() override;

    void close() override;

protected:
private:
    std::shared_ptr<Widget> dialog_bg_item_;

    std::shared_ptr<PushButton> bt_btn_;
    std::shared_ptr<PushButton> reboot_btn_;
    std::shared_ptr<PushButton> cancel_btn_;
    std::shared_ptr<Label>   title_label_;

    ChangeDirCB  change_dir_cb_;
    HandleCancel cancel_cb_;
};

} // namespace lvglpp::ui::page

#endif // REBOOTCONFIRM_DIALOG_H
