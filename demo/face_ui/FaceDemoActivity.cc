#include "FaceDemoActivity.h"

#include <ctime>
#include <string>

namespace lvglpp::demo
{
namespace
{

constexpr std::uint32_t kColorBg   = 0x101820;
constexpr std::uint32_t kColorHint = 0x9AA5B1;

const char* stateName(VoiceState state)
{
    static constexpr const char* kNames[] = {
        "Init", "Idle", "Listening", "Thinking", "Speaking", "Error",
    };
    const auto index = static_cast<std::size_t>(state);
    return index < sizeof(kNames) / sizeof(kNames[0]) ? kNames[index] : "Unknown";
}

}    // namespace

void FaceDemoActivity::onCreate(std::any arg)
{
    sys::BaseActivity::onCreate(arg);

    widgets::Widget* root = getRoot();
    root->setBgColor(kColorBg);
    root->setBgOpacity(LV_OPA_COVER);

    face_ = std::make_unique<FacePanel>(root);
    face_->setStatus("lvglpp 360x360");

    /* On a 360x360 panel the face fills the screen. On a desktop window there
     * is room for a title and a short hint around the round panel. */
    const int width = lv_display_get_horizontal_resolution(lv_display_get_default());
    if (width > 400)
    {
        title_
            = std::make_unique<widgets::Label>("lvglpp 360x360 face demo", 0xFFFFFF, root, widgets::Label::FontSize24);
        title_->setAlignment(LV_ALIGN_TOP_MID, 0, 16);

        state_label_
            = std::make_unique<widgets::Label>("State: Idle (auto)", 0x00E5FF, root, widgets::Label::FontSize16);
        state_label_->setAlignment(LV_ALIGN_BOTTOM_MID, 0, -40);

        hint_ = std::make_unique<widgets::Label>("Tap the face or PLAY; swipe/back to navigate. "
                                                 "Mic level is simulated.",
                                                 kColorHint, root, widgets::Label::FontSize16);
        hint_->setTextAlign(LV_TEXT_ALIGN_CENTER);
        hint_->setAlignment(LV_ALIGN_BOTTOM_MID, 0, -12);
    }

    applyState(VoiceState::Idle);
    updateClock();

    static constexpr VoiceState kStates[] = {
        VoiceState::Idle, VoiceState::Listening, VoiceState::Thinking, VoiceState::Speaking, VoiceState::Error,
    };

    state_timer_ = sys::Timer::create(
        [this](sys::Timer*) {
            state_ = kStates[state_index_ % (sizeof(kStates) / sizeof(kStates[0]))];
            ++state_index_;
            applyState(state_);
        },
        3000);

    clock_timer_ = sys::Timer::create(
        [this](sys::Timer*) {
            updateClock();
        },
        1000);

    level_timer_ = sys::Timer::create(
        [this](sys::Timer*) {
            ++level_tick_;
            if (face_ == nullptr)
            {
                return;
            }
            if (state_ == VoiceState::Listening && !face_->isCandlePageActive())
            {
                face_->setMicLevel(static_cast<std::uint8_t>(55 + (level_tick_ * 7) % 40));
            }
            else
            {
                face_->setMicLevel(0);
            }
        },
        100);
}

void FaceDemoActivity::onDestroy()
{
    state_timer_.reset();
    clock_timer_.reset();
    level_timer_.reset();

    title_.reset();
    state_label_.reset();
    hint_.reset();
    face_.reset();

    sys::BaseActivity::onDestroy();
}

void FaceDemoActivity::applyState(VoiceState state)
{
    state_ = state;
    if (face_ != nullptr)
    {
        face_->setVoiceState(state);
    }
    if (state_label_ != nullptr)
    {
        state_label_->setText(std::string("State: ") + stateName(state) + " (auto)");
    }
}

void FaceDemoActivity::updateClock()
{
    if (face_ == nullptr)
    {
        return;
    }
    const std::time_t now = std::time(nullptr);
    const std::tm* local  = std::localtime(&now);
    if (local == nullptr)
    {
        return;
    }
    char hhmm[8] = {};
    if (std::strftime(hhmm, sizeof(hhmm), "%H:%M", local) > 0)
    {
        face_->setTime(hhmm);
    }
}

}    // namespace lvglpp::demo
