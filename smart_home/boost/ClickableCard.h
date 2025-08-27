#ifndef CLICKABLE_CARD_H
#define CLICKABLE_CARD_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/Image.h"
#include <memory>

namespace wrapper::widgets {
using namespace lvglpp::widgets;

class ClickableCard : public BaseItem
{
public:
    explicit ClickableCard(const std::string &text, const std::string &icon_src, BaseItem *parent = nullptr);

    ClickableCard(const std::string &text, const std::string &note, const std::string &icon_src, BaseItem *parent = nullptr);

    void setSelected(bool selected);

    void setSelected(bool selected, const std::string &note);

    void setBorderWidth(uint32_t border_width)
    {
        _border_width = border_width;
    }

    void Resize(int width, int height);

    void setCardTxtMaxWidth(int weight);

    void SetSelectedBgColor(uint32_t main_color, uint32_t border_color);

    void SetDefaultBgColor(uint32_t main_color, uint32_t border_color);

private:
    uint32_t _selected_bg_color;
    uint32_t _selected_border_color;
    uint32_t _default_bg_color;
    uint32_t _default_border_color;

    uint32_t _border_width{2};

    std::shared_ptr<ConsumeText>    _label;
    std::shared_ptr<ConsumeText>    _foot_label;
    std::shared_ptr<widgets::Image> _icon_;
};
} // namespace wrapper::widgets

#endif // CLICKABLE_CARD_H