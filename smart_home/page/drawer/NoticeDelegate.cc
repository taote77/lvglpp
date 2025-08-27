#include "NoticeDelegate.h"
#include "theme/form_design.h"

namespace lvglpp::ui::lDelegate {
using namespace lvglpp;
using namespace form;
using namespace form::design;

NoticeDelegate::NoticeDelegate(int index, BaseItem *parent) : ListViewDelegate(index, parent)
{}

void NoticeDelegate::draw(const NoticeData &data)
{
    constexpr short msg_width  = 554;
    constexpr short msg_height = 94;
    setSize(msg_width, msg_height);

    _bg_item = std::make_shared<BaseItem>(this);
    _bg_item->setSize(msg_width - 10, msg_height);
    _bg_item->setPos(0, 0);
    _bg_item->setRadius(8);

    if (data.type == NoticeData::Type_Info)
    {
        _bg_item->setBgColor(design::ThemeColor(design::CLR_SURFACE_CONTAINER));
    } else if (data.type == NoticeData::Type_Warning)
    {
        _bg_item->setBgColor(design::ThemeColor(design::CLR_WARNING_CONTAINER));
    } else if (data.type == NoticeData::Type_Error)
    {
        // red color
        _bg_item->setBgColor(design::ThemeColor(CLR_ERROR_CONTAINER));
    }
    _bg_item->setOpacity(1);

    _head_date_label = std::make_shared<ConsumeText>("2022-04-25 10:31", CLR_ON_SURFACE, _bg_item.get(), ConsumeText::FontSize20);

    _head_date_label->setPos(20, 10);

    _msg_label = std::make_shared<ConsumeText>(data.message, CLR_ON_SURFACE, _bg_item.get(), ConsumeText::FontSize24);
    _msg_label->setBgColor(design::ThemeColor(CLR_ON_SECONDARY));
    _msg_label->setPos(20, 30);
}

} // namespace lvglpp::ui::lDelegate
