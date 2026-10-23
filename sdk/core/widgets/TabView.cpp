#include "TabView.h"
#include "core/base/LvObjFactory.h"

namespace lvglpp::widgets {

TabView::TabView(Widget *parent) : Widget(tools::LvObjFactory::createLvglTabView, parent)
{}

std::unique_ptr<Widget> TabView::addTab(const std::string &name)
{
    // lv_tabview_add_tab returns the content lv_obj_t*. Wrap it directly instead
    // of creating a separate object and deleting LVGL's content page.
    lv_obj_t *contentObj = lv_tabview_add_tab(lvObj(), name.c_str());
    if (contentObj == nullptr)
    {
        return nullptr;
    }
    return std::make_unique<Widget>(Widget::AdoptExistingTag::Adopt, contentObj, false);
}

void TabView::renameTab(uint32_t tabIdx, const std::string &newName)
{
    lv_tabview_rename_tab(lvObj(), tabIdx, newName.c_str());
}

void TabView::setActive(uint32_t tabIdx, bool animate)
{
    lv_tabview_set_active(lvObj(), tabIdx, animate ? LV_ANIM_ON : LV_ANIM_OFF);
}

uint32_t TabView::active() const
{
    return lv_tabview_get_tab_active(lvObj());
}

lv_obj_t *TabView::tabBar() const
{
    return lv_tabview_get_tab_bar(lvObj());
}

lv_obj_t *TabView::content(uint32_t tabIdx) const
{
    lv_obj_t *content_container = lv_tabview_get_content(lvObj());
    if (content_container == nullptr || tabIdx >= lv_obj_get_child_count(content_container))
    {
        return nullptr;
    }
    return lv_obj_get_child(content_container, static_cast<int32_t>(tabIdx));
}

} // namespace lvglpp::widgets
