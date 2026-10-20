#include "TestFramework.h"
#include "core/sys/Animation.h"
#include "core/sys/Timer.h"
#include "core/widgets/BaseItem.h"
#include "lvgl.h"

#include <memory>

using namespace lvglpp::sys;
using namespace lvglpp::test;
using namespace lvglpp::widgets;

TEST(Timer_one_shot_does_not_self_delete)
{
    int  calls = 0;
    auto timer = Timer::create([&calls](Timer *) { calls++; }, 10, true);

    CHECK_TRUE(timer != nullptr);
    CHECK_TRUE(timer->isRunning());

    lv_timer_ready(timer->handle());
    lv_timer_handler();

    CHECK_EQ(calls, 1);
    CHECK_TRUE(timer->isFinished());
    CHECK_FALSE(timer->isRunning());
    CHECK_TRUE(timer->handle() != nullptr); // wrapper still owns a valid lv_timer_t

    timer.reset(); // must not double free
    return true;
}

TEST(Timer_periodic_pause_resume)
{
    int  calls = 0;
    auto timer = Timer::create([&calls](Timer *) { calls++; }, 10, Timer::Mode::Periodic);

    lv_timer_ready(timer->handle());
    lv_timer_handler();
    CHECK_EQ(calls, 1);

    timer->pause();
    CHECK_TRUE(timer->isPaused());
    lv_timer_ready(timer->handle());
    lv_timer_handler();
    CHECK_EQ(calls, 1); // paused

    timer->resume();
    CHECK_FALSE(timer->isPaused());
    lv_timer_ready(timer->handle());
    lv_timer_handler();
    CHECK_EQ(calls, 2);
    return true;
}

TEST(Timer_can_be_destroyed_inside_callback)
{
    std::unique_ptr<Timer> timer;
    int                    calls = 0;

    timer = Timer::create(
        [&timer, &calls](Timer *) {
            calls++;
            timer.reset();
        },
        5, true);

    lv_timer_ready(timer->handle());
    lv_timer_handler();

    CHECK_EQ(calls, 1);
    CHECK(timer == nullptr);
    return true;
}

TEST(Timer_can_restart_after_finish)
{
    int  calls = 0;
    auto timer = Timer::create([&calls](Timer *) { calls++; }, 10, true);

    lv_timer_ready(timer->handle());
    lv_timer_handler();
    CHECK_EQ(calls, 1);
    CHECK_TRUE(timer->isFinished());

    timer->start();
    CHECK_TRUE(timer->isRunning());
    CHECK_FALSE(timer->isFinished());

    lv_timer_ready(timer->handle());
    lv_timer_handler();
    CHECK_EQ(calls, 2);
    return true;
}

TEST(Animation_start_and_stop)
{
    BaseItem  target;
    Animation anim;
    int       ticks = 0;

    anim.setTarget(&target).setDuration(1000).setValues(0, 100).setExecCallback([&ticks](BaseItem *, int32_t) { ticks++; });

    anim.start();
    CHECK_TRUE(anim.isRunning());

    anim.stop();
    CHECK_FALSE(anim.isRunning());
    return true;
}

TEST(Animation_stops_when_target_is_destroyed)
{
    auto      target = std::make_unique<BaseItem>();
    Animation anim;
    int       ticks = 0;

    anim.setTarget(target.get()).setDuration(1000).setValues(0, 100).setRepeatCount(LV_ANIM_REPEAT_INFINITE).setExecCallback([&ticks](BaseItem *, int32_t) { ticks++; });

    anim.start();
    CHECK_TRUE(anim.isRunning());

    target.reset();
    CHECK_FALSE(anim.isRunning());
    return true;
}

TEST(Animation_is_safe_to_move_after_start)
{
    BaseItem  target;
    Animation anim;
    int       ticks = 0;

    anim.setTarget(&target).setDuration(1000).setValues(0, 100).setRepeatCount(LV_ANIM_REPEAT_INFINITE).setExecCallback([&ticks](BaseItem *, int32_t) { ticks++; });

    anim.start();

    Animation moved(std::move(anim));
    CHECK_TRUE(moved.isRunning());
    moved.stop();
    CHECK_FALSE(moved.isRunning());
    return true;
}
