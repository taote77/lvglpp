#ifndef LVGLPP_WIDGETSTORE_H
#define LVGLPP_WIDGETSTORE_H

#include "Widget.h"

#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

namespace lvglpp::widgets {

/**
 * Owns the C++ wrappers for a group of widgets whose lv_obj objects are owned
 * by the LVGL tree (or by another long-lived wrapper).
 *
 * Background: Widget wrappers are non-owning handles for child lv_obj
 * objects and event handlers are removed when the wrapper dies. Application
 * code that creates an interactive widget in a local scope must keep its
 * wrapper alive, otherwise the widget stays visible but becomes inert.
 *
 * WidgetStore makes that lifetime explicit:
 *
 *   WidgetStore store;
 *   auto *button = store.emplace<PushButton>(120, 40, PushButton::Blue, "OK", parent);
 *   button->onClicked([] { ... });
 *
 * The stored wrappers are destroyed together with the store. The underlying
 * lv_obj objects remain owned by their LVGL parent.
 */
class WidgetStore
{
public:
    WidgetStore() = default;

    ~WidgetStore() = default;

    WidgetStore(const WidgetStore &)            = delete;
    WidgetStore &operator=(const WidgetStore &) = delete;

    template <class T, class... Args>
    T *emplace(Args &&...args)
    {
        auto item = std::make_unique<T>(std::forward<Args>(args)...);
        T   *raw  = item.get();
        items_.push_back(std::move(item));
        return raw;
    }

    template <class T>
    T *add(std::unique_ptr<T> item)
    {
        T *raw = item.get();
        items_.push_back(std::move(item));
        return raw;
    }

    /// Destroy all wrappers. Their lv_obj objects stay alive under their parent.
    void clear()
    {
        items_.clear();
    }

    std::size_t size() const
    {
        return items_.size();
    }

    bool empty() const
    {
        return items_.empty();
    }

private:
    std::vector<std::unique_ptr<Widget>> items_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_WIDGETSTORE_H
