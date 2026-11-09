#include "Timer.h"

namespace lvglpp::sys {

Timer::Timer(Callback cb, uint32_t periodMs, Mode mode)
    : _cb(std::move(cb))
    , _periodMs(periodMs)
    , _mode(mode)
{}

Timer::~Timer()
{
    stop();
}

std::unique_ptr<Timer> Timer::create(Callback cb, uint32_t periodMs)
{
    return create(std::move(cb), periodMs, Mode::Periodic);
}

std::unique_ptr<Timer> Timer::create(Callback cb, uint32_t periodMs, bool oneShot)
{
    return create(std::move(cb), periodMs, oneShot ? Mode::OneShot : Mode::Periodic);
}

std::unique_ptr<Timer> Timer::create(Callback cb, uint32_t periodMs, Mode mode)
{
    auto timer = std::unique_ptr<Timer>(new Timer(std::move(cb), periodMs, mode));
    timer->schedule();
    return timer;
}

void Timer::schedule()
{
    stop();

    _finished = false;
    _timer    = lv_timer_create(&Timer::_lvglCallback, _periodMs, this);

    if (_timer != nullptr && _mode == Mode::OneShot)
    {
        // Run exactly once, then LVGL pauses the timer. The wrapper keeps the
        // lv_timer_t alive so the owner stays in control of its lifetime.
        lv_timer_set_repeat_count(_timer, 1);
        lv_timer_set_auto_delete(_timer, false);
    }

    if (_timer != nullptr && _paused)
    {
        lv_timer_pause(_timer);
    }
}

void Timer::_lvglCallback(lv_timer_t *t)
{
    auto *self = static_cast<Timer *>(lv_timer_get_user_data(t));
    if (self == nullptr)
    {
        return;
    }

    // Copy before invoking: the callback may destroy `self`.
    auto callback = self->_cb;
    if (self->_mode == Mode::OneShot)
    {
        self->_finished = true;
    }

    if (callback)
    {
        callback(self);
    }
}

void Timer::start()
{
    if (_timer == nullptr || _finished)
    {
        schedule();
        _paused = false;
        return;
    }

    resume();
    setReady();
}

void Timer::stop()
{
    if (_timer != nullptr)
    {
        lv_timer_delete(_timer);
        _timer = nullptr;
    }
    _paused = false;
}

void Timer::restart()
{
    schedule();
    _paused = false;
}

void Timer::pause()
{
    if (_timer != nullptr)
    {
        lv_timer_pause(_timer);
        _paused = true;
    }
}

void Timer::resume()
{
    if (_timer != nullptr && !_finished)
    {
        lv_timer_resume(_timer);
        _paused = false;
    }
}

void Timer::setReady()
{
    if (_timer != nullptr)
    {
        lv_timer_ready(_timer);
    }
}

void Timer::setPeriod(uint32_t periodMs)
{
    _periodMs = periodMs;
    if (_timer != nullptr)
    {
        lv_timer_set_period(_timer, periodMs);
    }
}

bool Timer::isRunning() const
{
    if (_timer == nullptr || _finished)
    {
        return false;
    }
    return !lv_timer_get_paused(_timer);
}

} // namespace lvglpp::sys
