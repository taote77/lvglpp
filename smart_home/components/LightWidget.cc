#include "LightWidget.h"
#include "i18n/LvTranslator.h"
#include "theme/form_design.h"
#include <format>

namespace lvglpp::ui::widgets {

using namespace form::design;

LightWidget::LightWidget(const std::string &lightName, w::BaseItem *parent)
    : w::BaseItem(parent)
{
    setSize(360, 160);
    setBgColor(CLR_SURFACE_CONTAINER);
    setRadius(RADIUS_12);

    // Name
    _nameLabel = std::make_unique<w::LvText>(lightName, CLR_ON_SURFACE, this, w::LvText::FontSize24Bold);
    _nameLabel->setAligment(LV_ALIGN_TOP_LEFT, 12, 12);

    // Power toggle
    _powerBtn = std::make_unique<w::PushButton>(60, 36, w::PushButton::Dark, lvTr("Common.Off"), this);
    _powerBtn->setAligment(LV_ALIGN_TOP_RIGHT, -12, 12);
    _powerBtn->setOnClickedListener([this]() {
        _on = !_on;
        _updateUI();
    });

    // Brightness slider
    _brightnessSlider = std::make_unique<w::Slider>(this);
    _brightnessSlider->setSize(320, 20);
    _brightnessSlider->setRange(0, 100);
    _brightnessSlider->setValue(_brightness);
    _brightnessSlider->setAligment(LV_ALIGN_CENTER, 0, -10);
    _brightnessSlider->setOnClickedListener([this]() {
        _brightness = _brightnessSlider->value();
        _valueLabel->setText(lvTrFmt("Common.Percent", _brightness));
        if (_brightnessCb) _brightnessCb(_brightness);
    });

    // Value label
    _valueLabel = std::make_unique<w::LvText>(lvTrFmt("Common.Percent", 100), CLR_ON_SURFACE_VARIANT_1, this, w::LvText::FontSize28Bold);
    _valueLabel->setAligment(LV_ALIGN_CENTER, 0, 25);

    // Color presets
    _warmBtn = std::make_unique<w::PushButton>(70, 30, w::PushButton::Gray, lvTr("Common.Warm"), this);
    _warmBtn->setAligment(LV_ALIGN_BOTTOM_LEFT, 12, -12);

    _coolBtn = std::make_unique<w::PushButton>(70, 30, w::PushButton::Gray, lvTr("Common.Cool"), this);
    _coolBtn->setAligment(LV_ALIGN_BOTTOM_LEFT, 90, -12);

    _dayBtn = std::make_unique<w::PushButton>(70, 30, w::PushButton::Gray, lvTr("Common.Day"), this);
    _dayBtn->setAligment(LV_ALIGN_BOTTOM_LEFT, 168, -12);
}

void LightWidget::setBrightness(int percent) {
    _brightness = percent;
    _updateUI();
}

void LightWidget::setOn(bool on) {
    _on = on;
    _updateUI();
}

void LightWidget::_updateUI() {
    _powerBtn->setTextMsg(_on ? lvTr("Common.On") : lvTr("Common.Off"));
    _powerBtn->setColorType(_on ? w::PushButton::Blue : w::PushButton::Dark);
    _brightnessSlider->setValue(_on ? _brightness : 0);
    _valueLabel->setText(lvTrFmt("Common.Percent", _on ? _brightness : 0));
}

} // namespace lvglpp::ui::widgets
