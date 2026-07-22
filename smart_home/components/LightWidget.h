#ifndef SMART_HOME_LIGHT_WIDGET_H
#define SMART_HOME_LIGHT_WIDGET_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include "core/widgets/Slider.h"
#include <functional>
#include <memory>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::widgets {

/// Light control panel with brightness slider and color presets
class LightWidget : public w::BaseItem
{
public:
    LightWidget(const std::string &lightName, w::BaseItem *parent = nullptr);

    void setBrightness(int percent);
    int brightness() const { return _brightness; }

    void setOn(bool on);
    bool isOn() const { return _on; }

    void setOnBrightnessChanged(std::function<void(int)> cb) { _brightnessCb = std::move(cb); }

private:
    void _updateUI();

    bool _on = false;
    int  _brightness = 100;

    std::function<void(int)> _brightnessCb;

    std::unique_ptr<w::LvText>     _nameLabel;
    std::unique_ptr<w::LvText>     _valueLabel;
    std::unique_ptr<w::Slider>     _brightnessSlider;
    std::unique_ptr<w::PushButton> _powerBtn;
    std::unique_ptr<w::PushButton> _warmBtn;
    std::unique_ptr<w::PushButton> _coolBtn;
    std::unique_ptr<w::PushButton> _dayBtn;
};

} // namespace lvglpp::ui::widgets

#endif
