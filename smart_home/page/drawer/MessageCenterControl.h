#ifndef MESSAGE_CENTER_H
#define MESSAGE_CENTER_H

#include "NoticeDelegate.h"
#include "RebootConfirmDialog.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/Image.h"
#include "core/widgets/ListView.hpp"
#include "theme/form_design.h"
#include <memory>
#include <string>

namespace lvglpp::ui::page {
using namespace widgets;
using namespace lvglpp::ui::lDelegate;

using namespace form;

class Layout : public BaseItem
{
public:
    /// 84 20  364 24
    explicit Layout(BaseItem *parent = nullptr) : BaseItem(parent)
    {
        // this->setOpacity(1);

        // lv_obj_set_flex_flow(this->getLvglItem(), LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_flex_flow(this->getLvglItem(), LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(this->getLvglItem(),
                              LV_FLEX_ALIGN_SPACE_EVENLY, // 主轴对齐方式
                              LV_FLEX_ALIGN_CENTER,       // 交叉轴居中对齐（可选）
                              LV_FLEX_ALIGN_CENTER);
    }

    void SetColSpage(short space)
    {
        // lv_obj_set_style_pad_column(this->getLvglItem(), space, LV_STATE_DEFAULT);
    }
};

class StatusIcon : public BaseItem
{
public:
    void update(bool status)
    {
        if (status)
        {
            _base->setBgColor(design::ThemeColor(design::CLR_PRIMARY)); // blue
        } else
        {
            _base->setBgColor(design::ThemeColor(design::CLR_SECONDARY)); // gray
        }
    }

    void setIcon(const std::string &img_src)
    {
        if (_icon)
        {
            _icon->setImageSrcPath(img_src);
        }
    }

    void enable(bool enable) // touch enable
    {
        _enabled = enable;
        _icon->setEnable(enable);
    }

    void OnClick(const std::function<void()> &clicked_cb)
    {
        _icon->setOnClickedListener(clicked_cb);
    }

    explicit StatusIcon(const std::string &icon_path, BaseItem *parent = nullptr) : BaseItem(parent)
    {
        this->setOpacity(1);
        constexpr short base_length{64};

        this->setSize(base_length, base_length);

        this->setRadius(base_length / 2);

        _base = std::make_shared<widgets::BaseItem>(this);
        _base->setSize(base_length, base_length);
        _base->setRadius(base_length / 2);
        _base->setBgColor(design::ThemeColor(design::CLR_SECONDARY));
        _base->setOpacity(1);

        _icon = std::make_shared<widgets::Image>(icon_path, this);
        _icon->setAligmentTo(*_base, LV_ALIGN_CENTER, 0, 0);
        // _icon->setOpacity(1);
    }

private:
    bool _status{false};
    bool _enabled{false};

    std::shared_ptr<widgets::BaseItem> _base;
    std::shared_ptr<widgets::Image>    _icon;
};

class MessageCenterControl : public BaseItem
{
public:
    explicit MessageCenterControl(BaseItem *parent = nullptr);

    void udiskChange(bool state);

    void cloudChange(bool state);

    void wifiChange(bool state);

protected:
    void init();

private:
    std::shared_ptr<Layout>     _toolbar_layout;
    std::shared_ptr<StatusIcon> _wifi_status;
    std::shared_ptr<StatusIcon> _cloud_status;
    std::shared_ptr<StatusIcon> _udisk_status;
    std::shared_ptr<StatusIcon> _reboot_status;

    std::shared_ptr<ConsumeText> _txt1;
    std::shared_ptr<ConsumeText> _txt2;

    std::shared_ptr<ListView<NoticeData, lDelegate::NoticeDelegate>> _listview;

    std::shared_ptr<RebootConfirmDialog> _reboot_confirm_dialog;
};

} // namespace lvglpp::ui::page

#endif // MESSAGE_CENTER_H
