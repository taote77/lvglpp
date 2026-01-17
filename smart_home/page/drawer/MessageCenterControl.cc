#include "MessageCenterControl.h"
#include "core/log/log.h"
// #include "page/FrontendPersistence.h"
#include "page/drawer/NoticeDelegate.h"
#include "theme/form_design.h"
#include <vector>

namespace lvglpp::ui::page {

using namespace form;
using namespace form::design;

static constexpr int container_width  = 672;
static constexpr int container_height = 368;
MessageCenterControl::MessageCenterControl(BaseItem *parent) : BaseItem(parent)
{
    this->setSize(container_width, container_height);
    this->setOpacity(1);

    this->setEnable(true);
    init();
}

void MessageCenterControl::udiskChange(bool state)
{
    if (_udisk_status)
    {
        _udisk_status->setIcon(state ? ":/image/32x32/udisk.png" : ":/image/32x32/udisk_dis.png");
    }
}

void MessageCenterControl::cloudChange(bool state)
{
    if (_cloud_status)
    {
        _cloud_status->setIcon(state ? ":/image/32x32/cloud.png" : ":/image/32x32/cloud_off.png");
    }
}

void MessageCenterControl::wifiChange(bool state)
{
    if (_wifi_status)
    {
        _wifi_status->setIcon(state ? ":/image/32x32/wifi.png" : ":/image/32x32/wifi_off.png");
    }
}

void MessageCenterControl::init()
{
    constexpr short tool_bar_width  = 104;
    constexpr short tool_bar_height = 368;

    _reboot_confirm_dialog = std::make_shared<RebootConfirmDialog>();
    // _reboot_confirm_dialog->setAutoDel(false);

    this->setOpacity(1);
    _toolbar_layout = std::make_shared<Layout>(this);
    _toolbar_layout->setSize(tool_bar_width, tool_bar_height);

    _toolbar_layout->setOpacity(1);
    _toolbar_layout->setBgColor(design::ThemeColor(CLR_SURFACE_CONTAINER));
    _toolbar_layout->setPos(0, 0);

    _toolbar_layout->setRadius(8);
    lv_obj_set_style_pad_hor(_toolbar_layout->getLvglItem(), 24, LV_STATE_DEFAULT);

    _wifi_status = std::make_shared<StatusIcon>(":/image/32x32/wifi.png", _toolbar_layout.get());
    _wifi_status->setOpacity(1);
    _wifi_status->setPos(10, 10);

    lv_obj_center(_wifi_status->getLvglItem());

    _cloud_status = std::make_shared<StatusIcon>(":/image/32x32/cloud.png", _toolbar_layout.get());
    lv_obj_center(_cloud_status->getLvglItem());

    bool udisk_mounted = true;
    _udisk_status      = std::make_shared<StatusIcon>(udisk_mounted ? ":/image/32x32/udisk.png" : ":/image/32x32/usb_dis.png", _toolbar_layout.get());
    lv_obj_center(_udisk_status->getLvglItem());

    _reboot_status = std::make_shared<StatusIcon>(":/image/32x32/reboot.png", _toolbar_layout.get());
    lv_obj_center(_reboot_status->getLvglItem());

    _wifi_status->enable(true);
    _reboot_status->enable(true);
    _reboot_status->OnClick([this]() -> void {
        LOG_INFO() << "reboot_status reboot";
        _reboot_confirm_dialog->open();
    });

    _listview = std::make_shared<ListView<NoticeData, lDelegate::NoticeDelegate>>(this);
    _listview->setSize(container_width - tool_bar_width, container_height);
    _listview->setOpacity(1);

    _listview->setPos(tool_bar_width + 24, 0);
    _listview->setSpan(8);

    std::vector<NoticeData> notice_vec;

    _listview->setModel(BaseModel<NoticeData>(notice_vec));
    _listview->update();
    LOG_DEBUG() << "notice suize: " << _listview->getModel()->count();
}

} // namespace lvglpp::ui::page
