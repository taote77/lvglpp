#include "RebootConfirmDialog.h"
#include "core/log/log.h"
#include "core/sys/StackView.h"
#include "core/sys/UiConfig.h"
#include "core/tools/Utils.h"
#include "core/widgets/Toast.h"
#include "i18n/LvTranslator.h"
#include "theme/theme_keeper.h"

namespace lvglpp::ui::page {
using namespace lvglpp::tools;
using namespace form;
using namespace lvglpp;

RebootConfirmDialog::RebootConfirmDialog() : BaseDialog(true), change_dir_cb_(nullptr), cancel_cb_(nullptr)
{
    const auto &cfg      = sys::UiConfig::instance();
    const bool  portrait = cfg.isPortrait();

    dialog_bg_item_ = std::make_shared<Widget>(this->getRoot());
    dialog_bg_item_->setSize(cfg.width(), cfg.height());
    dialog_bg_item_->setRadius(16);

    dialog_bg_item_->setBgColor(design::ThemeColor(design::CLR_SURFACE));
    dialog_bg_item_->setAlignment(LV_ALIGN_CENTER, 0, 0);
    dialog_bg_item_->setOpacity(1.0);

    title_label_ = std::make_shared<Label>(lvTr("restart_tips"), design::ThemeColor(design::CLR_ON_SURFACE), dialog_bg_item_.get(), Label::FontSize32Bold);
    title_label_->setAlignment(LV_ALIGN_TOP_MID, 0, 170);

    bt_btn_ = std::make_shared<PushButton>(200, 64, PushButton::Dark, lvTr("common_bootloader"), dialog_bg_item_.get());
    bt_btn_->setBgColor(design::ThemeColor(design::CLR_SECONDARY_CONTAINER));
    bt_btn_->setAlignment(LV_ALIGN_TOP_MID, portrait ? 0 : -250, portrait ? 300 : 320);
    bt_btn_->setOnClickedListener([]() -> void {
        // todo
    });

    reboot_btn_ = std::make_shared<PushButton>(200, 64, PushButton::Red, lvTr("click_reboot"), dialog_bg_item_.get());
    reboot_btn_->setAlignment(LV_ALIGN_TOP_MID, 0, portrait ? 390 : 320);
    reboot_btn_->setOnClickedListener([]() -> void {
        LogDebug << "====reboot====";
        ::system("reboot");
    });

    cancel_btn_ = std::make_shared<PushButton>(200, 64, PushButton::Dark, lvTr("click_cancel"), dialog_bg_item_.get());
    cancel_btn_->setBgColor(design::ThemeColor(design::CLR_SECONDARY));
    cancel_btn_->setAlignment(LV_ALIGN_TOP_MID, portrait ? 0 : 250, portrait ? 480 : 320);
    cancel_btn_->setOnClickedListener([this]() -> void {
        LogDebug << "====cancel====";
        this->close();
        // StackView::getInstance().popViewImmediately();
    });
}

RebootConfirmDialog::~RebootConfirmDialog()
{}

void RebootConfirmDialog::open()
{
    BaseDialog::open();
    cancel_btn_->setEnabled(true);
}

void RebootConfirmDialog::close()
{
    BaseDialog::close();
}

} // namespace lvglpp::ui::page
