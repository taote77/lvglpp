#include "DeviceControlPage.h"
#include "components/DeviceCard.h"
#include "components/LightWidget.h"
#include "components/ThermostatWidget.h"
#include "core/log/log.h"
#include "core/widgets/Toast.h"
#include "theme/form_design.h"

namespace lvglpp::ui::page {

using namespace form::design;

DeviceControlPage::DeviceControlPage() = default;
DeviceControlPage::~DeviceControlPage() = default;

void DeviceControlPage::onCreate(std::any arg) {
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    _title = std::make_unique<w::Label>("Devices", CLR_ON_SURFACE, getRoot(), w::Label::FontSize28Bold);
    _title->setAlignment(LV_ALIGN_TOP_LEFT, 20, 10);

    int y = 50;
    buildLightsSection(y);
    buildThermostatsSection(y);
    buildLocksSection(y);

    LOG_INFO() << "DeviceControlPage created";
}

void DeviceControlPage::buildLightsSection(int &y) {
    auto sectionLabel = std::make_unique<w::Label>("Lights", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::Label::FontSize22Bold);
    sectionLabel->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    y += 28;

    // Ceiling Light with brightness control
    auto ceilingLight = std::make_unique<ui::widgets::LightWidget>("Ceiling Light", getRoot());
    ceilingLight->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    y += 170;

    // Floor Lamp (simple toggle)
    auto floorLamp = std::make_unique<ui::widgets::DeviceCard>("Floor Lamp", "L", true, getRoot());
    floorLamp->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    floorLamp->setSubtitle("Living Room");
    floorLamp->setOnToggleCallback([](bool on) { w::Toast::success(on ? "Floor lamp on" : "Floor lamp off"); });
    y += 72;
}

void DeviceControlPage::buildThermostatsSection(int &y) {
    auto sectionLabel = std::make_unique<w::Label>("Thermostats", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::Label::FontSize22Bold);
    sectionLabel->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    y += 28;

    auto thermostat = std::make_unique<ui::widgets::ThermostatWidget>(22.0f, 21.0f, getRoot());
    thermostat->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    y += 210;
}

void DeviceControlPage::buildLocksSection(int &y) {
    auto sectionLabel = std::make_unique<w::Label>("Security", CLR_ON_SURFACE_VARIANT_1, getRoot(), w::Label::FontSize22Bold);
    sectionLabel->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    y += 28;

    auto frontDoor = std::make_unique<ui::widgets::DeviceCard>("Front Door", "D", true, getRoot());
    frontDoor->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    frontDoor->setSubtitle("Locked");
    y += 72;

    auto garageDoor = std::make_unique<ui::widgets::DeviceCard>("Garage Door", "G", false, getRoot());
    garageDoor->setAlignment(LV_ALIGN_TOP_LEFT, 20, y);
    garageDoor->setSubtitle("Open");
}

void DeviceControlPage::onNotifyUI(const sys::Event &) {}
void DeviceControlPage::onDestroy() {}

} // namespace lvglpp::ui::page
