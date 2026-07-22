#include "StatusBadge.h"
#include "core/widgets/LvText.h"
#include "theme/form_design.h"

namespace lvglpp::ui::widgets {

using namespace form::design;
namespace w = ::lvglpp::widgets;

static uint32_t _colorForState(StatusBadge::State s) {
    switch (s) {
        case StatusBadge::Online:  return CLR_SUCCESS;
        case StatusBadge::Offline: return CLR_PRIMARY_DIS_BG;
        case StatusBadge::Warning: return CLR_WARNING;
        case StatusBadge::Error:   return CLR_ERROR;
    }
    return CLR_SUCCESS;
}

StatusBadge::StatusBadge(State state, w::BaseItem *parent)
    : w::BaseItem(parent), _state(state)
{
    setSize(80, 24);

    _dot = std::make_unique<w::BaseItem>(this);
    _dot->setSize(10, 10);
    _dot->setRadius(5);
    _dot->setAligment(LV_ALIGN_LEFT_MID, 0, 0);

    _label = std::make_unique<w::LvText>("", CLR_ON_SURFACE_VARIANT_1, this, w::LvText::FontSize20);
    _label->setAligment(LV_ALIGN_LEFT_MID, 16, 0);

    _updateAppearance();
}

void StatusBadge::setState(State state) {
    _state = state;
    _updateAppearance();
}

void StatusBadge::setLabel(const std::string &text) {
    if (auto *t = dynamic_cast<w::LvText *>(_label.get())) {
        t->setText(text);
    }
}

void StatusBadge::_updateAppearance() {
    _dot->setBgColor(_colorForState(_state));
}

} // namespace lvglpp::ui::widgets
