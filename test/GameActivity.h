#ifndef GAME_ACTIVITY_H
#define GAME_ACTIVITY_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/LvText.h"
#include "core/widgets/LvButton.h"
#include "core/sys/Event.h"
#include <memory>
#include <list>

using namespace lvglpp;

class GameActivity : public ::sys::BaseActivity
{
public:
    explicit GameActivity();
    ~GameActivity();

protected:
    void onCreate(void *arg) override;

    void onNotifyUI(const sys::Event &evt) override;

    void onDestroy() override;

    void init();

    void update();

private:
    struct Point
    {
        short x;
        short y;
    };

    static constexpr int hor_box_size = WINDOW_WIDTH / 20; //  800 / 20 = 40
    static constexpr int ver_box_size = WINDOW_HEIGHT / 20; // 480 / 20 = 24

    std::list<Point> _snake_body;

    std::shared_ptr<widgets::LvButton> _btn;

    std::vector<std::shared_ptr<widgets::BaseItem>> _texts;
};

#endif // GAME_ACTIVITY_H