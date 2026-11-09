#include "InputManager.h"

#include "core/widgets/Widget.h"

#include <algorithm>
#include <utility>

namespace lvglpp::sys {

InputManager &InputManager::getInstance()
{
    static InputManager instance;
    return instance;
}

void InputManager::install()
{
    if (installed_)
    {
        return;
    }
    installed_ = true;

    virtual_indev_ = lv_indev_create();
    if (virtual_indev_ != nullptr)
    {
        lv_indev_set_type(virtual_indev_, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_read_cb(virtual_indev_, &InputManager::virtualReadCb);
        lv_indev_set_user_data(virtual_indev_, this);
        if (lv_display_t *display = lv_display_get_default(); display != nullptr)
        {
            lv_indev_set_display(virtual_indev_, display);
        }
    }

    syncIndevs();
}

void InputManager::shutdown()
{
    while (!scopes_.empty())
    {
        popScope(scopes_.back().id);
    }

    if (virtual_indev_ != nullptr)
    {
        lv_indev_delete(virtual_indev_);
        virtual_indev_ = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        queue_.clear();
    }
    active_raw_ = 0;
    long_sent_  = false;

    for (auto &slot : global_handlers_)
    {
        if (slot.connection)
        {
            slot.connection->connected     = false;
            slot.connection->on_disconnect = nullptr;
        }
    }
    global_handlers_.clear();
    back_entries_.clear();
    installed_ = false;
}

// ---------------------------------------------------------------- focus scopes

InputManager::ScopeToken InputManager::pushScope(widgets::Widget *root, BackHandler back, BackPriority priority)
{
    install();

    Scope scope;
    scope.id       = next_scope_id_++;
    scope.root     = root;
    scope.group    = lv_group_create();
    scope.priority = priority;
    scope.back     = std::move(back);

    const ScopeToken token = scope.id;
    scopes_.push_back(std::move(scope));
    std::stable_sort(scopes_.begin(), scopes_.end(), [](const Scope &a, const Scope &b) {
        if (a.priority != b.priority)
        {
            return static_cast<std::int32_t>(a.priority) < static_cast<std::int32_t>(b.priority);
        }
        return a.id < b.id;
    });

    activateTopScope();
    return token;
}

InputManager::ScopeToken InputManager::pushModalScope(widgets::Widget *root, BackHandler back, BackPriority priority)
{
    return pushScope(root, std::move(back), priority);
}

void InputManager::popScope(ScopeToken token)
{
    auto it = std::find_if(scopes_.begin(), scopes_.end(), [token](const Scope &scope) { return scope.id == token; });
    if (it == scopes_.end())
    {
        return;
    }

    deactivateScope(*it);
    scopes_.erase(it);
    activateTopScope();
}

void InputManager::refresh(ScopeToken token)
{
    auto *top = topVisibleScope();
    if (top != nullptr && top->id == token)
    {
        activateScope(*top);
    }
}

bool InputManager::refreshScopeContaining(widgets::Widget *item)
{
    if (item == nullptr)
    {
        return false;
    }

    Scope *top = topVisibleScope();
    if (top == nullptr || !isUnderScope(*top, item))
    {
        return false;
    }

    activateScope(*top);
    return true;
}

void InputManager::refreshFor(widgets::Widget *item)
{
    Scope *top = topVisibleScope();
    if (item == nullptr || top == nullptr || item->lvObj() == nullptr)
    {
        return;
    }

    Scope &scope = *top;
    if (scope.group == nullptr || !isUnderScope(scope, item))
    {
        return;
    }

    const bool visible = item->isEffectivelyVisible();
    const bool enabled = item->isEnabled();
    if (item->isFocusable() && visible && enabled)
    {
        if (lv_obj_get_group(item->lvObj()) != scope.group)
        {
            lv_group_add_obj(scope.group, item->lvObj());
        }

        // Replace a focus sink once a real focusable widget appears.
        if (scope.sink != nullptr && lv_group_get_focused(scope.group) == scope.sink->lvObj())
        {
            widgets::Widget *sink = scope.sink;
            scope.sink            = nullptr;
            lv_group_remove_obj(sink->lvObj());
            lv_group_focus_obj(item->lvObj());
        } else if (lv_group_get_focused(scope.group) == nullptr)
        {
            lv_group_focus_obj(item->lvObj());
        }
    } else
    {
        lv_group_remove_obj(item->lvObj());
    }
}

bool InputManager::requestFocus(widgets::Widget *item)
{
    if (item == nullptr || item->lvObj() == nullptr || !item->isFocusable())
    {
        return false;
    }

    Scope *top = topVisibleScope();
    if (top == nullptr || top->group == nullptr || !isUnderScope(*top, item))
    {
        return false;
    }

    Scope &scope = *top;

    lv_group_add_obj(scope.group, item->lvObj());
    lv_group_focus_obj(item->lvObj());
    return lv_group_get_focused(scope.group) == item->lvObj();
}

widgets::Widget *InputManager::focusedItem() const
{
    lv_group_t *group = activeGroup();
    if (group == nullptr)
    {
        return nullptr;
    }
    return widgets::Widget::fromLvglObject(lv_group_get_focused(group));
}

void InputManager::focusFirst()
{
    lv_group_t *group = activeGroup();
    if (group != nullptr)
    {
        lv_group_focus_next(group);
    }
}

void InputManager::focusLast()
{
    lv_group_t *group = activeGroup();
    if (group != nullptr)
    {
        lv_group_focus_prev(group);
    }
}

lv_group_t *InputManager::activeGroup() const
{
    const Scope *top = topVisibleScope();
    return top != nullptr ? top->group : nullptr;
}

InputManager::Scope *InputManager::topVisibleScope()
{
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        if (it->root != nullptr && it->root->isEffectivelyVisible())
        {
            return &*it;
        }
    }
    return nullptr;
}

const InputManager::Scope *InputManager::topVisibleScope() const
{
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        if (it->root != nullptr && it->root->isEffectivelyVisible())
        {
            return &*it;
        }
    }
    return nullptr;
}

