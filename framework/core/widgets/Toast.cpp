#include "Toast.h"

#include "ToastView.h"

namespace lvglpp::widgets {

namespace {
constexpr std::uint32_t kFadeInMs  = 200;
constexpr std::uint32_t kFadeOutMs = 200;
} // namespace

Toast *&Toast::instanceSlot()
{
    static Toast *instance = nullptr;
    return instance;
}

Toast *Toast::instanceIfCreated()
{
    return instanceSlot();
}

Toast &Toast::getInstance()
{
    Toast *&slot = instanceSlot();
    if (slot == nullptr)
    {
        slot = new Toast();
    }
    return *slot;
}

Toast::Toast()
{
    view_ = std::make_unique<ToastView>();
    view_->setVisible(false);
    overlay_token_ = OverlayManager::getInstance().registerTransient(view_.get(), OverlayManager::Priority::Toast);
}

Toast::~Toast()
{
    stopAnimations();
    if (overlay_token_)
    {
        OverlayManager::getInstance().unregisterTransient(overlay_token_);
    }
}

void Toast::show(const std::string &message, ToastType type, std::uint32_t durationMs)
{
    getInstance().showImpl(message, type, durationMs);
}

void Toast::success(const std::string &message)
{
    show(message, ToastType::Success);
}

void Toast::warn(const std::string &message)
{
    show(message, ToastType::Warning);
}

void Toast::error(const std::string &message)
{
    show(message, ToastType::Error);
}

void Toast::cancel()
{
    if (Toast *instance = instanceIfCreated())
    {
        instance->hideNow();
    }
}

void Toast::shutdown()
{
    Toast *&slot = instanceSlot();
    delete slot;
    slot = nullptr;
}

void Toast::showImpl(const std::string &message, ToastType type, std::uint32_t durationMs)
{
    if (message.empty())
    {
        return;
    }
    if (durationMs == 0)
    {
        durationMs = kDefaultDurationMs;
    }

    stopAnimations();
    current_duration_ms_ = durationMs;

    view_->setContent(type, message);
    view_->setVisible(true);
    view_->setOpacity(1.0);
    OverlayManager::getInstance().bringToFront(overlay_token_);

    if (state_ == State::Idle)
    {
        view_->setOpacity(0);
        state_ = State::Showing;
        startShowAnimation();
        return;
    }

    // Already on screen (or fading out): update in place and restart the
    // display timer instead of queueing a second toast.
    state_ = State::Visible;
    scheduleHide(current_duration_ms_);
}

void Toast::startShowAnimation()
{
    lv_anim_init(&show_anim_);
    lv_anim_set_var(&show_anim_, this);
    lv_anim_set_exec_cb(&show_anim_, [](void *target, int32_t value) {
        static_cast<Toast *>(target)->view_->setOpacity(value / 100.0);
    });
    lv_anim_set_values(&show_anim_, 0, 100);
    lv_anim_set_duration(&show_anim_, kFadeInMs);
    lv_anim_set_path_cb(&show_anim_, lv_anim_path_ease_out);
    lv_anim_set_completed_cb(&show_anim_, [](lv_anim_t *anim) {
        auto *self = static_cast<Toast *>(anim->var);
        if (self->state_ != State::Showing)
        {
            return;
        }
        self->state_ = State::Visible;
        self->scheduleHide(self->current_duration_ms_);
    });
    lv_anim_start(&show_anim_);
}

void Toast::scheduleHide(std::uint32_t durationMs)
{
    if (hide_timer_ != nullptr)
    {
        lv_timer_delete(hide_timer_);
        hide_timer_ = nullptr;
    }

    hide_timer_ = lv_timer_create(
        [](lv_timer_t *timer) {
            auto *self = static_cast<Toast *>(lv_timer_get_user_data(timer));
            if (self == nullptr)
            {
                return;
            }
            self->hide_timer_ = nullptr; // one-shot timer deletes itself
            self->startHideAnimation();
        },
        durationMs, this);
    lv_timer_set_repeat_count(hide_timer_, 1);
}

void Toast::startHideAnimation()
{
    if (state_ == State::Idle)
    {
        return;
    }

    state_ = State::Hiding;
    lv_anim_init(&hide_anim_);
    lv_anim_set_var(&hide_anim_, this);
    lv_anim_set_exec_cb(&hide_anim_, [](void *target, int32_t value) {
        static_cast<Toast *>(target)->view_->setOpacity((100 - value) / 100.0);
    });
    lv_anim_set_values(&hide_anim_, 0, 100);
    lv_anim_set_duration(&hide_anim_, kFadeOutMs);
    lv_anim_set_path_cb(&hide_anim_, lv_anim_path_ease_out);
    lv_anim_set_completed_cb(&hide_anim_, [](lv_anim_t *anim) {
        auto *self = static_cast<Toast *>(anim->var);
        if (self->state_ != State::Hiding)
        {
            return;
        }
        self->state_ = State::Idle;
        self->view_->setVisible(false);
    });
    lv_anim_start(&hide_anim_);
}

void Toast::hideNow()
{
    stopAnimations();
    state_ = State::Idle;
    if (view_)
    {
        view_->setVisible(false);
        view_->setOpacity(0.0);
    }
}

void Toast::stopAnimations()
{
    lv_anim_delete(this, nullptr);
    if (hide_timer_ != nullptr)
    {
        lv_timer_delete(hide_timer_);
        hide_timer_ = nullptr;
    }
}

} // namespace lvglpp::widgets
