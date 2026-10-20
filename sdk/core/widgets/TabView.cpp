#include "TabView.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

TabView::TabView(BaseItem *parent) : BaseItem(tools::LvObjFactory::createLvglTabView, parent)
{}

std::unique_ptr<BaseItem> TabView::addTab(const std::string &name)
{
    // lv_tabview_add_tab returns the content lv_obj_t*. Wrap it directly instead
    // of creating a separate object and deleting LVGL's content page.
    lv_obj_t *contentObj = lv_tabview_add_tab(getLvglItem(), name.c_str());
    if (contentObj == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<BaseItem>(BaseItem::AdoptExistingTag::Adopt, contentObj, this, false);
}

void TabView::renameTab(uint32_t tabIdx, const std::string &newName)
{
    lv_tabview_rename_tab(getLvglItem(), tabIdx, newName.c_str());
}

void TabView::setActive(uint32_t tabIdx, bool animate)
{
    lv_tabview_set_active(getLvglItem(), tabIdx, animate ? LV_ANIM_ON : LV_ANIM_OFF);
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
    lv_obj_t *content_container = lv_tabview_get_content(getLvglItem());
    if (content_container == nullptr || tabIdx >= lv_obj_get_child_count(content_container))
    {
        return nullptr;
    }
    return lv_obj_get_child(content_container, static_cast<int32_t>(tabIdx));
}

} // namespace lvglpp::widgets
