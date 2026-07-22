#ifndef GROUP_CHECK_BUTTON_H
#define GROUP_CHECK_BUTTON_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/Image.h"
#include <memory>

namespace wrapper::widgets {
using namespace lvglpp::widgets;

class SelectedItem : public BaseItem
{
public:
    enum State { UnCheck, Check };
    explicit SelectedItem(const std::string &display, BaseItem *parent = nullptr);

    void updateState(State sta);

    State getSlectedState() const
    {
        return state_;
    }

    /// Correct spelling
    State getSelectedState() const { return getSlectedState(); }

private:
    std::shared_ptr<ConsumeText>    display_label_;
    std::shared_ptr<widgets::Image> icon_;
    State                           state_;
};

class GroupCheckButton : public BaseItem
{
public:
    explicit GroupCheckButton(const std::string &txt1, const std::string &txt2, BaseItem *parent = nullptr);

    void SelectItem1(bool selected);

    bool Item1Selected() const;

    void setItemClickOnListener(std::function<void()> click);

    void setItem2ClickOnListener(std::function<void()> click);

private:
    bool _item1_selected{true};

    std::shared_ptr<SelectedItem> _item1;
    std::shared_ptr<SelectedItem> _item2;

    std::function<void()> _item1_click;
    std::function<void()> _item2_click;
};

} // namespace wrapper::widgets

#endif // RADIO_BUTTON_H
