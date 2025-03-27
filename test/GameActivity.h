#ifndef GAME_ACTIVITY_H
#define GAME_ACTIVITY_H

#include "GameStateDialog.h"
#include "core/sys/BaseActivity.h"
#include "core/sys/Event.h"
#include "core/widgets/LvButton.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <src/misc/lv_area.h>

using namespace lvglpp;

class GameActivity : public ::sys::BaseActivity
{
public:
    struct GameState {
        std::list<lv_point_t> body_indexs;

        std::vector<std::shared_ptr<widgets::BaseItem>> _images; // 蛇体部件集合

        bool       exist_food{false}; // 是否有食物
        lv_point_t food{0, 0};        // 食物部件

        std::shared_ptr<widgets::LvText> score_label; // 分数显示

        lv_dir_t direction{lv_dir_t::LV_DIR_RIGHT}; // 当前移动方向
        // lv_dir_t direction{lv_dir_t::LV_DIR_TOP}; // 当up移动方向

        int score{0}; // 当前得分

        bool game_over{false}; // 游戏结束标志

        enum State {
            INIT = 0,
            RUNNING,
            PAUSE,
            ENDING,
        } state;

        void log()
        {
            // for (int i = 0; i < body_indexs.size(); i++) {
            //     std::cout << "index: " << i << body_indexs[i].x << body_indexs[i].y << std::endl;
            // }
        }
    };

    explicit GameActivity();

    ~GameActivity();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

    void init();

    /// [snake]
    lv_point_t getRandomPos();

    lv_point_t createFood();

    bool checkCollision();

    void move();

    void updateFrame();

    /// [snake]

private:
    static constexpr int box_length   = 20;
    static constexpr int hor_box_size = (WINDOW_WIDTH - 200) / 20; //  800 / 20 = 40 左右各留100像素，用来装控制按钮
    static constexpr int ver_box_size = WINDOW_HEIGHT / 20;        // 480 / 20 = 24

    lv_timer_t *_timer;

    std::shared_ptr<widgets::RoundedButton> _btn_up;
    std::shared_ptr<widgets::RoundedButton> _btn_left;
    std::shared_ptr<widgets::RoundedButton> _btn_right;
    std::shared_ptr<widgets::RoundedButton> _btn_down;

    std::shared_ptr<widgets::BaseItem> _canvas;

    GameState _game_state;

    std::shared_ptr<GameStateDialog> _dialog;
};

#endif // GAME_ACTIVITY_H