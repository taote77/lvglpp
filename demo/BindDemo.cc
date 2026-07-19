#include "BindDemo.h"
#include "config/color_sheet.h"
#include "core/sys/StackView.h"
#include "core/widgets/BaseItem.h"
#include "thorvg.h"
#include <iostream>
#include <src/others/observer/lv_observer.h>

BindDemo::BindDemo() : sys::BaseActivity()
{}

BindDemo::~BindDemo()
{}

void BindDemo::onCreate(std::any arg)
{
    // _label = std::make_shared<lvglpp::widgets::LvText>(_txt, CLR_SUCCESS_CONTAINER, getRoot());

    // _label->setAligment(LV_ALIGN_CENTER, 0, -80);

    // _gif = std::make_shared<widgets::GifImage>("G:/gif/bird.gif", getRoot());
    // _gif->setPos(0, 0);

    _lottie = std::make_shared<widgets::LottieCanvas>("asset/lottie/camera.json", getRoot());
    // _lottie->setPos(0, 0);
    _lottie->setCacheSize(190, 120);
    _lottie->setAligment(LV_ALIGN_CENTER, 0, 0);

    // _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Click", getRoot());

    // char buf[100]     = {0};
    // char pre_buf[100] = {0};
    // lv_subject_init_string(&_subject, buf, pre_buf, (100), "Start");
    // lv_label_bind_text(_label->getLvglItem(), &_subject, "%s");

    // lv_subject_add_observer(
    //     &_subject,
    //     [](lv_observer_t *observer, lv_subject_t *subject) {
    //         // auto value = lv_subject_get_string(subject);
    //         // std::cout << "value: " << value << std::endl;
    //     },
    //     nullptr);

    // _btn->setPos(480, 300);
    // _btn->setAligment(LV_ALIGN_CENTER, 0, 0);

    // _btn->setOnClickedListener([this]() {
    //     _txt = (_txt == "Start") ? "Stop" : "Start";
    //     lv_subject_copy_string(&_subject, _txt.c_str());
    // });

    // _img = std::make_shared<widgets::Image>(":/image/400x300/sky.png", getRoot());
    // // _img->setSize(200, 200);
    // _img->setAligment(LV_ALIGN_TOP_RIGHT, 0, 0);

    // widgets::BaseItem *src = new widgets::BaseItem(getRoot());
    // src->setSize(256, 217);
    // lv_example_lottie(getRoot()->getLvglItem()); // 创建演示

    // src->setAligment(LV_ALIGN_BOTTOM_LEFT, -60, 60);
}

void BindDemo::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void BindDemo::onDestroy()
{}
