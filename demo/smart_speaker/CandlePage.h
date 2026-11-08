#ifndef LVGLPP_CANDLE_PAGE_H
#define LVGLPP_CANDLE_PAGE_H

#include "core/sys/Timer.h"
#include "core/widgets/Label.h"
#include "core/widgets/Widget.h"
#include "core/widgets/WidgetStore.h"
#include "FaceTypes.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>

namespace lvglpp::demo
{

/**
 * The blow-out-the-candles page. Host it as one tile of an application's own
 * TileView and call setActive() when the tile becomes visible.
 */
class CandlePage
{
public:
    using Action = std::function<void()>;

    explicit CandlePage(widgets::Widget* parent, int diameter = 360);
    ~CandlePage();

    CandlePage(const CandlePage&)            = delete;
    CandlePage& operator=(const CandlePage&) = delete;
    CandlePage(CandlePage&&)                 = delete;
    CandlePage& operator=(CandlePage&&)      = delete;

    widgets::Widget* widget() const { return root_.get(); }

    /// Pause the flame/confetti timer while the page is hidden.
    void setActive(bool active);

    /// Used to gate breath detection (only Idle/Listening).
    void setVoiceState(VoiceState state);
    void setMicLevel(std::uint8_t level);

    /// Called when the back button is tapped.
    void setOnBack(Action action);

private:
    struct Candle
    {
        widgets::Widget* flame = nullptr;
        widgets::Widget* inner = nullptr;
        widgets::Widget* smoke = nullptr;
        int x                  = 0;
        int flame_y            = 0;
        int smoke_y            = 0;
        int smoke_life         = 0;
        bool lit               = true;
    };

    void build_back_btn();
    void candle_tick();
    void blow_once();
    void reset_candles();
    void start_celebration();
    void spawn_confetti();
    void move_confetti();

    int diameter_ = 360;

    std::unique_ptr<widgets::Widget> root_;
    widgets::WidgetStore widgets_;

    std::array<Candle, 3> candles_             = {};
    widgets::Widget* blow_btn_                 = nullptr;
    widgets::Label* blow_label_                = nullptr;
    widgets::Label* candle_title_              = nullptr;
    widgets::Label* candle_hint_               = nullptr;
    std::array<widgets::Widget*, 16> confetti_ = {};
    std::array<int, 16> confetti_x_            = {};
    std::array<float, 16> confetti_y_          = {};

    VoiceState state_       = VoiceState::Init;
    std::uint8_t mic_level_ = 0;
    bool active_            = false;
    bool blowing_           = false;
    bool celebrating_       = false;
    int candle_tick_        = 0;
    int blow_ticks_         = 0;

    Action on_back_;

    std::unique_ptr<sys::Timer> candle_timer_;
};

}    // namespace lvglpp::demo

#endif    // LVGLPP_CANDLE_PAGE_H
