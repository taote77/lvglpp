#include "GameStateDialog.h"
#include "config/color_sheet.h"
#include "core/tools/Utils.h"

namespace lvglpp {

using namespace tools;
using namespace form::design;

GameStateDialog::GameStateDialog() : widgets::Dialog()
{
    this->getRoot()->setOpacity(1);

    _bg_item = std::make_shared<widgets::Widget>(getRoot());
    _bg_item->setSize(800, 480);
    // _bg_item->setBgColor(CLR_PRIMARY_BRIGHT);
    _bg_item->setOpacity(1);

    _score_lbl = std::make_shared<lvglpp::widgets::Label>("Game Over!!", CLR_SUCCESS_CONTAINER, _bg_item.get());
    _score_lbl->setOpacity(1);

    _score_lbl->setAlignment(LV_ALIGN_CENTER, -100, 0);

    _btn = std::make_shared<widgets::PushButton>(200, 40, widgets::PushButton::ColorStyle::Gray, "Start", _bg_item.get());

    _btn->setAlignmentTo(*_score_lbl, LV_ALIGN_LEFT_MID, 100, 0);
}

// GameStateDialog::~GameStateDialog()
// {
//     //
// }

void GameStateDialog::open()
{
    Dialog::open();
}

}; // namespace lvglpp