void InputManager::activateTopScope()
{
    Scope *top = topVisibleScope();
    if (top != nullptr)
    {
        activateScope(*top);
    } else
    {
        syncIndevs();
    }
}

void InputManager::activateScope(Scope &scope)
{
    if (scope.group == nullptr)
    {
        return;
    }

    lv_group_remove_all_objs(scope.group);
    scope.sink = nullptr;

    std::vector<lv_obj_t *> focusables;
    if (scope.root != nullptr)
    {
        collectFocusables(scope.root->lvObj(), focusables);
    }

    for (lv_obj_t *obj : focusables)
    {
        lv_group_add_obj(scope.group, obj);
    }

    if (focusables.empty() && scope.root != nullptr && scope.root->lvObj() != nullptr)
    {
        // Focus sink: keeps global key routing working in an otherwise empty UI.
        scope.sink = scope.root;
        lv_group_add_obj(scope.group, scope.root->lvObj());
    }

    syncIndevs();

    if (!focusables.empty())
    {
        lv_group_focus_obj(focusables.front());
    } else if (scope.sink != nullptr)
    {
        lv_group_focus_obj(scope.sink->lvObj());
    }
}

void InputManager::deactivateScope(Scope &scope)
{
    if (scope.group == nullptr)
    {
        return;
    }

    if (lv_group_get_default() == scope.group)
    {
        lv_group_set_default(nullptr);
    }

    lv_group_remove_all_objs(scope.group);
    lv_group_delete(scope.group);
    scope.group = nullptr;
    scope.sink  = nullptr;
}

