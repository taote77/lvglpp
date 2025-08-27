#ifndef REBOOTCONFIRM_DIALOG_H
#define REBOOTCONFIRM_DIALOG_H

#include "core/widgets/BaseDialog.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/RoundedButton.h"

namespace lvglpp::ui::page {
using namespace lvglpp::widgets;

using ChangeDirCB  = std::function<void(const std::string &)>;
using HandleCancel = std::function<void(const std::string &)>;

class RebootConfirmDialog : public BaseDialog
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
    std::shared_ptr<BaseItem> dialog_bg_item_;

    std::shared_ptr<RoundedButton> bt_btn_;
    std::shared_ptr<RoundedButton> reboot_btn_;
    std::shared_ptr<RoundedButton> cancel_btn_;
    std::shared_ptr<ConsumeText>   title_label_;

    ChangeDirCB  change_dir_cb_;
    HandleCancel cancel_cb_;
};

} // namespace lvglpp::ui::page

#endif // REBOOTCONFIRM_DIALOG_H
