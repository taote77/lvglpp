#ifndef SMART_HOME_ROOM_CARD_H
#define SMART_HOME_ROOM_CARD_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include <functional>
#include <memory>
#include <string>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::widgets {

/// A card showing room summary: name, temperature, humidity, device count
class RoomCard : public w::BaseItem
{
public:
    RoomCard(const std::string &name, const std::string &icon, int deviceCount,
             float temperature, float humidity, w::BaseItem *parent = nullptr);

    void setTemperature(float temp);
    void setHumidity(float humidity);
    void setDeviceCount(int count);
    void setOnTap(std::function<void()> cb) { _tapCb = std::move(cb); }

private:
    std::string _name;
    std::function<void()> _tapCb;

    std::unique_ptr<w::LvText> _nameLabel;
    std::unique_ptr<w::LvText> _tempLabel;
    std::unique_ptr<w::LvText> _humidLabel;
    std::unique_ptr<w::LvText> _deviceLabel;
};

} // namespace lvglpp::ui::widgets

#endif
