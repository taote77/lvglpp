#include "HomePage.h"
#include "core/sys/Activity.h"
#include "core/sys/TaskStack.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/Toast.h"
#include "page/sensor/SensorActivity.h"
#include <iostream>

using namespace lvglpp;

HomePage::HomePage() : sys::BaseActivity()
{}

HomePage::~HomePage()
{}

void HomePage::onCreate(void *arg)
{
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(form::design::CLR_SURFACE);

    _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Light On", getRoot());

    _btn->setPos(480, 300);
    _btn->setAligment(LV_ALIGN_CENTER, 0, 0);

    _btn->setOnClickedListener([this]() {
        bool light_status = (_txt == "Light On");
        _txt              = light_status ? "Light Off" : "Light On";
        _btn->setTextMsg(_txt);

        // Simulate a light toggle action
        widgets::Toast::success(light_status ? "点击，灯光已打开" : "点击，灯光已关闭");
    });

    _btn_air_temp = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Temp Curve", getRoot());
    _btn_air_temp->setPos(480, 400);
    _btn_air_temp->setAligment(LV_ALIGN_CENTER, 0, 0);

    _btn_air_temp->setOnClickedListener([this]() {
        std::cout << "Temp Curve\n";
        // openActivity<AppPage>("AppPage");

        auto page = std::make_shared<SensorActivity>();
        sys::TaskStack::getInstance()->pushView(page);
    });

    _drawer = std::make_shared<lvglpp::ui::page::TopDrawer>(this->getRoot());
}

void HomePage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
    // sys::Activity::onNotifyUI(evt);
}

void HomePage::onDestroy()
{}