void InputManager::collectFocusables(lv_obj_t *obj, std::vector<lv_obj_t *> &out) const
{
    if (obj == nullptr)
    {
        return;
    }

    bool visible = true;
    for (lv_obj_t *parent = obj; parent != nullptr; parent = lv_obj_get_parent(parent))
    {
        if (lv_obj_has_flag(parent, LV_OBJ_FLAG_HIDDEN))
        {
            visible = false;
            break;
        }
    }

    if (visible)
    {
        if (widgets::Widget *item = widgets::Widget::fromLvglObject(obj); item != nullptr)
        {
            if (item->isFocusable() && item->isEnabled())
            {
                out.push_back(obj);
            }
        } else if (lv_obj_is_group_def(obj) && lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICKABLE))
        {
            // Native LVGL widgets without a C++ wrapper (e.g. TabView tab
            // buttons) can still participate in keyboard navigation.
            out.push_back(obj);
        }
    }

    const uint32_t count = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < count; ++i)
    {
        collectFocusables(lv_obj_get_child(obj, static_cast<int32_t>(i)), out);
    }
}

bool InputManager::isUnderScope(const Scope &scope, const widgets::Widget *item) const
{
    if (scope.root == nullptr || scope.root->lvObj() == nullptr || item == nullptr || item->lvObj() == nullptr)
    {
        return false;
    }

    lv_obj_t *root_obj = scope.root->lvObj();
    for (lv_obj_t *obj = item->lvObj(); obj != nullptr; obj = lv_obj_get_parent(obj))
    {
        if (obj == root_obj)
        {
            return true;
        }
    }
    return false;
}

void InputManager::syncIndevs()
{
    lv_group_t *group = activeGroup();

    for (lv_indev_t *indev = lv_indev_get_next(nullptr); indev != nullptr; indev = lv_indev_get_next(indev))
    {
        const lv_indev_type_t type = lv_indev_get_type(indev);
        if (type == LV_INDEV_TYPE_KEYPAD || type == LV_INDEV_TYPE_ENCODER)
        {
            lv_indev_set_group(indev, group);
        }
    }

    lv_group_set_default(group);
}

// ---------------------------------------------------------------- key routing

void InputManager::postKey(widgets::Key key, bool pressed)
{
    const uint32_t raw = widgets::lvglKey(key);
    if (raw != 0)
    {
        postKeyRaw(raw, pressed);
    }
}

void InputManager::postKeyRaw(uint32_t raw, bool pressed)
{
    if (raw == 0)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(queue_mutex_);
    queue_.push_back(PostedKey{raw, pressed});
}

bool InputManager::dispatchKey(widgets::KeyEvent &event)
{
    widgets::Widget *focused = focusedItem();
    if (focused != nullptr)
    {
        focused->dispatchKeyEvent(event);
    } else
    {
        dispatchGlobalKey(event);
    }

    if (!event.consumed && event.action == widgets::KeyAction::Press && event.key == widgets::Key::Escape)
    {
        event.consumed = dispatchBack();
    }

    return event.consumed;
}

bool InputManager::dispatchGlobalKey(widgets::KeyEvent &event)
{
    auto slots = global_handlers_;
    for (auto &slot : slots)
    {
        if (event.consumed)
        {
            break;
        }
        if (slot.connection && slot.connection->connected && slot.callback)
        {
            slot.callback(event);
        }
    }
    return event.consumed;
}

core::Connection InputManager::addGlobalKeyHandler(GlobalKeyHandler handler)
{
    const auto id        = next_global_id_++;
    auto       state     = std::make_shared<core::detail::ConnectionState>();
    state->on_disconnect = [this, id]() { removeGlobalHandler(id); };

    global_handlers_.push_back(GlobalSlot{id, state, std::move(handler)});
    return core::Connection(state);
}

void InputManager::removeGlobalHandler(std::uint64_t id)
{
    auto found = std::find_if(global_handlers_.begin(), global_handlers_.end(), [id](const GlobalSlot &slot) { return slot.id == id; });
    if (found == global_handlers_.end())
    {
        return;
    }

    if (found->connection)
    {
        found->connection->connected     = false;
        found->connection->on_disconnect = nullptr;
    }
    global_handlers_.erase(found);
}

