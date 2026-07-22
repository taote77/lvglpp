#include "Timer.h"

namespace lvglpp::sys {

Timer::Timer(Callback cb, uint32_t periodMs, bool oneShot)
    : _cb(std::move(cb))
    , _periodMs(periodMs)
    , _oneShot(oneShot)
{}

Timer::~Timer()
{
    if (_timer) {
        lv_timer_delete(_timer);
        _timer = nullptr;
    }
}

std::unique_ptr<Timer> Timer::create(Callback cb, uint32_t periodMs)
{
    return create(std::move(cb), periodMs, false);
}

std::unique_ptr<Timer> Timer::create(Callback cb, uint32_t periodMs, bool oneShot)
{
    auto timer = std::unique_ptr<Timer>(new Timer(std::move(cb), periodMs, oneShot));
    timer->_timer = lv_timer_create(
        _lvglCallback,
        periodMs,
        timer.get()
    );
    return timer;
}

void Timer::_lvglCallback(lv_timer_t *t)
{
    auto *self = static_cast<Timer *>(lv_timer_get_user_data(t));
    if (!self) return;

    if (self->_cb) {
        self->_cb(self);
    }

    if (self->_oneShot) {
        // Mark timer as already-deleted so destructor doesn't double-free
        self->_timer = nullptr;
        delete self;
    }
}

void Timer::pause()
{
    if (_timer) {
        lv_timer_pause(_timer);
        _paused = true;
    }
}

void Timer::resume()
{
    if (_timer) {
        lv_timer_resume(_timer);
        _paused = false;
    }
}

void Timer::setReady()
{
    if (_timer) {
        lv_timer_ready(_timer);
    }
}

void Timer::setPeriod(uint32_t periodMs)
{
    _periodMs = periodMs;
    if (_timer) {
        lv_timer_set_period(_timer, periodMs);
    }
}

} // namespace lvglpp::sys
