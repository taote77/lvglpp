#include "Home.h"
#include "core/sys/Navigators.h"
#include "core/widgets/GlobalVar.h"
#include "GameActivity.h"

Home::Home() : sys::BaseActivity() { }

Home::~Home() { }

void Home::onCreate(void *arg)
{

    _label = std::make_shared<widgets::LvText>("Network Preparing...", CLR_PRIMARY_BLACK,
                                               getRootWindow(), widgets::LvText::FontSize32);

    _label->setTextAlign(LV_TEXT_ALIGN_CENTER);
    _label->setAligment(LV_ALIGN_TOP_MID, 0, 424);

    _btn = std::make_shared<widgets::LvButton>("Start", CLR_PRIMARY_BLACK, getRootWindow(),
                                               widgets::LvText::FontSize32);
    // _btn->setText("Start Game");
    _btn->setPos(480, 300);
    // _btn->setTextAlign(LV_TEXT_ALIGN_CENTER);
    // _btn->setAligment(LV_ALIGN_CENTER, 0, 0);
    // _btn->setEna
    _btn->setOnClickedListener([this]() {
        lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<GameActivity>());
    });
}

void Home::onNotifyUI(const sys::Event &evt) { }

void Home::onDestroy() { }
