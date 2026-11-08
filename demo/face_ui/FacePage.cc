#include "FacePage.h"

#include <cmath>
#include <cstdlib>
#include <iterator>
#include <string>

namespace lvglpp::demo
{
namespace
{

constexpr std::uint32_t kColorWhite = 0xFFFFFF;
constexpr std::uint32_t kColorCard  = 0x2A2A3A;

constexpr std::uint32_t kStateColors[] = {
    0x555555, /* Init */
    0xE8D8B0, /* Idle */
    0x00E5FF, /* Listening */
    0xFFC857, /* Thinking */
    0x7CFF9B, /* Speaking */
    0xFF4D5E, /* Error */
};

}    // namespace

FacePage::FacePage(widgets::Widget* parent, int diameter) : diameter_(diameter)
{
    root_ = std::make_unique<widgets::Widget>(parent);
    root_->setSize(diameter_, diameter_);
    root_->setAlignment(LV_ALIGN_CENTER, 0, 0);
    root_->setBgOpacity(LV_OPA_TRANSP);
    /* The page container must not swallow tileview swipes; its children
     * (PLAY, face) keep their own click handling. */
    lv_obj_clear_flag(root_->lvObj(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(root_->lvObj(), LV_OBJ_FLAG_SCROLLABLE);

    face_ = widgets_.emplace<widgets::Widget>(root_.get());
    face_->setSize(332, 332);
    face_->setRadius(LV_RADIUS_CIRCLE);
    face_->setBgColor(0x0B1420);
    face_->setBorder(1, 0x1E2A3A);
    face_->setAlignment(LV_ALIGN_CENTER, 0, 0);
    face_->onClicked([this]() {
        pet();
    });
    lv_obj_clear_flag(face_->lvObj(), LV_OBJ_FLAG_SCROLLABLE);

    home_ring_ = widgets_.emplace<widgets::Widget>(root_.get());
    home_ring_->setSize(344, 344);
    home_ring_->setAlignment(LV_ALIGN_CENTER, 0, 0);
    home_ring_->setRadius(LV_RADIUS_CIRCLE);
    home_ring_->setBgOpacity(LV_OPA_TRANSP);
    home_ring_->setBorder(4, kColorCard);
    lv_obj_clear_flag(home_ring_->lvObj(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(home_ring_->lvObj(), LV_OBJ_FLAG_SCROLLABLE);

    build_face(face_);

    status_ = widgets_.emplace<widgets::Label>("lvglpp 360x360", 0x00FF88, root_.get(), widgets::Label::FontSize16);
    status_->setTextAlign(LV_TEXT_ALIGN_CENTER);
    status_->setAlignment(LV_ALIGN_TOP_MID, 0, 30);

    clock_ = widgets_.emplace<widgets::Label>("12:34", kColorWhite, root_.get(), widgets::Label::FontSize20);
    clock_->setAlignment(LV_ALIGN_BOTTOM_MID, 0, -46);

    play_btn_ = widgets_.emplace<widgets::Widget>(root_.get());
    play_btn_->setSize(96, 26);
    play_btn_->setRadius(13);
    play_btn_->setBgColor(0x1E2A3A);
    play_btn_->setBorder(1, 0x00AAFF);
    play_btn_->setAlignment(LV_ALIGN_BOTTOM_MID, 0, -12);
    play_btn_->onClicked([this]() {
        if (on_play_)
        {
            on_play_();
        }
    });

    auto* play_label = widgets_.emplace<widgets::Label>("PLAY", kColorWhite, play_btn_, widgets::Label::FontSize16);
    play_label->setAlignment(LV_ALIGN_CENTER, 0, 0);

    set_face_state(VoiceState::Init);

    face_timer_ = sys::Timer::create(
        [this](sys::Timer*) {
            face_tick();
        },
        50);
    if (!active_)
    {
        face_timer_->pause();
    }
}

FacePage::~FacePage() = default;

void FacePage::setActive(bool active)
{
    if (active_ == active)
    {
        return;
    }
    active_ = active;
    if (face_timer_ == nullptr)
    {
        return;
    }
    if (active_)
    {
        set_face_state(state_);
        face_timer_->resume();
    }
    else
    {
        face_timer_->pause();
    }
}

void FacePage::setVoiceState(VoiceState state)
{
    state_           = state;
    const auto index = static_cast<std::size_t>(state);
    if (index < std::size(kStateColors))
    {
        face_color_ = kStateColors[index];
    }
    set_face_state(state);
}

void FacePage::setMicLevel(std::uint8_t level)
{
    mic_level_ = level;
}

void FacePage::setTime(std::string_view hhmm)
{
    if (clock_ != nullptr)
    {
        clock_->setText(std::string(hhmm));
    }
}

void FacePage::setStatus(std::string_view status)
{
    if (status_ != nullptr)
    {
        status_->setText(std::string(status));
    }
}

void FacePage::setOnPlay(Action action)
{
    on_play_ = std::move(action);
}

void FacePage::build_face(widgets::Widget* face)
{
    constexpr int kEyeSize = 54;

    auto make_eye = [&](int offset_x) -> widgets::Widget* {
        auto* eye = widgets_.emplace<widgets::Widget>(face);
        eye->setSize(kEyeSize, kEyeSize);
        eye->setRadius(LV_RADIUS_CIRCLE);
        eye->setBgColor(0xE8F7FF);
        eye->setBorder(0, 0xE8F7FF);
        eye->setAlignment(LV_ALIGN_CENTER, offset_x, -32);
        return eye;
    };

    auto make_pupil = [&](widgets::Widget* eye) -> widgets::Widget* {
        auto* pupil = widgets_.emplace<widgets::Widget>(eye);
        pupil->setSize(20, 20);
        pupil->setRadius(LV_RADIUS_CIRCLE);
        pupil->setBgColor(0x0A1420);
        pupil->setBorder(0, 0x0A1420);
        pupil->setPos(17, 17);

        auto* glint = widgets_.emplace<widgets::Widget>(pupil);
        glint->setSize(6, 6);
        glint->setRadius(LV_RADIUS_CIRCLE);
        glint->setBgColor(kColorWhite);
        glint->setBorder(0, kColorWhite);
        glint->setPos(12, 4);
        return pupil;
    };

    auto make_lid = [&](widgets::Widget* eye) -> widgets::Widget* {
        auto* lid = widgets_.emplace<widgets::Widget>(eye);
        lid->setSize(kEyeSize, 0);
        lid->setRadius(0);
        lid->setBgColor(0x0B1420);
        lid->setBorder(0, 0x0B1420);
        lid->setPos(0, 0);
        return lid;
    };

    eye_left_    = make_eye(-64);
    eye_right_   = make_eye(64);
    pupil_left_  = make_pupil(eye_left_);
    pupil_right_ = make_pupil(eye_right_);
    lid_left_    = make_lid(eye_left_);
    lid_right_   = make_lid(eye_right_);

    mouth_ = widgets_.emplace<widgets::Widget>(face);
    mouth_->setSize(56, 10);
    mouth_->setRadius(5);
    mouth_->setBgColor(face_color_);
    mouth_->setBorder(0, face_color_);
    mouth_->setAlignment(LV_ALIGN_CENTER, 0, 64);

    for (int side : {-1, 1})
    {
        auto* cheek = widgets_.emplace<widgets::Widget>(face);
        cheek->setSize(22, 22);
        cheek->setRadius(LV_RADIUS_CIRCLE);
        cheek->setBgColor(0xFF7EB6);
        cheek->setBorder(0, 0xFF7EB6);
        cheek->setOpacity(0.18);
        cheek->setAlignment(LV_ALIGN_CENTER, side * 92, 34);
    }
}

void FacePage::set_pupils(int dx, int dy)
{
    if (pupil_left_ == nullptr || pupil_right_ == nullptr)
    {
        return;
    }
    pupil_left_->setPos(17 + dx, 17 + dy);
    pupil_right_->setPos(17 + dx, 17 + dy);
}

void FacePage::set_face_state(VoiceState state)
{
    state_       = state;
    blink_phase_ = -1;
    pet_ticks_   = 0;

    if (face_ == nullptr || mouth_ == nullptr)
    {
        return;
    }

    int lid_h   = 0;
    int mouth_w = 56;
    int mouth_h = 10;
    switch (state)
    {
    case VoiceState::Init:
        lid_h   = 54;
        mouth_w = 24;
        mouth_h = 6;
        break;
    case VoiceState::Error:
        lid_h   = 27;
        mouth_w = 38;
        mouth_h = 8;
        break;
    case VoiceState::Thinking:
        mouth_w = 26;
        mouth_h = 8;
        break;
    case VoiceState::Listening:
        mouth_w = 30;
        mouth_h = 8;
        break;
    case VoiceState::Speaking:
    case VoiceState::Idle:
    default:
        break;
    }

    lid_left_->setSize(54, lid_h);
    lid_right_->setSize(54, lid_h);
    mouth_->setSize(mouth_w, mouth_h);
    const std::uint32_t mouth_color = state == VoiceState::Error ? 0xFF4D5E : face_color_;
    mouth_->setBgColor(mouth_color);
    mouth_->setBorder(0, mouth_color);
    set_pupils(0, state == VoiceState::Thinking ? -6 : 0);

    if (home_ring_ != nullptr)
    {
        home_ring_->setBorder(4, face_color_);
    }
}

void FacePage::pet()
{
    if (state_ == VoiceState::Init || state_ == VoiceState::Error)
    {
        return;
    }
    pet_ticks_   = 8;
    blink_phase_ = -1;
    lid_left_->setSize(54, 18);
    lid_right_->setSize(54, 18);
}

void FacePage::face_tick()
{
    if (face_ == nullptr || !active_)
    {
        return;
    }
    ++face_tick_;

    if (pet_ticks_ > 0)
    {
        if (--pet_ticks_ == 0)
        {
            lid_left_->setSize(54, 0);
            lid_right_->setSize(54, 0);
        }
    }
    else if (blink_phase_ >= 0)
    {
        static constexpr int kLidHeights[5] = {0, 27, 54, 27, 0};
        const int h                         = kLidHeights[blink_phase_];
        lid_left_->setSize(54, h);
        lid_right_->setSize(54, h);
        if (++blink_phase_ >= 5)
        {
            blink_phase_     = -1;
            blink_countdown_ = 50 + (std::rand() % 50);
        }
    }
    else if (state_ == VoiceState::Idle)
    {
        if (--blink_countdown_ <= 0)
        {
            blink_phase_ = 0;
        }
    }

    if (state_ == VoiceState::Idle)
    {
        if (--gaze_countdown_ <= 0)
        {
            set_pupils((std::rand() % 13) - 6, (std::rand() % 7) - 3);
            gaze_countdown_ = 30 + (std::rand() % 40);
        }
    }
    else if (state_ == VoiceState::Thinking)
    {
        set_pupils(0, -6);
    }
    else
    {
        set_pupils(0, 0);
    }

    int ring_w = 4;
    if (state_ == VoiceState::Listening)
    {
        ring_w = 4 + static_cast<int>(mic_level_) / 25;
    }
    else if (state_ == VoiceState::Speaking)
    {
        ring_w = 4 + static_cast<int>(2.5 * (1.0 + std::sin(face_tick_ * 0.55)));
    }
    if (home_ring_ != nullptr)
    {
        home_ring_->setBorder(ring_w, face_color_);
    }

    if (state_ == VoiceState::Speaking)
    {
        const int h = 8 + static_cast<int>(8.0 * (1.0 + std::sin(face_tick_ * 0.8)) / 2.0);
        mouth_->setSize(56, h);
    }
}

}    // namespace lvglpp::demo
