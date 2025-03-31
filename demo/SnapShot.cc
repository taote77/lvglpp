#include "SnapShot.h"
#include "core/sys/Navigators.h"
#include "core/widgets/GlobalVar.h"
#include <iostream>
#include <src/others/observer/lv_observer.h>

SnapShot::SnapShot() : sys::BaseActivity()
{}

SnapShot::~SnapShot()
{}

void SnapShot::onCreate(void *arg)
{
    _label = std::make_shared<lvglpp::widgets::LvText>(_txt, CLR_SUCCESS_CONTAINER, getRootItem());

    _label->setAligment(LV_ALIGN_CENTER, 0, -80);

    _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Click", getRootItem());

    _btn->setPos(480, 300);
    _btn->setAligment(LV_ALIGN_CENTER, 0, 0);

    _btn->setOnClickedListener([this]() { _txt = (_txt == "Start") ? "Stop" : "Start"; });
}

void SnapShot::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void SnapShot::onDestroy()
{}

void SnapShot::takeShot()
{
    auto widget = getRootItem()->getLvglItem();

    lv_color_format_t cf;

    auto snapshot = lv_snapshot_take(widget, cf);

    std::string filename{"./qq.bmp"};

    // save_as_bmp_file(snapshot->data, snapshot->header.w, snapshot->header.h, 32, filename);

    lv_draw_buf_destroy(snapshot);
}