/**************************************************************************

Class: Widget

Ownership model
---------------
The LVGL parent owns its child lv_obj objects. A Widget wrapper is a
non-owning handle for child objects and may therefore be destroyed while its
lv_obj keeps living under the parent. Only top-level wrappers (no logical
parent) and wrappers whose creator attaches to a layer (lv_layer_top()/sys())
own their lv_obj and delete it in the destructor.

All event callbacks registered by the wrapper are removed when the wrapper is
destroyed. The wrapper also watches LV_EVENT_DELETE, so an externally deleted
lv_obj safely invalidates the wrapper instead of leaving a dangling pointer.

**************************************************************************/

#ifndef LVGLPP_WIDGET_H
#define LVGLPP_WIDGET_H

#include "KeyEvent.h"
#include "core/base/Connection.h"
#include "core/base/Object.h"
#include "core/log/log.h"
#include "core/sys/UiFeatures.h"
#include "lvgl.h"
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace lvglpp::core {
class Style;
}

namespace lvglpp::widgets {

using namespace lvglpp::core;

struct WidgetRegistryEntry;

class Widget : public Object
{
public:
    /// Factory function type: takes parent lv_obj*, returns created lv_obj*
    using Creator = std::function<lv_obj_t *(lv_obj_t *)>;

    /// Raw LVGL event handler (advanced escape hatch; removed with the wrapper)
    using RawEventHandler = std::function<void(lv_event_t *)>;

    using ClickHandler = std::function<void()>;
    using KeyHandler   = std::function<void(KeyEvent &)>;
    using FocusHandler = std::function<void(bool focused)>;

    enum class AdoptExistingTag { Adopt };

    explicit Widget(Widget *parentItem = nullptr);

    explicit Widget(Creator creator, Widget *parentItem = nullptr);

    /**
     * Wrap an already existing lv_obj (e.g. a lv_tabview page or an internal
     * widget child). The wrapper does not own the object unless takeOwnership
     * is true.
     */
    Widget(AdoptExistingTag, lv_obj_t *existing, bool takeOwnership = false);

    virtual ~Widget();

    Widget(const Widget &)            = delete;
    Widget &operator=(const Widget &) = delete;
    Widget(Widget &&)                 = delete;
    Widget &operator=(Widget &&)      = delete;

    // ---- Registry / lifetime diagnostics ----

    /// Resolve the live wrapper for an lv_obj, or nullptr.
    static Widget *fromLvglObject(lv_obj_t *obj);

    bool isAttached() const
    {
        return lv_base_ptr_ != nullptr;
    }

    bool ownsLvglObject() const
    {
        return owns_lv_obj_;
    }

    // ---- Geometry ----

    Widget &setPos(lv_coord_t x, lv_coord_t y);

    /// Physical position, never mirrored under RTL.
    Widget &setPosPhysical(lv_coord_t x, lv_coord_t y);

    lv_coord_t getX() const;

    lv_coord_t getY() const;

    Widget &setAlignment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY);

    /// Physical alignment, never mirrored under RTL.
    Widget &setAlignmentPhysical(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY);

    /// Opt out of automatic RTL mirroring for this item (e.g. charts).
    Widget &setMirrorInRtl(bool enabled = true);

    bool mirrorInRtl() const;

    /// Set the LVGL base direction for this subtree.
    Widget &setBaseDir(lv_base_dir_t direction);

    Widget &setAlignmentTo(const Widget &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY);

    Widget &setSize(lv_coord_t width, lv_coord_t height);

    Widget &setWidth(lv_coord_t w);

    Widget &setHeight(lv_coord_t h);

    /// Live layout width. Falls back to the requested width before first layout.
    lv_coord_t getWidth() const;

    /// Live layout height. Falls back to the requested height before first layout.
    lv_coord_t getHeight() const;

    Widget &setSizePercent(lv_coord_t widthPercent, lv_coord_t heightPercent);

    // ---- Appearance ----

    Widget &setBgColor(uint32_t clr);

    Widget &setDisabledBgColor(uint32_t clr);

    [[deprecated("use setDisabledBgColor()")]]
    Widget &setDisableBgColor(uint32_t clr)
    {
        return setDisabledBgColor(clr);
    }

