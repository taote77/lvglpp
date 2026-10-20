#include "MsgBox.h"
#include "core/base/LvObjFactory.h"

#include <utility>

namespace lvglpp::widgets {

MsgBox::MsgBox(BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglMsgBox, parent)
{}

MsgBox &MsgBox::setTitle(const std::string &title)
{
    if (getLvglItem() != nullptr)
    {
        lv_msgbox_add_title(getLvglItem(), title.c_str());
    }
    return *this;
}

MsgBox &MsgBox::addText(const std::string &text)
{
    if (getLvglItem() != nullptr)
    {
        lv_msgbox_add_text(getLvglItem(), text.c_str());
    }
    return *this;
}

BaseItem *MsgBox::addButton(const std::string &text, std::function<void()> onClick, bool closeOnClick)
{
    if (getLvglItem() == nullptr)
    {
        return nullptr;
    }

    lv_obj_t *button = lv_msgbox_add_footer_button(getLvglItem(), text.c_str());
    if (button == nullptr)
    {
        return nullptr;
    }

    auto wrapper = std::make_unique<BaseItem>(BaseItem::AdoptExistingTag::Adopt, button, this, false);
    wrapper->setFocusable(true);
    wrapper->onClicked([this, callback = std::move(onClick), closeOnClick]() {
        if (callback)
        {
            callback();
        }
        if (closeOnClick)
        {
            close();
        }
    });

    BaseItem *raw = wrapper.get();
    button_wrappers_.push_back(std::move(wrapper));
    return raw;
}

void MsgBox::close()
{
    if (getLvglItem() != nullptr)
    {
        lv_msgbox_close(getLvglItem());
    }
}

} // namespace lvglpp::widgets
