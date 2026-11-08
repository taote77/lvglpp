#ifndef LVGLPP_TILEVIEW_H
#define LVGLPP_TILEVIEW_H

#include "Widget.h"

#include <cstdint>
#include <memory>

namespace lvglpp::widgets {

/// Wrapper around lv_tileview. Tiles are owned by the LVGL tileview; addTile()
/// returns a non-owning Widget handle that the caller must keep alive while
/// its callbacks are needed (widgets::WidgetStore is the usual owner).
class TileView : public Widget
{
public:
    explicit TileView(Widget *parent = nullptr);

    std::unique_ptr<Widget> addTile(std::uint32_t col, std::uint32_t row, lv_dir_t dir);

    void setTileByIndex(std::uint32_t col, std::uint32_t row, lv_anim_enable_t anim = LV_ANIM_OFF);

    /// Active tile handle, or nullptr if the wrapper is not alive.
    Widget *activeTile() const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TILEVIEW_H