    Widget &setBgOpacity(lv_opa_t opa);

    /**
     * Set opacity (0.0 = fully transparent, 1.0 = fully opaque)
     * @param opa opacity value in range [0.0, 1.0]
     */
    Widget &setOpacity(double opa);

    Widget &setRadius(lv_coord_t radius);

    lv_coord_t getRadius() const;

    /// Clip children to the rounded corners (LVGL clip_corner).
    Widget &setClip(bool clip);

    Widget &setBorder(lv_coord_t width, uint32_t color);

    /// Apply a reusable style. The style is kept alive until the lv_obj dies.
    Widget &applyStyle(const Style &style,
                       lv_style_selector_t selector = static_cast<lv_style_selector_t>(LV_PART_MAIN)
                                                        | static_cast<lv_style_selector_t>(LV_STATE_DEFAULT));

    /// Remove a style previously applied through applyStyle().
    Widget &removeStyle(const Style &style, lv_style_selector_t selector = 0);

    Widget &setState(lv_state_t state, bool enabled = true);

    Widget &addState(lv_state_t state);

    Widget &clearState(lv_state_t state);

    virtual void setVisible(bool visible);

    bool isVisible() const;

    /// True when this widget and every ancestor are visible (not HIDDEN).
    bool isEffectivelyVisible() const;

    [[deprecated("use isVisible()")]]
    bool getVisible() const
    {
        return isVisible();
    }

    virtual void setEnabled(bool enabled);

    [[deprecated("use setEnabled()")]]
    void setEnable(bool enabled)
    {
        setEnabled(enabled);
    }

    bool isEnabled() const;

    [[deprecated("use isEnabled()")]]
    bool getEnable() const
    {
        return isEnabled();
    }

    // ---- Padding / margin ----

    Widget &setPaddingAll(lv_coord_t padding);

    Widget &setPaddingHor(lv_coord_t padding);

    Widget &setPaddingVer(lv_coord_t padding);

    Widget &setPaddingTop(lv_coord_t padding);

    Widget &setPaddingBottom(lv_coord_t padding);

    Widget &setPaddingLeft(lv_coord_t padding);

    Widget &setPaddingRight(lv_coord_t padding);

    Widget &setPaddingRow(lv_coord_t padding);

    Widget &setPaddingColumn(lv_coord_t padding);

    Widget &setMarginAll(lv_coord_t margin);

    Widget &setMarginHor(lv_coord_t margin);

    Widget &setMarginVer(lv_coord_t margin);

    Widget &setMarginTop(lv_coord_t margin);

    Widget &setMarginBottom(lv_coord_t margin);

    Widget &setMarginLeft(lv_coord_t margin);

    Widget &setMarginRight(lv_coord_t margin);

    // ---- Tree ----

    /// Nearest live wrapper ancestor, or nullptr.
    Widget *getParent() const;

    /// Reparent the underlying lv_obj. Ownership follows the new parent.
    void setParent(Widget *parent);

    lv_obj_t *lvObj() const
    {
        return lv_base_ptr_;
    }

    [[deprecated("use lvObj()")]]
    lv_obj_t *getLvglItem() const
    {
        return lvObj();
    }

    // ---- Events ----

    /// Modern event API. The returned token does not disconnect on destruction;
    /// use core::ScopedConnection for RAII disconnection.
    Connection onClicked(ClickHandler handler);

    Connection onPressed(ClickHandler handler);

    Connection onReleased(ClickHandler handler);

    Connection onLongPressed(ClickHandler handler);

    /// Key handler. Unconsumed keys bubble to ancestors and then to
    /// InputManager's global handlers.
    Connection onKey(KeyHandler handler);

    /// Focus change handler.
    Connection onFocus(FocusHandler handler);

    /// Convenience: called with focused == false.
    Connection onBlur(FocusHandler handler);

    /// Dispatch a key through this widget and its ancestors.
    void dispatchKeyEvent(KeyEvent &event);

    /// Enroll this widget in the active focus group.
    Widget &setFocusable(bool focusable = true);

    bool isFocusable() const
    {
        return focusable_;
    }

