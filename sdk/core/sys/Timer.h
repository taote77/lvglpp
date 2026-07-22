#ifndef LVGLPP_TIMER_H
#define LVGLPP_TIMER_H

#include "lvgl.h"
#include <functional>
#include <memory>

namespace lvglpp::sys {

/**
 * @brief C++ wrapper around LVGL lv_timer for periodic and one-shot callbacks.
 *
 * Usage:
 *   auto t = Timer::create([](Timer*) { ... }, 500);       // periodic every 500ms
 *   auto t = Timer::create([](Timer*) { ... }, 1000, true); // one-shot after 1s
 */
class Timer
{
public:
    using Callback = std::function<void(Timer *)>;

    ~Timer();

    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;

    /// Create and start a periodic timer
    static std::unique_ptr<Timer> create(Callback cb, uint32_t periodMs);

    /// Create a timer (oneShot=true runs once, then self-deletes)
    static std::unique_ptr<Timer> create(Callback cb, uint32_t periodMs, bool oneShot);

    /// Pause the timer; call resume() to restart
    void pause();

    /// Resume a paused timer
    void resume();

    /// Reset the timer to fire immediately (ignores pending period)
    void setReady();

    /// Change the callback period
    void setPeriod(uint32_t periodMs);

    /// Get current period in ms
    uint32_t period() const { return _periodMs; }

    /// Check if timer is paused
    bool isPaused() const { return _paused; }

    /// The underlying LVGL timer handle (for advanced use)
    lv_timer_t *handle() const { return _timer; }

private:
    Timer(Callback cb, uint32_t periodMs, bool oneShot);

    static void _lvglCallback(lv_timer_t *t);

    Callback   _cb;
    uint32_t   _periodMs;
    bool       _oneShot;
    bool       _paused = false;
    lv_timer_t *_timer = nullptr;
};

} // namespace lvglpp::sys

#endif // LVGLPP_TIMER_H
