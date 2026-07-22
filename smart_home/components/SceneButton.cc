#include "SceneButton.h"
#include "theme/form_design.h"

namespace lvglpp::ui::widgets {

using namespace form::design;

SceneButton::SceneButton(const std::string &icon, const std::string &name,
                         const std::string &description, w::BaseItem *parent)
    : w::BaseItem(parent)
{
    setSize(170, 120);
    setBgColor(CLR_SURFACE_CONTAINER);
    setRadius(RADIUS_12);

    // Icon
    _iconLabel = std::make_unique<w::LvText>(icon, CLR_ON_SURFACE, this, w::LvText::FontSize36);
    _iconLabel->setAligment(LV_ALIGN_TOP_MID, 0, 8);

    // Name
    _nameLabel = std::make_unique<w::LvText>(name, CLR_ON_SURFACE_VARIANT_0, this, w::LvText::FontSize22Bold);
    _nameLabel->setAligment(LV_ALIGN_CENTER, 0, 5);

    // Description
    _descLabel = std::make_unique<w::LvText>(description, CLR_ON_SURFACE_VARIANT_2, this, w::LvText::FontSize20);
    _descLabel->setAligment(LV_ALIGN_BOTTOM_MID, 0, -8);

    setOnClickedListener([this]() {
        _active = !_active;
        setBorder(_active ? 2 : 0, _active ? CLR_PRIMARY : CLR_SURFACE_CONTAINER);
        if (_active && _activateCb) _activateCb();
    });
}

void SceneButton::setActive(bool active) {
    _active = active;
    setBorder(active ? 2 : 0, active ? CLR_PRIMARY : CLR_SURFACE_CONTAINER);
}

} // namespace lvglpp::ui::widgets