    /// Give keyboard focus to this widget (must be focusable and in a scope).
    bool requestFocus();

    bool hasFocus() const;

    /// Called when the wrapper is destroyed or its lv_obj is deleted.
    Connection onDestroyed(std::function<void()> handler);

    /// Register a raw LVGL callback that is automatically removed together
    /// with this wrapper. Returns the event descriptor (may be nullptr).
    lv_event_dsc_t *addLvglEventHandler(lv_event_cb_t cb, lv_event_code_t filter, void *userData = nullptr);

    /// Same as above for a child lv_obj owned by this widget (e.g. MsgBox buttons).
    lv_event_dsc_t *addChildLvglEventHandler(lv_obj_t *child, lv_event_cb_t cb, lv_event_code_t filter, void *userData = nullptr);

    // Legacy single-listener API (kept for compatibility).
    virtual void setOnClickedListener(const std::function<void()> &l);
    virtual void setOnPressedListener(const std::function<void()> &l);
    virtual void setOnReleasedListener(const std::function<void()> &l);
    void         setOnLongPressedListener(const std::function<void()> &l);

    // ---- Keepalive for caller-owned raw pointers LVGL retains ----

    /**
     * Keep `data` alive until the underlying lv_obj is deleted. Used for grid
     * descriptor arrays, image source strings and styles.
     */
    void setObjectKeepalive(const std::string &key, std::shared_ptr<void> data);

    void clearObjectKeepalive(const std::string &key);

    // ---- Introspection / Accessibility API ----

    /// Set a human-readable name for this widget (like HTML id attribute)
    void setName(const std::string &n)
    {
        _name = n;
    }

    const std::string &name() const
    {
        return _name;
    }

    /// Number of direct children in the LVGL tree.
    uint32_t childCount() const;

    /// Get the nth child wrapper (0-indexed), or nullptr if it has no live wrapper.
    Widget *childAt(uint32_t index) const;

    /// Recursively find the first descendant (or self) with the given name.
    Widget *findChild(const std::string &n) const;

    /// Find all wrapper descendants of type T (including self when matching).
    template <typename T = Widget>
    std::vector<T *> findChildrenByType() const
    {
        std::vector<Widget *> wrappers;
        collectWrapperSubtree(lv_base_ptr_, wrappers);
        std::vector<T *> result;
        for (Widget *item : wrappers)
        {
            if (auto *typed = dynamic_cast<T *>(item))
            {
                result.push_back(typed);
            }
        }
        return result;
    }

    /// Dump the widget tree to stdout (for debugging/AI observation)
    void dumpTree(int depth = 0) const;

    /// Get a JSON-like representation of this widget's state
    std::string describeState() const;

    /// Screen-relative bounding rectangle {x, y, w, h}
    struct Rect {
        lv_coord_t x, y, w, h;
    };

    Rect bounds() const;

    // ---- Layout API (wrapping LVGL flex/grid) ----

    Widget &setFlexFlow(lv_flex_flow_t flow);

    Widget &setFlexAlign(lv_flex_align_t mainAxis, lv_flex_align_t crossAxis, lv_flex_align_t trackAlign);

    Widget &setFlexGrow(uint8_t grow);

    /// Set grid column descriptors (e.g. "1fr 2fr auto 100px")
    Widget &setGridColumns(const std::string &columnDescriptors);

    /// Set grid row descriptors
    Widget &setGridRows(const std::string &rowDescriptors);

    /// Set this item's grid cell position and span
    Widget &setGridCell(uint32_t column, uint32_t row, uint32_t colSpan = 1, uint32_t rowSpan = 1);

    /// Set grid cell column alignment (position/span are preserved)
    Widget &setGridCellColumnAlign(lv_grid_align_t align);

    /// Set grid cell row alignment (position/span are preserved)
    Widget &setGridCellRowAlign(lv_grid_align_t align);

protected:
    /// Virtual hook invoked when the underlying lv_obj is deleted externally.
    virtual void onLvglObjectDeleted()
    {}

    /// Internal: called by the LV_EVENT_DELETE watchdog.
    void handleUnderlyingObjectDeleted();

private:
    struct EventSlot {
        std::uint64_t                            id = 0;
        std::shared_ptr<detail::ConnectionState> connection;
        RawEventHandler                          callback;
    };

