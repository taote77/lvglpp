#include "TabView.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

TabView::TabView(BaseItem *parent)
    : BaseItem(tools::LvObjFactory::createLvglTabView, parent)
{}

BaseItem *TabView::addTab(const std::string &name)
{
    // lv_tabview_add_tab returns the content lv_obj_t*
    auto *contentObj = lv_tabview_add_tab(getLvglItem(), name.c_str());
    // Return a lightweight wrapper; caller owns the BaseItem through LVGL hierarchy
    auto *item = new BaseItem(tools::LvObjFactory::createLvglItem, this);
    // Replace the default content with our wrapper
    lv_obj_del(contentObj);
    item->setParent(this);
    return item;
}

void TabView::renameTab(uint32_t tabIdx, const std::string &newName)
{
    lv_tabview_rename_tab(getLvglItem(), tabIdx, newName.c_str());
}

void TabView::setActive(uint32_t tabIdx, bool animate)
{
    lv_tabview_set_active(getLvglItem(), tabIdx,
        animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

uint32_t TabView::active() const
{
    return lv_tabview_get_tab_active(getLvglItem());
}

lv_obj_t *TabView::tabBar() const
{
    return lv_tabview_get_tab_bar(getLvglItem());
}

lv_obj_t *TabView::content(uint32_t tabIdx) const
{
    return lv_tabview_get_content(getLvglItem());
}

} // namespace lvglpp::widgets
