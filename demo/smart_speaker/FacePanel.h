#ifndef LVGLPP_FACE_PANEL_H
#define LVGLPP_FACE_PANEL_H

#include "CandlePage.h"
#include "core/sys/Timer.h"
#include "core/widgets/TileView.h"
#include "core/widgets/Widget.h"
#include "core/widgets/WidgetStore.h"
#include "FacePage.h"
#include "FaceTypes.h"

#include <cstdint>
#include <memory>
#include <string_view>

namespace lvglpp::demo
{

/**
 * A 360x360 smart-speaker UI composed from a FacePage and a CandlePage inside
 * one TileView. The host can also use the two pages directly in its own
 * navigation (this is what the smart-speaker app does).
 */
class FacePanel
{
public:
    explicit FacePanel(widgets::Widget* parent, int diameter = 360);
    ~FacePanel();

    FacePanel(const FacePanel&)            = delete;
    FacePanel& operator=(const FacePanel&) = delete;
    FacePanel(FacePanel&&)                 = delete;
    FacePanel& operator=(FacePanel&&)      = delete;

    widgets::Widget* widget() const { return root_.get(); }

    bool isCandlePageActive() const;

    void setVoiceState(VoiceState state);
    void setMicLevel(std::uint8_t level);
    void setTime(std::string_view hhmm);
    void setStatus(std::string_view status);

private:
    int diameter_ = 360;

    std::unique_ptr<widgets::Widget> root_;
    std::unique_ptr<widgets::TileView> tileview_;
    widgets::WidgetStore widgets_;

    widgets::Widget* candle_tile_ = nullptr;
    std::unique_ptr<FacePage> face_page_;
    std::unique_ptr<CandlePage> candle_page_;
    std::unique_ptr<sys::Timer> page_sync_timer_;
};

}    // namespace lvglpp::demo

#endif    // LVGLPP_FACE_PANEL_H
