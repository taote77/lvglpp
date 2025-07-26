#include "HomePage.h"
#include "core/sys/Activity.h"
#include "core/sys/Navigators.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/GlobalVar.h"
#include "core/widgets/Toast.h"
#include <iostream>
#include <src/others/observer/lv_observer.h>

HomePage::HomePage() : sys::BaseActivity()
{}

HomePage::~HomePage()
{}

void HomePage::onCreate(void *arg)
{
    _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Click", getRootItem());

    _btn->setPos(480, 300);
    _btn->setAligment(LV_ALIGN_CENTER, 0, 0);

    _btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";
        widgets::Toast::showToast("点击事件，检测到点击", widgets::Toast::Type::Warn);
        // lv_subject_copy_string(&_subject, _txt.c_str());
    });
}

void HomePage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
    // sys::Activity::onNotifyUI(evt);
}

void HomePage::onDestroy()
{}
