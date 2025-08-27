#include "ClickableCard.h"
#include <theme/form_design.h>

namespace wrapper::widgets
{

    using namespace form::design;

    ClickableCard::ClickableCard(const std::string &text, const std::string &icon_src, BaseItem *parent)
        : BaseItem(parent),
          _selected_bg_color(CLR_SURFACE_CONTAINER),
          _selected_border_color(CLR_PRIMARY),
          _default_bg_color(CLR_SURFACE_CONTAINER),
          _default_border_color(CLR_SURFACE_CONTAINER)
    {
        constexpr short langContentWidth{356};
        constexpr short langContentHeight{200};

        setSize(langContentWidth, langContentHeight);
        setRadius(12);
        setEnable(true);

        if (!icon_src.empty())
        {
            _icon_ = std::make_shared<widgets::Image>(icon_src, this);
            _icon_->setAligment(LV_ALIGN_CENTER, 0, 0);
        }


        _label = std::make_shared<ConsumeText>(text, ThemeColor(CLR_ON_SURFACE), this, ConsumeText::FontSize24Bold);

        _label->setAligment(LV_ALIGN_CENTER, 0, 0);
    }

    ClickableCard::ClickableCard(const std::string &text, const std::string &note, const std::string &icon_src, BaseItem *parent)
        : ClickableCard(text, icon_src, parent)
    {

        _label->setAligment(LV_ALIGN_TOP_MID, 0, 60);

        _foot_label = std::make_shared<ConsumeText>(note, ThemeColor(CLR_ON_SURFACE), this, ConsumeText::FontSize24);
        _foot_label->setAligment(LV_ALIGN_TOP_MID, 0, 60 + 40);
    }


    void ClickableCard::setSelected(bool selected)
    {
        if (selected)
        {
            setBgColor(_selected_bg_color);
            setBorder(_border_width, _selected_border_color);
        }
        else
        {
            setBgColor(_default_bg_color);

            setBorder(_border_width, _default_border_color);
        }
    }

    void ClickableCard::setSelected(bool selected, const std::string &note)
    {
        setSelected(selected);

        if (_foot_label)
        {
            _foot_label->setText(note);
        }
    }


    void ClickableCard::Resize(int width, int height)
    {
        setSize(width, height);
    }

    void ClickableCard::setCardTxtMaxWidth(int weight)
    {
        if (_label)
        {
            _label->setWidth(weight);
            _label->setTextAlign(LV_TEXT_ALIGN_CENTER);
        }
    }

    void ClickableCard::SetSelectedBgColor(uint32_t main_color, uint32_t border_color)
    {
        _selected_bg_color = main_color;
        _selected_border_color = border_color;
    }

    void ClickableCard::SetDefaultBgColor(uint32_t main_color, uint32_t border_color)
    {
        _default_bg_color = main_color;
        _default_border_color = border_color;
    }


}// namespace wrapper::widgets