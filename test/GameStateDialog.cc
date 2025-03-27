#include "GameStateDialog.h"

#include "core/tools/Utils.h"
#include "core/widgets/GlobalVar.h"

namespace lvglpp {

using namespace tools;

GameStateDialog::GameStateDialog() : widgets::BaseDialog()
{
    this->getRootItem()->setOpacity(1);

    _bg_item = std::make_shared<widgets::BaseItem>(getRootItem());
    _bg_item->setSize(800, 480);
    _bg_item->setBgColor(CLR_PRIMARY_BRIGHT);
    _bg_item->setOpacity(1);

    _score_lbl = std::make_shared<lvglpp::widgets::LvText>("Game Over!!", CLR_SUCCESS_CONTAINER, _bg_item.get());
    _score_lbl->setOpacity(1);

    _score_lbl->setAligment(LV_ALIGN_CENTER, -100, 0);

    _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Start", _bg_item.get());

    _btn->setAligmentTo(*_score_lbl, LV_ALIGN_LEFT_MID, 100, 0);
}

// GameStateDialog::~GameStateDialog()
// {
//     //
// }

void GameStateDialog::open()
{
    BaseDialog::open();
}

}; // namespace lvglpp