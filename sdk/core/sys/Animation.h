#ifndef LVGLPP_ANIMATION_H
#define LVGLPP_ANIMATION_H

#include "core/widgets/Widget.h"
#include <cstdint>
#include <functional>
#include <memory>

namespace lvglpp::sys {

/**
 * C++ wrapper around lv_anim_t.
 *
 * The LVGL animation stores a stable heap pointer (Impl), so moving the
 * Animation object after start() is safe. The animation stops automatically
 * when the target widget or its lv_obj is destroyed.
 */
class Animation
{
public:
    using AnimCallback = std::function<void(widgets::Widget *, int32_t)>;

    Animation();
    ~Animation();

    Animation(const Animation &)            = delete;
    Animation &operator=(const Animation &) = delete;
    Animation(Animation &&) noexcept;
    Animation &operator=(Animation &&) noexcept;

    Animation &setTarget(widgets::Widget *target);

    Animation &setDuration(uint32_t ms);

    uint32_t duration() const;

    Animation &setValues(int32_t start, int32_t end);

    Animation &setExecCallback(AnimCallback cb);

    Animation &setDelay(uint32_t ms);

    Animation &setRepeatCount(uint32_t count);

    Animation &setRepeatDelay(uint32_t ms);

    Animation &setReverseDuration(uint32_t ms);

    Animation &setReverseDelay(uint32_t ms);

    Animation &setStartCallback(std::function<void()> cb);

    Animation &setCompleteCallback(std::function<void()> cb);

    Animation &setDeletedCallback(std::function<void()> cb);

    Animation &setPathCallback(lv_anim_path_cb_t pathCb);

    void start();

    void stop();

    bool isRunning() const;

    bool isFinished() const;

    lv_anim_t *handle();

private:
    struct Impl;

    static void execThunk(void *var, int32_t value);

    static void startThunk(lv_anim_t *a);

    static void completedThunk(lv_anim_t *a);

    static void deletedThunk(lv_anim_t *a);

    std::unique_ptr<Impl> impl_;
};

} // namespace lvglpp::sys

#endif // LVGLPP_ANIMATION_H
