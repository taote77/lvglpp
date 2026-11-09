#include "ThermostatWidget.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <format>

namespace lvglpp::ui::widgets {

using namespace form::design;

ThermostatWidget::ThermostatWidget(float currentTemp, float targetTemp,
                                   w::Widget *parent)
    : w::Widget(parent), _currentTemp(currentTemp), _targetTemp(targetTemp)
{
    setSize(300, 200);
    setBgColor(CLR_SURFACE_CONTAINER);
    setRadius(RADIUS_12);

    // Arc dial showing current temperature (range 10-35)
    _arc = std::make_unique<w::Arc>(this);
    _arc->setSize(120, 120);
    _arc->setRange(100, 350); // 10.0 to 35.0 in tenths
    _arc->setValue(static_cast<int32_t>(_currentTemp * 10));
    _arc->setAlignment(LV_ALIGN_LEFT_MID, 10, 0);

    // Current temperature (large)
    _currentLabel = std::make_unique<w::Label>(
        lvTrFmt("common_temperature_c", _currentTemp), CLR_ON_SURFACE, this, w::Label::FontSize36Bold);
    _currentLabel->setAlignment(LV_ALIGN_CENTER, 30, -30);

    // Target temperature
    _targetLabel = std::make_unique<w::Label>(
        lvTrFmt("thermostat_target", _targetTemp), CLR_ON_SURFACE_VARIANT_1, this, w::Label::FontSize22);
    _targetLabel->setAlignment(LV_ALIGN_CENTER, 30, 10);

    // Minus button
    _minusBtn = std::make_unique<w::PushButton>(40, 40, w::PushButton::Gray, "-", this);
    _minusBtn->setAlignment(LV_ALIGN_RIGHT_MID, -100, -20);
    _minusBtn->setOnClickedListener([this]() { _adjustTemp(-0.5f); });

    // Plus button
    _plusBtn = std::make_unique<w::PushButton>(40, 40, w::PushButton::Blue, "+", this);
    _plusBtn->setAlignment(LV_ALIGN_RIGHT_MID, -50, -20);
    _plusBtn->setOnClickedListener([this]() { _adjustTemp(+0.5f); });

    // Mode button
    _modeBtn = std::make_unique<w::PushButton>(80, 40, w::PushButton::Dark, lvTr("common_heat"), this);
    _modeBtn->setAlignment(LV_ALIGN_RIGHT_MID, -50, 25);
    _modeBtn->setOnClickedListener([this]() {
        _mode = static_cast<Mode>((static_cast<int>(_mode) + 1) % 4);
        _updateDisplay();
    });
}

void ThermostatWidget::setCurrentTemp(float temp) {
    _currentTemp = temp;
    _updateDisplay();
}

void ThermostatWidget::setTargetTemp(float temp) {
    _targetTemp = temp;
    _updateDisplay();
}

void ThermostatWidget::setMode(Mode mode) {
    _mode = mode;
    _updateDisplay();
}

void ThermostatWidget::_adjustTemp(float delta) {
    _targetTemp += delta;
    if (_targetTemp < 10.0f) _targetTemp = 10.0f;
    if (_targetTemp > 35.0f) _targetTemp = 35.0f;
    _updateDisplay();
    if (_tempCb) _tempCb(_targetTemp);
}

void ThermostatWidget::_updateDisplay() {
    _arc->setValue(static_cast<int32_t>(_currentTemp * 10));
    _currentLabel->setText(lvTrFmt("common_temperature_c", _currentTemp));
    _targetLabel->setText(lvTrFmt("thermostat_target", _targetTemp));

    const char *modeNames[] = {lvTr("common_off"), lvTr("common_heat"), lvTr("common_cool"), lvTr("common_auto")};
    _modeBtn->setText(modeNames[static_cast<int>(_mode)]);
}

} // namespace lvglpp::ui::widgets
