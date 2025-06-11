#include "Widget.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/GlobalVar.h"
#include <iostream>

Widget::Widget() : sys::BaseActivity()
{}

Widget::~Widget()
{}

void Widget::onCreate(void *arg)
{
    _gif = std::make_shared<widgets::GifImage>("G:/gif/bird.gif", getRootItem());
    _gif->setPos(0, 0);

    _img = std::make_shared<widgets::Image>(":/image/400x300/sky.png", getRootItem());
    // _img->setSize(200, 200);
    _img->setAligment(LV_ALIGN_TOP_RIGHT, 0, 0);
}

void Widget::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void Widget::onDestroy()
{}
