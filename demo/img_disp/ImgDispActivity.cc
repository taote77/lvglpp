#include "ImgDispActivity.h"
#include "config/color_sheet.h"
#include "core/widgets/Widget.h"
#include <iostream>

ImgDispActivity::ImgDispActivity() : sys::BaseActivity()
{}

ImgDispActivity::~ImgDispActivity()
{}

void ImgDispActivity::onCreate(std::any arg)
{
    _gif = std::make_shared<widgets::GifImage>("gif/bird.gif", getRoot());
    _gif->setPos(0, 0);

    _img = std::make_shared<widgets::Image>("image/400x300/sky.png", getRoot());
    // _img->setSize(200, 200);
    _img->setAlignment(LV_ALIGN_TOP_RIGHT, 0, 0);
}

void ImgDispActivity::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void ImgDispActivity::onDestroy()
{}
