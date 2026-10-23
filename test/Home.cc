#include "Home.h"
#include "GameActivity.h"
#include "config/color_sheet.h"
#include "core/sys/StackView.h"
#include <iostream>
#include <src/others/observer/lv_observer.h>

Home::Home() : sys::BaseActivity()
{}

Home::~Home()
{}

void Home::onCreate(std::any arg)
{
    _btn = std::make_shared<widgets::PushButton>(200, 40, widgets::PushButton::ColorStyle::Gray, "Start", getRoot());

    _btn->setPos(480, 300);
    _btn->setAlignment(LV_ALIGN_CENTER, 0, 0);

    _btn->setOnClickedListener([this]() {
        //
        lvglpp::sys::StackView::getInstance().pushView(std::make_shared<GameActivity>());
    });
}

void Home::onNotifyUI(const sys::Event &evt)
{}

void Home::onDestroy()
{}
