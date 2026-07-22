#include "Animation.h"

namespace lvglpp::sys {

Animation::Animation()
{
    lv_anim_init(&_anim);
}

Animation::~Animation()
{
    // Don't call stop() — that would trigger deleted_cb which may reference freed memory
    lv_anim_delete(this, _lvglExecCallback);
}

Animation::Animation(Animation &&other) noexcept
{
    _anim = other._anim;
    _execCb = std::move(other._execCb);
    _startCb = std::move(other._startCb);
    _completedCb = std::move(other._completedCb);
    _deletedCb = std::move(other._deletedCb);
    _target = other._target;
    _duration = other._duration;

    // Update user_data pointer in LVGL animation
    lv_anim_set_var(&_anim, this);

    other._target = nullptr;
    other._duration = 0;
}

Animation &Animation::operator=(Animation &&other) noexcept
{
    if (this != &other) {
        lv_anim_delete(this, _lvglExecCallback);

        _anim = other._anim;
        _execCb = std::move(other._execCb);
        _startCb = std::move(other._startCb);
        _completedCb = std::move(other._completedCb);
        _deletedCb = std::move(other._deletedCb);
        _target = other._target;
        _duration = other._duration;

        lv_anim_set_var(&_anim, this);

        other._target = nullptr;
        other._duration = 0;
    }
    return *this;
}

Animation &Animation::setTarget(widgets::BaseItem *target)
{
    _target = target;
    return *this;
}

Animation &Animation::setDuration(uint32_t ms)
{
    _duration = ms;
    lv_anim_set_duration(&_anim, ms);
    return *this;
}

uint32_t Animation::duration() const
{
    return _duration;
}

Animation &Animation::setValues(int32_t start, int32_t end)
{
    lv_anim_set_values(&_anim, start, end);
    return *this;
}

Animation &Animation::setExecCallback(AnimCallback cb)
{
    _execCb = std::move(cb);
    lv_anim_set_exec_cb(&_anim, _lvglExecCallback);
    lv_anim_set_var(&_anim, this);
    return *this;
}

Animation &Animation::setDelay(uint32_t ms)
{
    lv_anim_set_delay(&_anim, ms);
    return *this;
}

Animation &Animation::setRepeatCount(uint32_t count)
{
    lv_anim_set_repeat_count(&_anim, count);
    return *this;
}

Animation &Animation::setRepeatDelay(uint32_t ms)
{
    lv_anim_set_reverse_delay(&_anim, ms);
    return *this;
}

Animation &Animation::setReverseDuration(uint32_t ms)
{
    lv_anim_set_reverse_duration(&_anim, ms);
    return *this;
}

Animation &Animation::setReverseDelay(uint32_t ms)
{
    lv_anim_set_reverse_delay(&_anim, ms);
    return *this;
}

Animation &Animation::setStartCallback(std::function<void()> cb)
{
    _startCb = std::move(cb);
    lv_anim_set_start_cb(&_anim, _lvglStartCallback);
    return *this;
}

Animation &Animation::setCompleteCallback(std::function<void()> cb)
{
    _completedCb = std::move(cb);
    lv_anim_set_completed_cb(&_anim, _lvglCompletedCallback);
    return *this;
}

Animation &Animation::setDeletedCallback(std::function<void()> cb)
{
    _deletedCb = std::move(cb);
    lv_anim_set_deleted_cb(&_anim, _lvglDeletedCallback);
    return *this;
}

Animation &Animation::setPathCallback(lv_anim_path_cb_t pathCb)
{
    lv_anim_set_path_cb(&_anim, pathCb);
    return *this;
}

void Animation::start()
{
    if (!_target || !_execCb) return;
    lv_anim_set_var(&_anim, this);
    lv_anim_start(&_anim);
}

void Animation::stop()
{
    lv_anim_delete(this, _lvglExecCallback);
}

bool Animation::isRunning() const
{
    return lv_anim_get_playtime(&_anim) < _duration;
}

// Static callbacks
void Animation::_lvglExecCallback(void *var, int32_t value)
{
    auto *self = static_cast<Animation *>(var);
    if (self && self->_execCb) {
        self->_execCb(self->_target, value);
    }
}

void Animation::_lvglStartCallback(lv_anim_t *a)
{
    auto *self = static_cast<Animation *>(lv_anim_get_user_data(a));
    if (self && self->_startCb) {
        self->_startCb();
    }
}

void Animation::_lvglCompletedCallback(lv_anim_t *a)
{
    auto *self = static_cast<Animation *>(lv_anim_get_user_data(a));
    if (self && self->_completedCb) {
        self->_completedCb();
    }
}

void Animation::_lvglDeletedCallback(lv_anim_t *a)
{
    auto *self = static_cast<Animation *>(lv_anim_get_user_data(a));
    if (self && self->_deletedCb) {
        self->_deletedCb();
    }
}

} // namespace lvglpp::sys
