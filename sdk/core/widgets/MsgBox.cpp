#include "MsgBox.h"
#include "core/base/LvObjFactory.h"

#include <utility>

namespace lvglpp::widgets {

MsgBox::MsgBox(Widget *parent) : Widget(tools::LvObjFactory::createLvglMsgBox, parent)
{}

MsgBox &MsgBox::setTitle(const std::string &title)
{
    if (lvObj() != nullptr)
    {
        lv_msgbox_add_title(lvObj(), title.c_str());
    }
    return *this;
}

MsgBox &MsgBox::addText(const std::string &text)
{
    if (lvObj() != nullptr)
    {
        lv_msgbox_add_text(lvObj(), text.c_str());
    }
    return *this;
}

Widget *MsgBox::addButton(const std::string &text, std::function<void()> onClick, bool closeOnClick)
{
    if (lvObj() == nullptr)
    {
        return nullptr;
    }

    lv_obj_t *button = lv_msgbox_add_footer_button(lvObj(), text.c_str());
    if (button == nullptr)
    {
        return nullptr;
    }

    auto wrapper = std::make_unique<Widget>(Widget::AdoptExistingTag::Adopt, button, false);
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

    Widget *raw = wrapper.get();
    button_wrappers_.push_back(std::move(wrapper));
    return raw;
}

void MsgBox::close()
{
    if (lvObj() != nullptr)
    {
        lv_msgbox_close(lvObj());
    }
}

} // namespace lvglpp::widgets
