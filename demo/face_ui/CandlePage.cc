#include "CandlePage.h"

#include <cstdlib>

namespace lvglpp::demo
{
namespace
{

constexpr std::uint32_t kColorAccent = 0x00AAFF;
constexpr std::uint32_t kColorWhite  = 0xFFFFFF;
constexpr std::uint32_t kColorCard   = 0x2A2A3A;

/* Mic level (0..100) that counts as a sustained blow. The smart-speaker
 * firmware measured a quiet room at max=20 / avg=8; 55 leaves headroom for
 * ambient noise while a direct breath sits well above it. */
constexpr std::uint8_t kBlowLevel = 55;

}    // namespace

CandlePage::CandlePage(widgets::Widget* parent, int diameter) : diameter_(diameter)
{
    root_ = std::make_unique<widgets::Widget>(parent);
    root_->setSize(diameter_, diameter_);
    root_->setAlignment(LV_ALIGN_CENTER, 0, 0);
    root_->setBgOpacity(LV_OPA_TRANSP);
    /* Let horizontal swipes reach the host tileview. */
    lv_obj_clear_flag(root_->lvObj(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(root_->lvObj(), LV_OBJ_FLAG_SCROLLABLE);

    build_back_btn();

    candle_title_ = widgets_.emplace<widgets::Label>("BIRTHDAY", kColorWhite, root_.get(), widgets::Label::FontSize24);
    candle_title_->setTextAlign(LV_TEXT_ALIGN_CENTER);
    candle_title_->setAlignment(LV_ALIGN_TOP_MID, 0, 68);

    candle_hint_ = widgets_.emplace<widgets::Label>("BLOW AT THE MIC OR HOLD", 0x9AA5B1, root_.get(),
                                                    widgets::Label::FontSize16);
    candle_hint_->setTextAlign(LV_TEXT_ALIGN_CENTER);
    candle_hint_->setAlignment(LV_ALIGN_BOTTOM_MID, 0, -72);

    for (int i = 0; i < static_cast<int>(candles_.size()); i++)
    {
        Candle& candle = candles_[i];
        candle.x       = 96 + i * 84;

        auto* body = widgets_.emplace<widgets::Widget>(root_.get());
        body->setSize(26, 80);
        body->setPos(candle.x, 170);
        body->setRadius(8);
        body->setBgColor(0xE8D8B0);
        body->setBorder(0, 0xE8D8B0);

        auto* wick = widgets_.emplace<widgets::Widget>(root_.get());
        wick->setSize(4, 10);
        wick->setPos(candle.x + 11, 160);
        wick->setRadius(2);
        wick->setBgColor(0x5A4A3A);
        wick->setBorder(0, 0x5A4A3A);

        candle.flame_y = 126;
        candle.flame   = widgets_.emplace<widgets::Widget>(root_.get());
        candle.flame->setSize(24, 34);
        candle.flame->setPos(candle.x + 1, candle.flame_y);
        candle.flame->setRadius(LV_RADIUS_CIRCLE);
        candle.flame->setBgColor(0xFF8A3D);
        candle.flame->setBorder(0, 0xFF8A3D);

        candle.inner = widgets_.emplace<widgets::Widget>(root_.get());
        candle.inner->setSize(10, 18);
        candle.inner->setPos(candle.x + 8, candle.flame_y + 10);
        candle.inner->setRadius(LV_RADIUS_CIRCLE);
        candle.inner->setBgColor(0xFFD166);
        candle.inner->setBorder(0, 0xFFD166);

        candle.smoke = widgets_.emplace<widgets::Widget>(root_.get());
        candle.smoke->setSize(20, 20);
        candle.smoke->setPos(candle.x + 3, 118);
        candle.smoke->setRadius(LV_RADIUS_CIRCLE);
        candle.smoke->setBgColor(0x9AA5B1);
        candle.smoke->setBorder(0, 0x9AA5B1);
        candle.smoke->setVisible(false);
    }

    blow_btn_ = widgets_.emplace<widgets::Widget>(root_.get());
    blow_btn_->setSize(180, 52);
    blow_btn_->setRadius(26);
    blow_btn_->setBgColor(0x1E2A3A);
    blow_btn_->setBorder(1, kColorAccent);
    blow_btn_->setAlignment(LV_ALIGN_BOTTOM_MID, 0, -18);
    blow_btn_->onPressed([this]() {
        if (!celebrating_)
        {
            blowing_ = true;
        }
    });
    blow_btn_->onReleased([this]() {
        if (celebrating_)
        {
            reset_candles();
        }
        blowing_ = false;
    });

    blow_label_ = widgets_.emplace<widgets::Label>("BLOW / HOLD", kColorWhite, blow_btn_, widgets::Label::FontSize16);
    blow_label_->setAlignment(LV_ALIGN_CENTER, 0, 0);

    static constexpr std::uint32_t kConfettiColors[6] = {
        0xFF6B6B, 0xFFD166, 0x6BE6A5, 0x6BCBFF, 0xC79BFF, 0xFF9BD1,
    };
    for (int i = 0; i < static_cast<int>(confetti_.size()); i++)
    {
        auto* piece = widgets_.emplace<widgets::Widget>(root_.get());
        piece->setSize(8, 8);
        piece->setRadius(LV_RADIUS_CIRCLE);
        piece->setBgColor(kConfettiColors[i % 6]);
        piece->setBorder(0, kConfettiColors[i % 6]);
        piece->setVisible(false);
        confetti_[i] = piece;
    }

    candle_timer_ = sys::Timer::create(
        [this](sys::Timer*) {
            candle_tick();
        },
        50);
    if (!active_)
    {
        candle_timer_->pause();
    }
}

CandlePage::~CandlePage() = default;

void CandlePage::setActive(bool active)
{
    if (active_ == active)
    {
        return;
    }
    active_ = active;
    if (candle_timer_ == nullptr)
    {
        return;
    }
    blowing_    = false;
    blow_ticks_ = 0;
    if (active_)
    {
        candle_timer_->resume();
    }
    else
    {
        candle_timer_->pause();
    }
}

void CandlePage::setVoiceState(VoiceState state)
{
    state_ = state;
}

void CandlePage::setMicLevel(std::uint8_t level)
{
    mic_level_ = level;
}

void CandlePage::setOnBack(Action action)
{
    on_back_ = std::move(action);
}

void CandlePage::build_back_btn()
{
    auto* button = widgets_.emplace<widgets::Widget>(root_.get());
    button->setSize(48, 48);
    button->setRadius(LV_RADIUS_CIRCLE);
    button->setBgColor(kColorCard);
    button->setAlignment(LV_ALIGN_TOP_MID, 0, 14);
    button->onClicked([this]() {
        if (on_back_)
        {
            on_back_();
        }
    });

    auto* label = widgets_.emplace<widgets::Label>("<", kColorWhite, button, widgets::Label::FontSize20);
    label->setAlignment(LV_ALIGN_CENTER, 0, 0);
}

void CandlePage::blow_once()
{
    for (Candle& candle : candles_)
    {
        if (!candle.lit)
        {
            continue;
        }
        candle.lit = false;
        candle.flame->setVisible(false);
        candle.inner->setVisible(false);
        candle.smoke->setVisible(true);
        candle.smoke->setOpacity(0.9);
        candle.smoke_y = 126;
        candle.smoke->setPos(candle.x + 3, candle.smoke_y);
        candle.smoke_life = 20;
        break;
    }

    bool any_lit = false;
    for (const Candle& candle : candles_)
    {
        any_lit = any_lit || candle.lit;
    }
    if (!any_lit)
    {
        start_celebration();
    }
}

void CandlePage::start_celebration()
{
    celebrating_ = true;
    blowing_     = false;
    blow_ticks_  = 0;
    candle_title_->setText("HAPPY BIRTHDAY!");
    candle_hint_->setText("TAP AGAIN TO REPLAY");
    blow_label_->setText("AGAIN");
    spawn_confetti();
}

void CandlePage::reset_candles()
{
    for (Candle& candle : candles_)
    {
        candle.lit = true;
        candle.flame->setVisible(true);
        candle.inner->setVisible(true);
        candle.smoke->setVisible(false);
        candle.smoke_life = 0;
        candle.flame->setSize(24, 34);
        candle.inner->setSize(10, 18);
        candle.flame->setPos(candle.x + 1, candle.flame_y);
        candle.inner->setPos(candle.x + 8, candle.flame_y + 10);
    }
    for (widgets::Widget* piece : confetti_)
    {
        if (piece != nullptr)
        {
            piece->setVisible(false);
        }
    }
    celebrating_ = false;
    blowing_     = false;
    blow_ticks_  = 0;
    candle_title_->setText("BIRTHDAY");
    candle_hint_->setText("BLOW AT THE MIC OR HOLD");
    blow_label_->setText("BLOW / HOLD");
}

void CandlePage::spawn_confetti()
{
    for (std::size_t i = 0; i < confetti_.size(); i++)
    {
        if (confetti_[i] == nullptr)
        {
            continue;
        }
        confetti_x_[i] = 20 + (std::rand() % 320);
        confetti_y_[i] = -10.0f - static_cast<float>(std::rand() % 80);
        confetti_[i]->setPos(confetti_x_[i], static_cast<int>(confetti_y_[i]));
        confetti_[i]->setVisible(true);
    }
}

void CandlePage::move_confetti()
{
    for (std::size_t i = 0; i < confetti_.size(); i++)
    {
        if (confetti_[i] == nullptr)
        {
            continue;
        }
        confetti_y_[i] += 3.0f + static_cast<float>(i % 3);
        if (confetti_y_[i] > 352.0f)
        {
            confetti_y_[i] = -10.0f - static_cast<float>(std::rand() % 40);
            confetti_x_[i] = 20 + (std::rand() % 320);
        }
        confetti_[i]->setPos(confetti_x_[i], static_cast<int>(confetti_y_[i]));
    }
}

void CandlePage::candle_tick()
{
    if (candles_[0].flame == nullptr)
    {
        return;
    }
    ++candle_tick_;

    for (Candle& candle : candles_)
    {
        if (candle.lit)
        {
            const int h = 30 + ((candle_tick_ + candle.x) % 7);
            candle.flame->setSize(24, h);
            candle.inner->setSize(10, h / 2);
            candle.flame->setPos(candle.x + 1, candle.flame_y + (34 - h) / 2);
            candle.inner->setPos(candle.x + 8, candle.flame_y + (34 - h) / 2 + h / 3);
        }
        else if (candle.smoke_life > 0)
        {
            --candle.smoke_life;
            candle.smoke_y -= 1;
            candle.smoke->setPos(candle.x + 3, candle.smoke_y);
            candle.smoke->setOpacity(candle.smoke_life / 20.0);
            if (candle.smoke_life == 0)
            {
                candle.smoke->setVisible(false);
            }
        }
    }

    if (celebrating_)
    {
        move_confetti();
        return;
    }

    const bool breath = (state_ == VoiceState::Idle || state_ == VoiceState::Listening) && mic_level_ >= kBlowLevel;
    const bool active = blowing_ || breath;
    if (blow_btn_ != nullptr)
    {
        blow_btn_->setBorder(active ? 2 : 1, active ? 0x00E5FF : kColorAccent);
    }

    if (active)
    {
        if (++blow_ticks_ >= 12)
        {
            blow_ticks_ = 0;
            blow_once();
        }
    }
    else
    {
        blow_ticks_ = 0;
    }
}

}    // namespace lvglpp::demo
