#include "GameActivity.h"

#include "core/widgets/GlobalVar.h"

GameActivity::GameActivity() : sys::BaseActivity()
{
    //
    //
}

GameActivity::~GameActivity()
{
    //
    //
}

void GameActivity::onCreate(void *arg)
{

    BaseActivity::onCreate(arg);
    this->getRootWindow()->setBgColor(CLR_PRIMARY_BRIGHT);

    _timer = lv_timer_create(
            [](lv_timer_t *t) -> void {
                auto user_data = lv_timer_get_user_data(t);
                auto p = (GameActivity *)user_data;

                // auto p = (GameActivity *)t->user_data;
                if (p != nullptr) {
                    //
                    for (auto &point : p->_snake_body) {
                        point.x++;
                    }

                    p->update();
                }
            },
            10 * 1000, this);

    init();
    // //
    update();
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
        box->setSize(box_length, box_length);
        box->setPos(point.x * box_length, point.y * box_length);
        box->setBgColor(0x000000);

        // box->setBorderColor(lv_color_hex(0xffffff));
        // box->setBorderWidth(1);
    }
}