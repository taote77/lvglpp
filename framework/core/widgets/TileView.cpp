#include "core/base/LvObjFactory.h"

#include "TileView.h"

namespace lvglpp::widgets {

TileView::TileView(Widget *parent) : Widget(tools::LvObjFactory::createLvglTileView, parent)
{}

std::unique_ptr<Widget> TileView::addTile(std::uint32_t col, std::uint32_t row, lv_dir_t dir)
{
    if (lvObj() == nullptr)
    {
        return nullptr;
    }

    lv_obj_t *tile = lv_tileview_add_tile(lvObj(), static_cast<std::uint8_t>(col), static_cast<std::uint8_t>(row), dir);
    if (tile == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<Widget>(Widget::AdoptExistingTag::Adopt, tile);
}

void TileView::setTileByIndex(std::uint32_t col, std::uint32_t row, lv_anim_enable_t anim)
{
    if (lvObj() != nullptr)
    {
        lv_tileview_set_tile_by_index(lvObj(), col, row, anim);
    }
}

Widget *TileView::activeTile() const
{
    if (lvObj() == nullptr)
    {
        return nullptr;
    }
    return Widget::fromLvglObject(lv_tileview_get_tile_active(lvObj()));
}

} // namespace lvglpp::widgets