    struct DestroySlot {
        std::uint64_t                            id = 0;
        std::shared_ptr<detail::ConnectionState> connection;
        std::function<void()>                    callback;
    };

    struct KeySlot {
        std::uint64_t                            id = 0;
        std::shared_ptr<detail::ConnectionState> connection;
        KeyHandler                               callback;
    };

    struct FocusSlot {
        std::uint64_t                            id = 0;
        std::shared_ptr<detail::ConnectionState> connection;
        FocusHandler                             callback;
    };

    struct GridState {
        std::vector<lv_coord_t> cols;
        std::vector<lv_coord_t> rows;
        lv_grid_align_t         col_align = LV_GRID_ALIGN_STRETCH;
        lv_grid_align_t         row_align = LV_GRID_ALIGN_STRETCH;
        uint32_t                col_pos   = 0;
        uint32_t                row_pos   = 0;
        uint32_t                col_span  = 1;
        uint32_t                row_span  = 1;
    };

    static void objectDeleteWatchdog(lv_event_t *e);

    static void eventTrampoline(lv_event_t *e);

    static Widget *findChildInRawTree(lv_obj_t *obj, const std::string &name);

    static void collectWrapperSubtree(lv_obj_t *obj, std::vector<Widget *> &out);

    void initItem(const Creator &creator, Widget *parentItem);

    void attachToLvglObject(lv_obj_t *obj);

    void ensureEventHandler(lv_event_code_t code);

    Connection onSimpleEvent(lv_event_code_t code, ClickHandler handler);

    void dispatchLvglEvent(lv_event_t *e);

    void removeEventHandler(lv_event_code_t code, std::uint64_t id);

    void removeDestroyHandler(std::uint64_t id);

    void removeKeyHandler(std::uint64_t id);

    void removeFocusHandler(std::uint64_t id);

    void invokeKeyHandlers(KeyEvent &event);

    void notifyFocus(bool focused);

    void handleLvglKeyEvent(lv_event_t *e);

    void synthReleasePendingKey();

    void notifyDestroyedObservers();

    void detachEventCallbacks();

    void applyGridTemplates();

    void applyGridCell();

    void applyLogicalAlignment();

    GridState &ensureGridState();

    std::shared_ptr<detail::ConnectionState> makeConnection(std::function<void()> onDisconnect);

    lv_obj_t              *lv_base_ptr_      = nullptr;
    WidgetRegistryEntry *registry_entry_   = nullptr;
    bool                   owns_lv_obj_      = false;
    bool                   destroy_notified_ = false;

    lv_coord_t  requested_width_  = 0;
    lv_coord_t  requested_height_ = 0;
    std::string _name;

#if LVGLPP_ENABLE_RTL
    lv_align_t logical_align_     = LV_ALIGN_DEFAULT;
    lv_coord_t logical_x_         = 0;
    lv_coord_t logical_y_         = 0;
    bool       has_logical_align_ = false;
    bool       mirror_in_rtl_     = true;
#endif

    std::function<void()> clicked_cb_;
    std::function<void()> pressed_cb_;
    std::function<void()> released_cb_;
    std::function<void()> long_pressed_cb_;

    std::map<int, std::vector<EventSlot>>                event_handlers_;
    std::vector<DestroySlot>                             destroy_handlers_;
    std::vector<KeySlot>                                 key_handlers_;
    std::vector<FocusSlot>                               focus_handlers_;
    std::vector<int>                                     registered_codes_;
    std::vector<lv_event_dsc_t *>                        event_dscs_;
    std::vector<std::pair<lv_obj_t *, lv_event_dsc_t *>> child_event_dscs_;

    std::shared_ptr<bool>      alive_token_   = std::make_shared<bool>(true);
    std::uint64_t              next_slot_id_  = 0;
    std::uint64_t              next_style_id_ = 0;
    std::shared_ptr<GridState> grid_;

    bool                         focusable_ = false;
    std::shared_ptr<core::Style> focus_style_;
    uint32_t                     pending_key_raw_  = 0;
    uint32_t                     pending_key_tick_ = 0;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_WIDGET_H
