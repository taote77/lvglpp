#include "Home.h"
#include "GameActivity.h"
#include "core/sys/Navigators.h"
#include "core/widgets/GlobalVar.h"
#include <iostream>
#include <src/others/observer/lv_observer.h>

Home::Home() : sys::BaseActivity()
{}

Home::~Home()
{}

void Home::onCreate(void *arg)
{
    _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Start", getRootItem());

    _btn->setPos(480, 300);
    _btn->setAligment(LV_ALIGN_CENTER, 0, 0);

    _btn->setOnClickedListener([this]() {
        //
        lvglpp::sys::Navigators::getInstance()->pushView(std::make_shared<GameActivity>());
    });
}

void Home::onNotifyUI(const sys::Event &evt)
{}

void Home::onDestroy()
{}
