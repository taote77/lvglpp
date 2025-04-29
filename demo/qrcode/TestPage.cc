#include "TestPage.h"
#include "core/sys/Navigators.h"
#include "core/widgets/GlobalVar.h"
#include <iostream>

using namespace lvglpp::core;

TestPage::TestPage() : sys::BaseActivity()
{}

TestPage::~TestPage()
{}

void TestPage::onCreate(void *arg)
{
    lv_color_t bg_color = lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 5);
    lv_color_t fg_color = lv_palette_darken(LV_PALETTE_BLUE, 4);

    _qr_code = std::make_shared<lvglpp::widgets::QrCode>(200, bg_color, fg_color, getRootItem());

    const std::string info = "https://www.baidu.com";
    _qr_code->setData(info);
    _qr_code->setAligment(LV_ALIGN_CENTER, 0, 0);
}

void TestPage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void TestPage::onDestroy()
{}
