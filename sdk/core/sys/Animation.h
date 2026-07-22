#ifndef LVGLPP_ANIMATION_H
#define LVGLPP_ANIMATION_H

#include "core/widgets/BaseItem.h"
#include <functional>

namespace lvglpp::sys {

/**
 * @brief C++ wrapper around LVGL lv_anim_t for declarative animations.
 *
 * Usage:
 *   Animation anim;
 *   anim.setTarget(item)
 *       .setDuration(300)
 *       .setValues(0, 100)
 *       .setExecCallback([](widgets::BaseItem *target, int32_t value) {
 *           target->setOpacity(value / 100.0);
 *       })
 *       .start();
 */
class Animation
{
public:
    using AnimCallback = std::function<void(widgets::BaseItem *, int32_t)>;

    Animation();
    ~Animation();

    Animation(const Animation &) = delete;
    Animation &operator=(const Animation &) = delete;
    Animation(Animation &&other) noexcept;
    Animation &operator=(Animation &&other) noexcept;

    /// Set the target widget
    Animation &setTarget(widgets::BaseItem *target);

    /// Set duration in milliseconds
    Animation &setDuration(uint32_t ms);

    /// Get current duration
    uint32_t duration() const;

    /// Set start and end values
    Animation &setValues(int32_t start, int32_t end);

    /// Set a custom execution callback (called each frame with interpolated value)
    Animation &setExecCallback(AnimCallback cb);

    /// Set playback delay in ms (before animation starts)
    Animation &setDelay(uint32_t ms);

    /// Set repeat count: 0 = once, LV_ANIM_REPEAT_INFINITE = forever
    Animation &setRepeatCount(uint32_t count);

    /// Set repeat delay in ms between repeats
    Animation &setRepeatDelay(uint32_t ms);

    /// Set reverse playback duration (for reverse phase)
    Animation &setReverseDuration(uint32_t ms);

    /// Set reverse playback delay
    Animation &setReverseDelay(uint32_t ms);

    /// Set a callback for when the animation starts
    Animation &setStartCallback(std::function<void()> cb);

    /// Set a callback for when the animation completes
    Animation &setCompleteCallback(std::function<void()> cb);

    /// Set a callback for when the animation is deleted
    Animation &setDeletedCallback(std::function<void()> cb);

    /// Set a custom path (easing) callback
    Animation &setPathCallback(lv_anim_path_cb_t pathCb);

    /// Start the animation
    void start();

    /// Stop the animation and call the deleted callback
    void stop();

    /// Check if animation has completed (playtime >= duration)
    bool isRunning() const;

    /// Get the underlying LVGL animation (advanced use)
    lv_anim_t *handle() { return &_anim; }

private:
    static void _lvglExecCallback(void *var, int32_t value);
    static void _lvglStartCallback(lv_anim_t *a);
    static void _lvglCompletedCallback(lv_anim_t *a);
    static void _lvglDeletedCallback(lv_anim_t *a);

    lv_anim_t             _anim;
    AnimCallback           _execCb;
    std::function<void()>  _startCb;
    std::function<void()>  _completedCb;
    std::function<void()>  _deletedCb;
    widgets::BaseItem     *_target = nullptr;
    uint32_t               _duration = 0;
};

} // namespace lvglpp::sys

#endif // LVGLPP_ANIMATION_H
