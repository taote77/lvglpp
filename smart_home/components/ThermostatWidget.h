#ifndef SMART_HOME_THERMOSTAT_WIDGET_H
#define SMART_HOME_THERMOSTAT_WIDGET_H

#include "core/widgets/Arc.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include <functional>
#include <memory>
#include <string>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::widgets {

/// Thermostat control with Arc dial, +/- buttons, mode selector
class ThermostatWidget : public w::BaseItem
{
public:
    enum Mode { Off, Heat, Cool, Auto };

    ThermostatWidget(float currentTemp = 22.0f, float targetTemp = 21.0f,
                     w::BaseItem *parent = nullptr);

    void setCurrentTemp(float temp);
    void setTargetTemp(float temp);
    void setMode(Mode mode);

    float targetTemp() const { return _targetTemp; }
    Mode mode() const { return _mode; }

    void setOnTempChanged(std::function<void(float)> cb) { _tempCb = std::move(cb); }

private:
    void _updateDisplay();
    void _adjustTemp(float delta);

    float _currentTemp = 22.0f;
    float _targetTemp = 21.0f;
    Mode  _mode = Heat;

    std::function<void(float)> _tempCb;

    std::unique_ptr<w::Arc>        _arc;
    std::unique_ptr<w::LvText>     _currentLabel;
    std::unique_ptr<w::LvText>     _targetLabel;
    std::unique_ptr<w::PushButton> _minusBtn;
    std::unique_ptr<w::PushButton> _plusBtn;
    std::unique_ptr<w::PushButton> _modeBtn;
};

} // namespace lvglpp::ui::widgets

#endif
