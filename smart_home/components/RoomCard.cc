#include "RoomCard.h"
#include "theme/form_design.h"
#include <format>

namespace lvglpp::ui::widgets {

using namespace form::design;

RoomCard::RoomCard(const std::string &name, const std::string &icon, int deviceCount,
                   float temperature, float humidity, w::BaseItem *parent)
    : w::BaseItem(parent), _name(name)
{
    setSize(180, 130);
    setBgColor(CLR_SURFACE_CONTAINER);
    setRadius(RADIUS_12);

    // Room icon/emoji
    auto iconLabel = std::make_unique<w::LvText>(icon, CLR_ON_SURFACE, this, w::LvText::FontSize32);
    iconLabel->setAligment(LV_ALIGN_TOP_LEFT, 12, 10);

    // Room name
    _nameLabel = std::make_unique<w::LvText>(name, CLR_ON_SURFACE_VARIANT_0, this, w::LvText::FontSize22Bold);
    _nameLabel->setAligment(LV_ALIGN_TOP_LEFT, 12, 50);

    // Temperature
    auto tempStr = std::format("{:.1f}C", temperature);
    _tempLabel = std::make_unique<w::LvText>(tempStr, CLR_PRIMARY, this, w::LvText::FontSize22Bold);
    _tempLabel->setAligment(LV_ALIGN_BOTTOM_LEFT, 12, -30);

    // Humidity
    auto humidStr = std::format("{:.0f}%", humidity);
    _humidLabel = std::make_unique<w::LvText>(humidStr, CLR_INFO, this, w::LvText::FontSize20);
    _humidLabel->setAligment(LV_ALIGN_BOTTOM_LEFT, 12, -10);

    // Device count
    auto devStr = std::format("{} devices", deviceCount);
    _deviceLabel = std::make_unique<w::LvText>(devStr, CLR_ON_SURFACE_VARIANT_2, this, w::LvText::FontSize20);
    _deviceLabel->setAligment(LV_ALIGN_BOTTOM_RIGHT, -10, -30);

    setOnClickedListener([this]() { if (_tapCb) _tapCb(); });
}

void RoomCard::setTemperature(float temp) {
    _tempLabel->setText(std::format("{:.1f}C", temp));
}

void RoomCard::setHumidity(float humidity) {
    _humidLabel->setText(std::format("{:.0f}%", humidity));
}

void RoomCard::setDeviceCount(int count) {
    _deviceLabel->setText(std::format("{} devices", count));
}

} // namespace lvglpp::ui::widgets