InputManager::BackToken InputManager::addBackHandler(BackPriority priority, BackHandler handler)
{
    if (!handler)
    {
        return 0;
    }

    const BackToken token = next_back_token_++;
    back_entries_.push_back(BackEntry{token, priority, std::move(handler)});
    return token;
}

void InputManager::removeBackHandler(BackToken token)
{
    back_entries_.erase(std::remove_if(back_entries_.begin(), back_entries_.end(), [token](const BackEntry &entry) { return entry.id == token; }), back_entries_.end());
}

bool InputManager::dispatchBack()
{
    // Copy first: a handler may pop scopes or destroy the widgets that own
    // later handlers.
    std::vector<BackHandler> scope_handlers;
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it)
    {
        if (it->root != nullptr && it->root->isEffectivelyVisible() && it->back)
        {
            scope_handlers.push_back(it->back);
        }
    }
    for (auto &handler : scope_handlers)
    {
        if (handler && handler())
        {
            return true;
        }
    }

    std::vector<BackEntry> entries = back_entries_;
    std::stable_sort(entries.begin(), entries.end(), [](const BackEntry &a, const BackEntry &b) {
        if (a.priority != b.priority)
        {
            return static_cast<std::int32_t>(a.priority) > static_cast<std::int32_t>(b.priority);
        }
        return a.id > b.id;
    });

    for (auto &entry : entries)
    {
        if (entry.handler && entry.handler())
        {
            return true;
        }
    }
    return false;
}

void InputManager::virtualReadCb(lv_indev_t *indev, lv_indev_data_t *data)
{
    auto *self = static_cast<InputManager *>(lv_indev_get_user_data(indev));
    if (self != nullptr)
    {
        self->fillKeyData(data);
    }
}

void InputManager::fillKeyData(lv_indev_data_t *data)
{
    std::vector<widgets::KeyEvent> events;

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);

        if (!queue_.empty())
        {
            const PostedKey posted = queue_.front();
            queue_.pop_front();

            if (posted.pressed)
            {
                if (active_raw_ != 0 && active_raw_ != posted.raw)
                {
                    events.push_back(widgets::KeyEvent{widgets::keyFromLvgl(active_raw_), active_raw_, widgets::KeyAction::Release, false});
                }

                const bool repeat = (active_raw_ == posted.raw);
                active_raw_       = posted.raw;
                press_tick_       = lv_tick_get();
                long_sent_        = false;

                data->key   = posted.raw;
                data->state = LV_INDEV_STATE_PRESSED;

                events.push_back(widgets::KeyEvent{widgets::keyFromLvgl(posted.raw), posted.raw, repeat ? widgets::KeyAction::Repeat : widgets::KeyAction::Press, false});
            } else
            {
                if (active_raw_ == posted.raw)
                {
                    data->key   = posted.raw;
                    data->state = LV_INDEV_STATE_RELEASED;

                    events.push_back(widgets::KeyEvent{widgets::keyFromLvgl(posted.raw), posted.raw, widgets::KeyAction::Release, false});
                    active_raw_ = 0;
                    long_sent_  = false;
                } else
                {
                    data->key   = 0;
                    data->state = LV_INDEV_STATE_RELEASED;
                }
            }
        } else if (active_raw_ != 0)
        {
            if (!long_sent_ && lv_tick_elaps(press_tick_) >= long_press_ms_)
            {
                long_sent_ = true;
                events.push_back(widgets::KeyEvent{widgets::keyFromLvgl(active_raw_), active_raw_, widgets::KeyAction::LongPress, false});
            }

            data->key   = active_raw_;
            data->state = LV_INDEV_STATE_PRESSED;
        } else
        {
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }

    for (auto &event : events)
    {
        dispatchKey(event);
    }
}

} // namespace lvglpp::sys
