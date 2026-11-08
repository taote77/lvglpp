#ifndef LVGLPP_FACE_DEMO_ACTIVITY_H
#define LVGLPP_FACE_DEMO_ACTIVITY_H

#include "core/sys/BaseActivity.h"
#include "core/sys/Timer.h"
#include "core/widgets/Label.h"
#include "FacePanel.h"

#include <memory>

namespace lvglpp::demo
{

/**
 * Desktop demo host for FacePanel. Cycles the voice state every 3 seconds and
 * simulates a microphone level while Listening so the ring, eyes and mouth
 * animate without a real audio pipeline.
 *
 * On a real device, host FacePanel directly and call setVoiceState() /
 * setMicLevel() from the voice pipeline instead of using this activity.
 */
class FaceDemoActivity final : public sys::BaseActivity
{
public:
    FaceDemoActivity()           = default;
    ~FaceDemoActivity() override = default;

protected:
    void onCreate(std::any arg) override;
    void onDestroy() override;

private:
    void applyState(VoiceState state);
    void updateClock();

    std::unique_ptr<FacePanel> face_;
    std::unique_ptr<sys::Timer> state_timer_;
    std::unique_ptr<sys::Timer> clock_timer_;
    std::unique_ptr<sys::Timer> level_timer_;

    std::unique_ptr<widgets::Label> title_;
    std::unique_ptr<widgets::Label> state_label_;
    std::unique_ptr<widgets::Label> hint_;

    VoiceState state_     = VoiceState::Idle;
    unsigned state_index_ = 0;
    unsigned level_tick_  = 0;
};

}    // namespace lvglpp::demo

#endif    // LVGLPP_FACE_DEMO_ACTIVITY_H
