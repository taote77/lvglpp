#include "GroupCheckButton.h"
#include "theme/theme_keeper.h"

#include <utility>

namespace wrapper::widgets
{
    using namespace form::design;


    SelectedItem::SelectedItem(const std::string &display, BaseItem *parent) : BaseItem(parent),
                                                                               state_(State::UnCheck)
    {
        icon_ = std::make_shared<widgets::Image>("", this);
        icon_->setAligment(LV_ALIGN_LEFT_MID, 0, 0);
        display_label_ = std::make_shared<ConsumeText>(display, ThemeColor(CLR_ON_SURFACE), this, ConsumeText::FontSize24);
        display_label_->setAligment(LV_ALIGN_CENTER, 0, 0);
        updateState(State::UnCheck);

        this->setEnable(true);
    }

    void SelectedItem::updateState(SelectedItem::State sta)
    {
        state_ = sta;
        if (state_ == State::Check)
        {
            setBgColor(CLR_PRIMARY_CONTAINER);
        }
        else
        {
            setBgColor(CLR_SURFACE);
        }
    }

    GroupCheckButton::GroupCheckButton(const std::string &txt1, const std::string &txt2, BaseItem *parent) : BaseItem(parent)
    {
        constexpr int cont_width{186};
        constexpr int cont_height{56};
        this->setSize(cont_width, cont_height);

        this->setBgColor(CLR_SURFACE);
        this->setBorder(1, CLR_OUTLINE);
        this->setRadius(RADIUS_10);

        _item1 = std::make_shared<SelectedItem>(txt1, this);
        _item1->setSize(cont_width / 2 - 2, cont_height - 2);
        _item1->setAligment(LV_ALIGN_CENTER, -cont_width / 4, 0);
        _item1->setRadius(RADIUS_10);

        _item1->updateState(SelectedItem::State::Check);

        _item2 = std::make_shared<SelectedItem>(txt2, this);
        _item2->setSize(cont_width / 2 - 2, cont_height - 2);
        _item2->setAligment(LV_ALIGN_CENTER, cont_width / 4, 0);
        _item2->setRadius(RADIUS_10);
    }


    void GroupCheckButton::SelectItem1(bool selected)
    {
        _item1_selected = selected;
        _item1->updateState(selected ? SelectedItem::State::Check : SelectedItem::State::UnCheck);
        _item2->updateState(selected ? SelectedItem::State::UnCheck : SelectedItem::State::Check);
    }

    bool GroupCheckButton::Item1Selected() const
    {
        return _item1_selected;
    }

    void GroupCheckButton::setItemClickOnListener(std::function<void()> click)
    {
        _item1_click = std::move(click);
        if (_item1)
        {
            _item1->setOnClickedListener([this]() {
                _item1_selected = true;
                _item1_click();
                _item1->updateState(SelectedItem::State::Check);
                _item2->updateState(SelectedItem::State::UnCheck);
            });
        }
    }

    void GroupCheckButton::setItem2ClickOnListener(std::function<void()> click)
    {
        _item2_click = std::move(click);

        if (_item2)
        {
            _item2->setOnClickedListener([this]() {
                _item1_selected = false;
                _item2_click();
                _item2->updateState(SelectedItem::State::Check);
                _item1->updateState(SelectedItem::State::UnCheck);
            });
        }
    }


}// namespace wrapper::widgets
