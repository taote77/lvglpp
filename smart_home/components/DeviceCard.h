#ifndef SMART_HOME_DEVICE_CARD_H
#define SMART_HOME_DEVICE_CARD_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/SwitchButton.h"
#include <functional>
#include <memory>
#include <string>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::widgets {

/// A card showing a smart device with icon, name, status, and toggle
class DeviceCard : public w::BaseItem
{
public:
    DeviceCard(const std::string &name, const std::string &iconText,
               bool initialState, w::BaseItem *parent = nullptr);

    void setOn(bool on);
    bool isOn() const { return _on; }

    void setOnToggleCallback(std::function<void(bool)> cb) { _toggleCb = std::move(cb); }
    void setName(const std::string &name);
    void setSubtitle(const std::string &sub);

private:
    void _onToggle();

    bool _on = false;
    std::function<void(bool)> _toggleCb;

    std::unique_ptr<w::BaseItem>        _iconArea;
    std::unique_ptr<w::LvText>          _iconLabel;
    std::unique_ptr<w::LvText>          _nameLabel;
    std::unique_ptr<w::LvText>          _subtitleLabel;
    std::unique_ptr<w::SwitchButton>    _switch;
};

} // namespace lvglpp::ui::widgets

#endif
