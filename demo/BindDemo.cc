#include "BindDemo.h"
#include "core/sys/Navigators.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/GlobalVar.h"
#include "thorvg.h"
#include <iostream>
#include <src/misc/lv_area.h>
#include <src/others/observer/lv_observer.h>

void lv_example_lottie(lv_obj_t *parent)
{
    lv_obj_t *lottie = lv_lottie_create(parent);
    lv_lottie_set_src_file(lottie, "/home/shane/workspace/lvglpp/build/target/bin/asset/lottie/out.json");

#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    /*If there are no special requirements, just declare a buffer
      x4 because the Lottie is rendered in ARGB8888 format*/
    static uint8_t buf[256 * 217 * 4];
    lv_lottie_set_buffer(lottie, 256, 217, buf);
#else
    /*For GPUs and special alignment/strid setting use a draw_buf instead*/
    LV_DRAW_BUF_DEFINE(draw_buf, 256, 217, LV_COLOR_FORMAT_ARGB8888);
    lv_lottie_set_draw_buf(lottie, &draw_buf);
#endif

    lv_obj_center(lottie);
}

BindDemo::BindDemo() : sys::BaseActivity()
{}

BindDemo::~BindDemo()
{}

void BindDemo::onCreate(void *arg)
{
    _label = std::make_shared<lvglpp::widgets::LvText>(_txt, CLR_SUCCESS_CONTAINER, getRootItem());

    _label->setAligment(LV_ALIGN_CENTER, 0, -80);

    _gif = std::make_shared<widgets::GifImage>("G:/gif/bird.gif", getRootItem());
    _gif->setPos(0, 0);

    _btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "Click", getRootItem());

    char buf[100]     = {0};
    char pre_buf[100] = {0};
    lv_subject_init_string(&_subject, buf, pre_buf, (100), "Start");
    lv_label_bind_text(_label->getLvglItem(), &_subject, "%s");

    lv_subject_add_observer(
        &_subject,
        [](lv_observer_t *observer, lv_subject_t *subject) {
            // auto value = lv_subject_get_string(subject);
            // std::cout << "value: " << value << std::endl;
        },
        nullptr);

    _btn->setPos(480, 300);
    _btn->setAligment(LV_ALIGN_CENTER, 0, 0);

    _btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";
        lv_subject_copy_string(&_subject, _txt.c_str());
    });

    _img = std::make_shared<widgets::Image>(":/image/400x300/sky.png", getRootItem());
    // _img->setSize(200, 200);
    _img->setAligment(LV_ALIGN_TOP_RIGHT, 0, 0);

    widgets::BaseItem *src = new widgets::BaseItem(getRootItem());
    src->setSize(256, 217);
    // lv_example_lottie(src->getLvglItem()); // 创建演示

    src->setAligment(LV_ALIGN_BOTTOM_LEFT, -60, 60);
}

void BindDemo::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void BindDemo::onDestroy()
{}
