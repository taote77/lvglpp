#include "DeviceCard.h"
#include "theme/form_design.h"

namespace lvglpp::ui::widgets {

using namespace form::design;

DeviceCard::DeviceCard(const std::string &name, const std::string &iconText,
                       bool initialState, w::BaseItem *parent)
    : w::BaseItem(parent), _on(initialState)
{
    setSize(360, 64);
    setBgColor(CLR_SURFACE_CONTAINER);
    setRadius(RADIUS_12);

    _iconArea = std::make_unique<w::BaseItem>(this);
    _iconArea->setSize(40, 40);
    _iconArea->setRadius(20);
    _iconArea->setBgColor(CLR_PRIMARY);
    _iconArea->setAligment(LV_ALIGN_LEFT_MID, 12, 0);

    _iconLabel = std::make_unique<w::LvText>(iconText, CLR_ON_PRIMARY, _iconArea.get(), w::LvText::FontSize20Bold);
    _iconLabel->setAligment(LV_ALIGN_CENTER, 0, 0);

    _nameLabel = std::make_unique<w::LvText>(name, CLR_ON_SURFACE, this, w::LvText::FontSize22Bold);
    _nameLabel->setAligment(LV_ALIGN_TOP_LEFT, 64, 10);

    _subtitleLabel = std::make_unique<w::LvText>("", CLR_ON_SURFACE_VARIANT_2, this, w::LvText::FontSize20);
    _subtitleLabel->setAligment(LV_ALIGN_TOP_LEFT, 64, 36);

    _switch = std::make_unique<w::SwitchButton>(48, 24, this);
    _switch->setAligment(LV_ALIGN_RIGHT_MID, -12, 0);
    _switch->setCheck(initialState);

    setOnClickedListener([this]() { _onToggle(); });
}

void DeviceCard::setOn(bool on) {
    _on = on;
    _switch->setCheck(on);
}

void DeviceCard::_onToggle() {
    _on = !_on;
    _switch->setCheck(_on);
    if (_toggleCb) _toggleCb(_on);
}

void DeviceCard::setName(const std::string &name) {
    _nameLabel->setText(name);
}

void DeviceCard::setSubtitle(const std::string &sub) {
    _subtitleLabel->setText(sub);
}

} // namespace lvglpp::ui::widgets
