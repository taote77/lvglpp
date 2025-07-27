#include "HomePage.h"
#include "core/sys/Activity.h"
#include "core/sys/Navigators.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/Toast.h"
#include <iostream>

HomePage::HomePage() : sys::BaseActivity()
{}

HomePage::~HomePage()
{}

void HomePage::onCreate(void *arg)
{
    _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Light On", getRootItem());

    _btn->setPos(480, 300);
    _btn->setAligment(LV_ALIGN_CENTER, 0, 0);

    _btn->setOnClickedListener([this]() {
        bool light_status = (_txt == "Light On");
        _txt              = light_status ? "Light Off" : "Light On";
        _btn->setTextMsg(_txt);

        // Simulate a light toggle action
        widgets::Toast::success(light_status ? "点击，灯光已打开" : "点击，灯光已关闭");
    });

    _drawer = std::make_shared<lvglpp::ui::page::Drawer>(lvglpp::ui::page::Drawer::Edge::TopEdge, this->getRootItem());
}

void HomePage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
    // sys::Activity::onNotifyUI(evt);
}

void HomePage::onDestroy()
{}
