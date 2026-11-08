#include "FacePanel.h"

namespace lvglpp::demo
{
namespace
{

constexpr std::uint32_t kColorBg     = 0x101820;
constexpr std::uint32_t kColorAccent = 0x00AAFF;

}    // namespace

FacePanel::FacePanel(widgets::Widget* parent, int diameter) : diameter_(diameter)
{
    root_ = std::make_unique<widgets::Widget>(parent);
    root_->setSize(diameter_, diameter_);
    root_->setAlignment(LV_ALIGN_CENTER, 0, 0);
    root_->setBgColor(kColorBg);
    root_->setBgOpacity(LV_OPA_COVER);
    root_->setBorder(0, kColorBg);

    tileview_ = std::make_unique<widgets::TileView>(root_.get());
    tileview_->setSize(diameter_, diameter_);
    tileview_->setAlignment(LV_ALIGN_CENTER, 0, 0);
    tileview_->setBgColor(kColorBg);
    tileview_->setBgOpacity(LV_OPA_COVER);

    const lv_dir_t both = static_cast<lv_dir_t>(LV_DIR_LEFT | LV_DIR_RIGHT);
    auto add_tile       = [&](std::uint32_t col) -> widgets::Widget* {
        auto tile = tileview_->addTile(col, 0, both);
        if (!tile)
        {
            return nullptr;
        }
        widgets::Widget* raw = tile.get();
        widgets_.add(std::move(tile));
        raw->setBgColor(kColorBg);
        raw->setBgOpacity(LV_OPA_COVER);
        raw->setBorder(0, kColorBg);
        return raw;
    };

    if (widgets::Widget* home = add_tile(0))
    {
        face_page_ = std::make_unique<FacePage>(home);
        face_page_->setOnPlay([this]() {
            if (tileview_)
            {
                tileview_->setTileByIndex(1, 0, LV_ANIM_ON);
            }
        });
    }
    if (widgets::Widget* candle = add_tile(1))
    {
        candle_tile_ = candle;
        candle_page_ = std::make_unique<CandlePage>(candle);
        candle_page_->setOnBack([this]() {
            if (tileview_)
            {
                tileview_->setTileByIndex(0, 0, LV_ANIM_ON);
            }
        });
    }

    tileview_->setTileByIndex(0, 0, LV_ANIM_OFF);

    /* Mark the physical round panel. Sits above the tileview but is
     * transparent to touch so gestures still reach the pages. */
    auto* bezel = widgets_.emplace<widgets::Widget>(root_.get());
    bezel->setSize(diameter_ - 4, diameter_ - 4);
    bezel->setAlignment(LV_ALIGN_CENTER, 0, 0);
    bezel->setRadius(LV_RADIUS_CIRCLE);
    bezel->setBgOpacity(LV_OPA_TRANSP);
    bezel->setBorder(2, kColorAccent);
    lv_obj_clear_flag(bezel->lvObj(), LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(bezel->lvObj(), LV_OBJ_FLAG_SCROLLABLE);

    setVoiceState(VoiceState::Idle);
    if (face_page_)
    {
        face_page_->setActive(true);
    }
    if (candle_page_)
    {
        candle_page_->setActive(false);
    }

    page_sync_timer_ = sys::Timer::create(
        [this](sys::Timer*) {
            const bool candle = isCandlePageActive();
            if (face_page_)
            {
                face_page_->setActive(!candle);
            }
            if (candle_page_)
            {
                candle_page_->setActive(candle);
            }
        },
        100);
}

FacePanel::~FacePanel() = default;

bool FacePanel::isCandlePageActive() const
{
    return tileview_ != nullptr && candle_tile_ != nullptr && tileview_->activeTile() == candle_tile_;
}

void FacePanel::setVoiceState(VoiceState state)
{
    if (face_page_)
    {
        face_page_->setVoiceState(state);
    }
    if (candle_page_)
    {
        candle_page_->setVoiceState(state);
    }
}

void FacePanel::setMicLevel(std::uint8_t level)
{
    if (face_page_)
    {
        face_page_->setMicLevel(level);
    }
    if (candle_page_)
    {
        candle_page_->setMicLevel(level);
    }
}

void FacePanel::setTime(std::string_view hhmm)
{
    if (face_page_)
    {
        face_page_->setTime(hhmm);
    }
}

void FacePanel::setStatus(std::string_view status)
{
    if (face_page_)
    {
        face_page_->setStatus(status);
    }
}

}    // namespace lvglpp::demo
