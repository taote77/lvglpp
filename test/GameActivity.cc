#include "GameActivity.h"

#include "core/widgets/GlobalVar.h"

GameActivity::GameActivity() : sys::BaseActivity() { }

GameActivity::~GameActivity() { }

void GameActivity::onCreate(void *arg)
{

    _label = std::make_shared<widgets::LvText>("Network Preparing...", CLR_PRIMARY_BLACK,
                                               getRootWindow(), widgets::LvText::FontSize32);

    _label->setTextAlign(LV_TEXT_ALIGN_CENTER);
    _label->setAligment(LV_ALIGN_TOP_MID, 0, 424);

    // _btn
}

void GameActivity::onNotifyUI(const sys::Event &evt) { }

void GameActivity::onDestroy() { }
