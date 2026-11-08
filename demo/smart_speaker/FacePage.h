#ifndef LVGLPP_FACE_PAGE_H
#define LVGLPP_FACE_PAGE_H

#include "core/sys/Timer.h"
#include "core/widgets/Label.h"
#include "core/widgets/Widget.h"
#include "core/widgets/WidgetStore.h"
#include "FaceTypes.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>

namespace lvglpp::demo
{

/**
 * The living-face page. Can be hosted directly as one tile of an application's
 * own TileView, or composed by FacePanel.
 *
 * Call setActive(false) when the page is not visible; its 20 Hz animation timer
 * is paused in that case.
 */
class FacePage
{
public:
    using Action = std::function<void()>;

    explicit FacePage(widgets::Widget* parent, int diameter = 360);
    ~FacePage();

    FacePage(const FacePage&)            = delete;
    FacePage& operator=(const FacePage&) = delete;
    FacePage(FacePage&&)                 = delete;
    FacePage& operator=(FacePage&&)      = delete;

    widgets::Widget* widget() const { return root_.get(); }

    void setActive(bool active);
    void setVoiceState(VoiceState state);
    void setMicLevel(std::uint8_t level);
    void setTime(std::string_view hhmm);
    void setStatus(std::string_view status);

    /// Called when the PLAY button is tapped.
    void setOnPlay(Action action);

private:
    void build_face(widgets::Widget* face);
    void face_tick();
    void set_face_state(VoiceState state);
    void set_pupils(int dx, int dy);
    void pet();

    int diameter_ = 360;

    std::unique_ptr<widgets::Widget> root_;
    widgets::WidgetStore widgets_;

    widgets::Widget* home_ring_   = nullptr;
    widgets::Widget* face_        = nullptr;
    widgets::Widget* eye_left_    = nullptr;
    widgets::Widget* eye_right_   = nullptr;
    widgets::Widget* pupil_left_  = nullptr;
    widgets::Widget* pupil_right_ = nullptr;
    widgets::Widget* lid_left_    = nullptr;
    widgets::Widget* lid_right_   = nullptr;
    widgets::Widget* mouth_       = nullptr;
    widgets::Widget* play_btn_    = nullptr;
    widgets::Label* clock_        = nullptr;
    widgets::Label* status_       = nullptr;

    VoiceState state_         = VoiceState::Init;
    std::uint32_t face_color_ = 0x555555;
    std::uint8_t mic_level_   = 0;
    bool active_              = true;

    int face_tick_       = 0;
    int blink_countdown_ = 40;
    int blink_phase_     = -1;
    int gaze_countdown_  = 30;
    int pet_ticks_       = 0;

    Action on_play_;

    std::unique_ptr<sys::Timer> face_timer_;
};

}    // namespace lvglpp::demo

#endif    // LVGLPP_FACE_PAGE_H
