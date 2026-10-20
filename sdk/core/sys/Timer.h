#ifndef LVGLPP_TIMER_H
#define LVGLPP_TIMER_H

#include "lvgl.h"

#include <cstdint>
#include <functional>
#include <memory>

namespace lvglpp::sys {

/**
 * C++ wrapper around lv_timer_t.
 *
 * Unlike the old implementation a one-shot timer never deletes itself: the
 * wrapper remains valid and can be restarted or destroyed by its owner. The
 * callback is safe to destroy the owning Timer.
 */
class Timer
{
public:
    enum class Mode {
        Periodic,
        OneShot,
    };

    using Callback = std::function<void(Timer *)>;

    ~Timer();

    Timer(const Timer &)            = delete;
    Timer &operator=(const Timer &) = delete;

    /// Create and start a periodic timer.
    static std::unique_ptr<Timer> create(Callback cb, uint32_t periodMs);

    /// Compatibility overload: oneShot=true creates a one-shot timer.
    static std::unique_ptr<Timer> create(Callback cb, uint32_t periodMs, bool oneShot);

    static std::unique_ptr<Timer> create(Callback cb, uint32_t periodMs, Mode mode);

    /// Start (or restart) the timer.
    void start();

    /// Stop and release the LVGL timer.
    void stop();

    /// Stop (if needed) and start again.
    void restart();

    /// Pause the timer; call resume() to continue.
    void pause();

    /// Resume a paused timer.
    void resume();

    /// Reset the timer to fire immediately.
    void setReady();

    void setPeriod(uint32_t periodMs);

    uint32_t period() const
    {
        return _periodMs;
    }

    bool isPaused() const
    {
        return _paused;
    }

    bool isRunning() const;

    /// True for a one-shot timer after its callback has run.
    bool isFinished() const
    {
        return _finished;
    }

    Mode mode() const
    {
        return _mode;
    }

    lv_timer_t *handle() const
    {
        return _timer;
    }

private:
    Timer(Callback cb, uint32_t periodMs, Mode mode);

    static void _lvglCallback(lv_timer_t *t);

    void schedule();

    Callback   _cb;
    uint32_t   _periodMs = 0;
    Mode       _mode     = Mode::Periodic;
    bool       _paused   = false;
    bool       _finished = false;
    lv_timer_t *_timer   = nullptr;
};

} // namespace lvglpp::sys

#endif // LVGLPP_TIMER_H
