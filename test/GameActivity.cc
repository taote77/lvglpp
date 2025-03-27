#include "GameActivity.h"
#include "GameStateDialog.h"
#include "core/widgets/GlobalVar.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>
#include <src/misc/lv_area.h>

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

    init();

    _timer = lv_timer_create(
        [](lv_timer_t *t) -> void {
            auto user_data = lv_timer_get_user_data(t);
            auto page      = (GameActivity *)user_data;

            auto &game_state = page->_game_state; // careful !!!, do not left reference &

            if (page != nullptr)
            {
                page->move();

                page->updateFrame();
            }
        },
        1 * 1000, this);

    updateFrame();
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
    _btn_up = std::make_shared<widgets::RoundedButton>(70, 40, widgets::RoundedButton::ColorStyle::Gray, "Start", getRootWindow());
    _btn_up->setPos(20, 100);
    _btn_up->setOnClickedListener([this]() -> void {
        //
        _game_state.direction = LV_DIR_TOP;
    });

    _btn_left = std::make_shared<widgets::RoundedButton>(70, 40, widgets::RoundedButton::ColorStyle::Gray, "Left", getRootWindow());
    _btn_left->setPos(20, 220);
    _btn_left->setOnClickedListener([this]() -> void {
        //
        _game_state.direction = LV_DIR_LEFT;
    });
    _btn_right = std::make_shared<widgets::RoundedButton>(70, 40, widgets::RoundedButton::ColorStyle::Gray, "Right", getRootWindow());
    _btn_right->setPos(720, 100);
    _btn_right->setOnClickedListener([this]() -> void {
        //
        _game_state.direction = LV_DIR_RIGHT;
    });

    _btn_down = std::make_shared<widgets::RoundedButton>(70, 40, widgets::RoundedButton::ColorStyle::Gray, "Down", getRootWindow());
    _btn_down->setPos(720, 220);

    _btn_down->setOnClickedListener([this]() -> void {
        //
        _game_state.direction = LV_DIR_BOTTOM;
    });

    _canvas = std::make_shared<widgets::BaseItem>(this->getRootWindow());
    _canvas->setSize(600, 480);
    _canvas->setPos(100, 0);

    _dialog = std::make_shared<GameStateDialog>();

    _game_state.body_indexs.emplace_back(lv_point_t{10, 10});
    _game_state.body_indexs.emplace_back(lv_point_t{10, 11});
    _game_state.body_indexs.emplace_back(lv_point_t{10, 12});
    _game_state.body_indexs.emplace_back(lv_point_t{11, 12});
}

lv_point_t GameActivity::getRandomPos()
{
    static std::random_device                 rd;
    static std::mt19937                       gen(rd());
    static std::uniform_int_distribution<int> x_dist(0, 30);
    static std::uniform_int_distribution<int> y_dist(0, 24);

    return lv_point_t{x_dist(gen), y_dist(gen)};
}

lv_point_t GameActivity::createFood()
{
    static auto existingInBody = [this](const lv_point_t &pos) -> bool {
        //
        bool exist{false};
        for (auto &point : _game_state.body_indexs)
        {
            if (point.x == pos.x && point.y == pos.y)
            {
                return true;
            }
        }
        return false;
    };

    while (!_game_state.exist_food && true)
    {
        auto pos = getRandomPos();
        if (!existingInBody(pos))
        {
            return pos;
        }
    }
}

bool GameActivity::checkCollision()
{
    if (_game_state.body_indexs.empty())
    {
        return true;
    }

    // std::cout << "size: " << this->getRootWindow()->getWidth() <<
    // this->getRootWindow()->getHeight(); std::cout << "nums: " << hor_box_size << ver_box_size <<
    // std::endl;

    auto head = _game_state.body_indexs.begin();

    // std::cout << "pos: " << head->x << head->y << std::endl;

    return (head->x < 0 || head->x >= hor_box_size || head->y < 0 || head->y >= ver_box_size);
}

void GameActivity::move()
{
    auto &indexs = _game_state.body_indexs;
    if (indexs.empty())
    {
        std::cout << "empty: " << indexs.size() << std::endl;
        return;
    }

    auto head = indexs.begin();
    auto tail = --indexs.end();

    for (auto &point : indexs)
    {
        switch (_game_state.direction)
        {
        case lv_dir_t::LV_DIR_RIGHT:
            tail->x = head->x + 1;
            tail->y = head->y;
            break;
        case lv_dir_t::LV_DIR_BOTTOM:
            tail->x = head->x;
            tail->y = head->y + 1;
            break;
        case lv_dir_t::LV_DIR_LEFT:
            tail->x = head->x - 1;
            tail->y = head->y;
            break;
        case lv_dir_t::LV_DIR_TOP:
            tail->x = head->x;
            tail->y = head->y - 1;
            break;
        default:
            break;
        };
    }

    indexs.erase(tail);
    indexs.emplace_front(*tail);
}

void GameActivity::updateFrame()
{
    auto parent = _canvas->getLvglItem();
    lv_obj_clean(parent); // 删除所有子对象

    if (!_game_state.exist_food)
    {
        _game_state.food       = createFood();
        _game_state.exist_food = true;
    }

    auto box = std::make_shared<widgets::BaseItem>(_canvas.get());
    box->setSize(box_length, box_length);
    box->setPos(_game_state.food.x * box_length, _game_state.food.y * box_length);
    box->setBgColor(0x00ff00);
    box->setRadius(10);

    if (checkCollision())
    {
        _dialog->open();
        std::cerr << "***********************************************************************";
        std::cerr << "**************************     STOP      ******************************";
        std::cerr << "***********************************************************************";
    }

    bool head{true};
    for (auto &point : _game_state.body_indexs)
    {
        auto box = std::make_shared<widgets::BaseItem>(_canvas.get());
        box->setSize(box_length, box_length);
        box->setPos(point.x * box_length, point.y * box_length);
        box->setBgColor(0x000000);

        if (head)
        {
            box->setRadius(6);
            head = false;
        }
    }
}