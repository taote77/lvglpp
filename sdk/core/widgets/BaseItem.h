/**************************************************************************

Class:BaseItem


**************************************************************************/

#ifndef LVGLPP_BASEITEM_H
#define LVGLPP_BASEITEM_H

#include "core/base/Object.h"
#include "core/log/log.h"
#include "lvgl.h"
#include <functional>
#include <sstream>
#include <string>
#include <vector>

namespace lvglpp::widgets {

using namespace lvglpp::core;

class BaseItem : public Object
{
public:
    /// Factory function type: takes parent lv_obj_t*, returns created lv_obj_t*
    using Creator = std::function<lv_obj_t *(lv_obj_t *)>;

    explicit BaseItem(BaseItem *parentItem = nullptr);

    explicit BaseItem(Creator creator, BaseItem *parentItem = nullptr);

    virtual ~BaseItem();

    virtual void setPos(lv_coord_t x, lv_coord_t y);

    lv_coord_t getX() const;

    lv_coord_t getY() const;

    virtual void setAligment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY);

    /// Correct spelling: setAlignment
    void setAlignment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY) {
        setAligment(align, offsetX, offsetY);
    }

    virtual void setAligmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY);

    /// Correct spelling: setAlignmentTo
    void setAlignmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY) {
        setAligmentTo(item, align, offsetX, offsetY);
    }

    void setBgColor(uint32_t clr);

    void setDisableBgColor(uint32_t clr);

    /**
     * Set opacity (0.0 = fully transparent, 1.0 = fully opaque)
     * @param opa opacity value in range [0.0, 1.0]
     */
    void setOpacity(double opa);

    void setSize(lv_coord_t width, lv_coord_t height);

    void setWidth(lv_coord_t w);

    lv_coord_t getWidth() const;

    lv_coord_t getHeight() const;

    virtual void setVisible(bool visible);

    bool getVisible() const;

    virtual void setEnable(bool enable);

    bool getEnable() const;

    void setRadius(lv_coord_t radius);

    lv_coord_t getRadius() const;

    void setClip(bool clip);

    /**
     * 回弹效果
     * @param b
     */

    void setBorder(lv_coord_t width, uint32_t color);

    BaseItem *getParent() const
    {
        return this->parent;
    }

    void setParent(BaseItem *parent);

    virtual void setOnClickedListener(const std::function<void()> &l)
    {
        this->clicked_cb_ = l;
    }

    virtual void setOnPressedListener(const std::function<void()> &l)
    {
        this->pressed_cb_ = l;
    }

    virtual void setOnReleasedListener(const std::function<void()> &l)
    {
        this->released_cb_ = l;
    }

    lv_obj_t *getLvglItem() const
    {
        return lv_base_ptr_;
    }

    void setPaddingBottom(lv_coord_t padding);

    void setPaddingTop(lv_coord_t padding);

    void setPaddingHor(lv_coord_t padding);

    // ---- Introspection / Accessibility API (for AI agent, testing, automation) ----

    /// Set a human-readable name for this widget (like HTML id attribute)
    void setName(const std::string &n) { _name = n; }
    const std::string &name() const { return _name; }

    /// Number of direct child widgets
    uint32_t childCount() const;

    /// Get the nth child widget (0-indexed), or nullptr
    BaseItem *childAt(uint32_t index) const;

    /// Recursively find the first descendant with the given name, or nullptr
    BaseItem *findChild(const std::string &n) const;

    /// Find all descendants (optionally of type T). Returns self if matching.
    template <typename T = BaseItem>
    std::vector<T *> findChildrenByType() const
    {
        std::vector<T *> result;
        _collectByType<T>(result);
        return result;
    }

    /// Dump the widget tree to stdout (for debugging/AI observation)
    void dumpTree(int depth = 0) const;

    /// Get a JSON-like representation of this widget's state (for AI consumption)
    std::string describeState() const;

    /// Get the screen-relative bounding rectangle {x, y, w, h}
    struct Rect { lv_coord_t x, y, w, h; };
    Rect bounds() const;

    // ---- Layout API (wrapping LVGL flex/grid) ----

    /// Set flex flow on this container. Children are laid out automatically.
    void setFlexFlow(lv_flex_flow_t flow);

    /// Set flex align for main axis and cross axis + track (wrap) alignment
    void setFlexAlign(lv_flex_align_t mainAxis, lv_flex_align_t crossAxis, lv_flex_align_t trackAlign);

    /// Set flex grow factor for this item (0 = fixed, >0 = proportion of remaining space)
    void setFlexGrow(uint8_t grow);

    /// Set grid layout with column descriptors (e.g. "1fr 2fr auto")
    void setGridColumns(const std::string &columnDescriptors);

    /// Set grid layout row descriptors
    void setGridRows(const std::string &rowDescriptors);

    /// Set this item's grid cell position (column, row) and optional span
    void setGridCell(uint32_t column, uint32_t row, uint32_t colSpan = 1, uint32_t rowSpan = 1);

    /// Set grid cell column alignment
    void setGridCellColumnAlign(lv_grid_align_t align);

    /// Set grid cell row alignment
    void setGridCellRowAlign(lv_grid_align_t align);

protected:
    void setLvglItem(lv_obj_t *ptr)
    {
        lv_base_ptr_ = ptr;
    }

    void registerEvent();

private:
    BaseItem             *parent       = nullptr;
    lv_obj_t             *lv_base_ptr_ = nullptr;
    std::function<void()> clicked_cb_  = nullptr;
    std::function<void()> pressed_cb_  = nullptr;
    std::function<void()> released_cb_ = nullptr;
    lv_coord_t            width_       = 0;
    lv_coord_t            height_      = 0;
    std::string           _name;

    void initItem(const Creator &creator);

    template <typename T>
    void _collectByType(std::vector<T *> &result) const
    {
        if (auto *p = dynamic_cast<T *>(const_cast<BaseItem *>(this)))
            result.push_back(p);
        for (uint32_t i = 0; i < childCount(); i++) {
            if (auto *child = childAt(i))
                child->_collectByType<T>(result);
        }
    }

    friend class BaseDialog;
};
} // namespace lvglpp::widgets

#endif // LVGLPP_BASEITEM_H
