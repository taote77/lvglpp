#include "GameActivity.h"

#include "core/widgets/GlobalVar.h"

GameActivity::GameActivity() : sys::BaseActivity()
{
    this->getRootWindow()->setBgColor(0xffffff);

    init();
    //
    update();
}

GameActivity::~GameActivity()
{
    //
    //
}

void GameActivity::onCreate(void *arg)
{
    //
    //
}

void GameActivity::onNotifyUI(const sys::Event &evt)
{
    //
    //
}

void GameActivity::onDestroy()
{
    //
    //
}

void GameActivity::init()
{
    _snake_body.emplace_back(Point{ 10, 10 });
    _snake_body.emplace_back(Point{ 10, 11 });
    _snake_body.emplace_back(Point{ 10, 12 });
    _snake_body.emplace_back(Point{ 11, 12 });
}

void GameActivity::update()
{
    //
    //
    for (auto &point : _snake_body) {

        auto box = std::make_shared<widgets::BaseItem>(this->getRootWindow());
        box->setSize(hor_box_size, ver_box_size);
        box->setPos(point.x * hor_box_size, point.y * ver_box_size);
        box->setBgColor(0x000000);
        // box->setBorderColor(lv_color_hex(0xffffff));
        // box->setBorderWidth(1);
    }
}