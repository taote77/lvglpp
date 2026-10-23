#include "Animation.h"

namespace lvglpp::sys {

struct Animation::Impl {
    // Must not be the first member: lv_anim_start() treats `var == &template`
    // as a self-reference and replaces var with LVGL's internal copy.
    std::uint32_t        self_tag = 0x414E494D; // "ANIM"
    lv_anim_t            anim{};
    AnimCallback         exec_cb;
    std::function<void()> start_cb;
    std::function<void()> completed_cb;
    std::function<void()> deleted_cb;
    widgets::Widget   *target = nullptr;
    core::ScopedConnection target_connection;
    bool                 started  = false;
    bool                 running  = false;
    bool                 finished = false;
    uint32_t             duration = 0;

    Impl()
    {
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, this);
        lv_anim_set_user_data(&anim, this);
        lv_anim_set_exec_cb(&anim, &Animation::execThunk);
    }
};

Animation::Animation()
    : impl_(std::make_unique<Impl>())
{}

Animation::~Animation()
{
    if (impl_)
    {
        // Prevent user callbacks from running while this object is being torn down.
        impl_->exec_cb      = nullptr;
        impl_->start_cb     = nullptr;
        impl_->completed_cb = nullptr;
        impl_->deleted_cb   = nullptr;
        auto *impl          = impl_.get();
        lv_anim_delete(impl, &Animation::execThunk);
    }
}

Animation::Animation(Animation &&other) noexcept = default;

Animation &Animation::operator=(Animation &&other) noexcept = default;

Animation &Animation::setTarget(widgets::Widget *target)
{
    if (impl_->target == target)
    {
        return *this;
    }

    impl_->target_connection = core::ScopedConnection();
    impl_->target            = target;

    if (target != nullptr)
    {
        Impl *impl = impl_.get();
        impl->target_connection = core::ScopedConnection(target->onDestroyed([impl]() {
            impl->target  = nullptr;
            impl->running = false;
            lv_anim_delete(impl, &Animation::execThunk);
        }));
    }
    return *this;
}

Animation &Animation::setDuration(uint32_t ms)
{
    impl_->duration = ms;
    lv_anim_set_duration(&impl_->anim, ms);
    return *this;
}

uint32_t Animation::duration() const
{
    return impl_->duration;
}

Animation &Animation::setValues(int32_t start, int32_t end)
{
    lv_anim_set_values(&impl_->anim, start, end);
    return *this;
}

Animation &Animation::setExecCallback(AnimCallback cb)
{
    impl_->exec_cb = std::move(cb);
    lv_anim_set_exec_cb(&impl_->anim, &Animation::execThunk);
    return *this;
}

Animation &Animation::setDelay(uint32_t ms)
{
    lv_anim_set_delay(&impl_->anim, ms);
    return *this;
}

Animation &Animation::setRepeatCount(uint32_t count)
{
    lv_anim_set_repeat_count(&impl_->anim, count);
    return *this;
}

Animation &Animation::setRepeatDelay(uint32_t ms)
{
    lv_anim_set_repeat_delay(&impl_->anim, ms);
    return *this;
}

Animation &Animation::setReverseDuration(uint32_t ms)
{
    lv_anim_set_reverse_duration(&impl_->anim, ms);
    return *this;
}

Animation &Animation::setReverseDelay(uint32_t ms)
{
    lv_anim_set_reverse_delay(&impl_->anim, ms);
    return *this;
}

Animation &Animation::setStartCallback(std::function<void()> cb)
{
    impl_->start_cb = std::move(cb);
    lv_anim_set_start_cb(&impl_->anim, &Animation::startThunk);
    return *this;
}

Animation &Animation::setCompleteCallback(std::function<void()> cb)
{
    impl_->completed_cb = std::move(cb);
    lv_anim_set_completed_cb(&impl_->anim, &Animation::completedThunk);
    return *this;
}

Animation &Animation::setDeletedCallback(std::function<void()> cb)
{
    impl_->deleted_cb = std::move(cb);
    lv_anim_set_deleted_cb(&impl_->anim, &Animation::deletedThunk);
    return *this;
}

Animation &Animation::setPathCallback(lv_anim_path_cb_t pathCb)
{
    lv_anim_set_path_cb(&impl_->anim, pathCb);
    return *this;
}

void Animation::start()
{
    if (impl_->target == nullptr || !impl_->exec_cb)
    {
        return;
    }

    // Restarting replaces any previous run of this Animation.
    lv_anim_delete(impl_.get(), &Animation::execThunk);

    lv_anim_set_var(&impl_->anim, impl_.get());
    lv_anim_set_user_data(&impl_->anim, impl_.get());
    lv_anim_set_exec_cb(&impl_->anim, &Animation::execThunk);

    impl_->started  = true;
    impl_->running  = true;
    impl_->finished = false;

    lv_anim_start(&impl_->anim);
}

void Animation::stop()
{
    lv_anim_delete(impl_.get(), &Animation::execThunk);
    impl_->running = false;
}

bool Animation::isRunning() const
{
    return impl_->running;
}

bool Animation::isFinished() const
{
    return impl_->finished;
}

lv_anim_t *Animation::handle()
{
    return &impl_->anim;
}

void Animation::execThunk(void *var, int32_t value)
{
    auto *impl = static_cast<Impl *>(var);
    if (impl == nullptr || impl->target == nullptr || !impl->exec_cb)
    {
        return;
    }
    impl->exec_cb(impl->target, value);
}

void Animation::startThunk(lv_anim_t *a)
{
    auto *impl = static_cast<Impl *>(lv_anim_get_user_data(a));
    if (impl != nullptr && impl->start_cb)
    {
        impl->start_cb();
    }
}

void Animation::completedThunk(lv_anim_t *a)
{
    auto *impl = static_cast<Impl *>(lv_anim_get_user_data(a));
    if (impl == nullptr)
    {
        return;
    }

    impl->running  = false;
    impl->finished = true;

    if (impl->completed_cb)
    {
        impl->completed_cb();
    }
}

void Animation::deletedThunk(lv_anim_t *a)
{
    auto *impl = static_cast<Impl *>(lv_anim_get_user_data(a));
    if (impl == nullptr)
    {
        return;
    }

    impl->running = false;
    if (impl->deleted_cb)
    {
        impl->deleted_cb();
    }
}

} // namespace lvglpp::sys
