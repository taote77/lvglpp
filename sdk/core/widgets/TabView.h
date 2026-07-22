#ifndef LVGLPP_TABVIEW_H
#define LVGLPP_TABVIEW_H

#include "BaseItem.h"
#include <string>

namespace lvglpp::widgets {

class TabView : public BaseItem
{
public:
    explicit TabView(BaseItem *parent = nullptr);

    /// Add a tab with the given name. Returns the content area BaseItem.
    BaseItem *addTab(const std::string &name);

    /// Rename a tab
    void renameTab(uint32_t tabIdx, const std::string &newName);

    /// Set the active tab by index
    void setActive(uint32_t tabIdx, bool animate = false);

    /// Get the active tab index
    uint32_t active() const;

    /// Get the tab button bar (for custom styling)
    lv_obj_t *tabBar() const;

    /// Get the content area for a specific tab index
    lv_obj_t *content(uint32_t tabIdx) const;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TABVIEW_H
