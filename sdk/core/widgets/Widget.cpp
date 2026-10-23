#include "Widget.h"

#include "GridLayout.h"
#include "core/base/LvObjFactory.h"
#include "core/style/Style.h"
#include "core/style/Theme.h"
#include "core/sys/InputManager.h"
#include "core/sys/UiConfig.h"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace {
constexpr std::uint32_t kKeyRepeatMs             = 250;
constexpr int           kMaxAncestorWalk         = 64;
constexpr lv_coord_t    kMaxRequestedWidthSanity = 4096;

std::string jsonEscape(const std::string &value)
{
    std::string out;
    out.reserve(value.size());
    for (const unsigned char c : value)
    {
        switch (c)
        {
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        case '\n':
            out += "\\n";
            break;
        case '\r':
            out += "\\r";
            break;
        case '\t':
            out += "\\t";
            break;
        default:
            if (c < 0x20)
            {
                char buffer[7];
                std::snprintf(buffer, sizeof(buffer), "\\u%04X", c);
                out += buffer;
            } else
            {
                out += static_cast<char>(c);
            }
            break;
        }
    }
    return out;
}
} // namespace

#if LVGLPP_ENABLE_RTL

namespace {

lv_align_t mirrorAlignForRtl(lv_align_t align)
{
    switch (align)
    {
    case LV_ALIGN_TOP_LEFT:
        return LV_ALIGN_TOP_RIGHT;
    case LV_ALIGN_TOP_RIGHT:
        return LV_ALIGN_TOP_LEFT;
    case LV_ALIGN_BOTTOM_LEFT:
        return LV_ALIGN_BOTTOM_RIGHT;
    case LV_ALIGN_BOTTOM_RIGHT:
        return LV_ALIGN_BOTTOM_LEFT;
    case LV_ALIGN_LEFT_MID:
        return LV_ALIGN_RIGHT_MID;
    case LV_ALIGN_RIGHT_MID:
        return LV_ALIGN_LEFT_MID;
    case LV_ALIGN_OUT_TOP_LEFT:
        return LV_ALIGN_OUT_TOP_RIGHT;
    case LV_ALIGN_OUT_TOP_RIGHT:
        return LV_ALIGN_OUT_TOP_LEFT;
    case LV_ALIGN_OUT_BOTTOM_LEFT:
        return LV_ALIGN_OUT_BOTTOM_RIGHT;
    case LV_ALIGN_OUT_BOTTOM_RIGHT:
        return LV_ALIGN_OUT_BOTTOM_LEFT;
    case LV_ALIGN_OUT_LEFT_TOP:
        return LV_ALIGN_OUT_RIGHT_TOP;
    case LV_ALIGN_OUT_RIGHT_TOP:
        return LV_ALIGN_OUT_LEFT_TOP;
    case LV_ALIGN_OUT_LEFT_MID:
        return LV_ALIGN_OUT_RIGHT_MID;
    case LV_ALIGN_OUT_RIGHT_MID:
        return LV_ALIGN_OUT_LEFT_MID;
    case LV_ALIGN_OUT_LEFT_BOTTOM:
        return LV_ALIGN_OUT_RIGHT_BOTTOM;
    case LV_ALIGN_OUT_RIGHT_BOTTOM:
        return LV_ALIGN_OUT_LEFT_BOTTOM;
    default:
        return align;
    }
}

} // namespace

#endif // LVGLPP_ENABLE_RTL

namespace lvglpp::widgets {

/**
 * One registry entry per lv_obj. It survives wrapper destruction and is
 * released by the single LV_EVENT_DELETE watchdog. Because it can outlive the
 * wrapper it also owns "keepalive" data that LVGL stores by raw pointer
 * (grid templates, image source strings, styles).
 *
 * More than one wrapper can adopt the same lv_obj (e.g. re-wrapping a tab
 * page). All of them are tracked here; the newest one is canonical for
 * fromLvglObject().
 */
struct WidgetRegistryEntry {
    struct WrapperRef {
        Widget              *widget = nullptr;
        std::weak_ptr<bool>  alive;
    };

    lv_obj_t                                              *obj = nullptr;
    std::vector<WrapperRef>                                wrappers;
    std::unordered_map<std::string, std::shared_ptr<void>> keepalive;
};

namespace {

using Registry = std::unordered_map<lv_obj_t *, WidgetRegistryEntry *>;

Registry &registry()
{
    static Registry map;
    return map;
}

WidgetRegistryEntry *entryFor(lv_obj_t *obj)
{
    auto it = registry().find(obj);
    return it == registry().end() ? nullptr : it->second;
}

} // namespace

// ---------------------------------------------------------------- lifetime

Widget::Widget(Widget *parentItem) : Widget(tools::LvObjFactory::createLvglItem, parentItem)
{}

Widget::Widget(Creator creator, Widget *parentItem)
{
    initItem(creator, parentItem);
}

Widget::Widget(AdoptExistingTag, lv_obj_t *existing, bool takeOwnership)
{
    owns_lv_obj_ = takeOwnership;
    attachToLvglObject(existing);
}

Widget::~Widget()
{
    if (alive_token_)
    {
        *alive_token_ = false;
    }

    // Stop LVGL animations that use this wrapper as their variable.
    if (lv_base_ptr_ != nullptr)
    {
        lv_anim_delete(this, nullptr);
    }

    notifyDestroyedObservers();
    detachEventCallbacks();

    lv_obj_t *obj = lv_base_ptr_;
    if (registry_entry_ != nullptr)
    {
        // The entry may outlive the wrapper (it also owns keepalive data).
        auto &wrappers = registry_entry_->wrappers;
        wrappers.erase(std::remove_if(wrappers.begin(), wrappers.end(),
                                      [this](const WidgetRegistryEntry::WrapperRef &ref) { return ref.widget == this; }),
                       wrappers.end());
    }
    lv_base_ptr_ = nullptr;

    if (obj != nullptr && lv_obj_is_valid(obj))
    {
        if (lv_obj_get_group(obj) != nullptr)
        {
            lv_group_remove_obj(obj);
        }
        if (owns_lv_obj_)
        {
            lv_obj_delete(obj);
        }
    }

    registry_entry_ = nullptr;
}

void Widget::initItem(const Creator &creator, Widget *parentItem)
{
    lv_obj_t *expected_parent  = lv_scr_act();
    Widget   *effective_parent = parentItem;

    if (effective_parent != nullptr)
    {
        expected_parent = effective_parent->lvObj();
        if (expected_parent == nullptr)
        {
            LOG_WARN() << "Widget parent has no lv_obj; attaching to the active screen";
            effective_parent = nullptr;
            expected_parent  = lv_scr_act();
        }
    }

    if (!creator)
    {
        LOG_ERROR() << "Widget created without a creator";
        return;
    }

    lv_obj_t *obj = creator(expected_parent);
    if (obj == nullptr)
    {
        LOG_ERROR() << "Widget creator returned nullptr";
        return;
    }

    // A creator may ignore `expected_parent` and attach to lv_layer_top()/sys().
    // Such objects are not owned by the LVGL parent we expected, so the wrapper
    // must delete them.
    owns_lv_obj_ = (effective_parent == nullptr) || (lv_obj_get_parent(obj) != expected_parent);
    attachToLvglObject(obj);
}

void Widget::attachToLvglObject(lv_obj_t *obj)
{
    lv_base_ptr_ = obj;
    if (obj == nullptr)
    {
        return;
    }

    WidgetRegistryEntry *entry = entryFor(obj);
    if (entry == nullptr)
    {
        entry           = new WidgetRegistryEntry();
        entry->obj      = obj;
        registry()[obj] = entry;

        // The watchdog is registered with the Entry as user data (never
        // `this`), so the wrapper destructor's callback sweep cannot remove
        // it and only one watchdog exists per lv_obj.
        lv_obj_add_event_cb(obj, &Widget::objectDeleteWatchdog, LV_EVENT_DELETE, entry);
    } else if (!entry->wrappers.empty())
    {
        LOG_WARN() << "multiple Widget wrappers for one lv_obj; the newest is canonical";
    }

    entry->wrappers.push_back(WidgetRegistryEntry::WrapperRef{this, alive_token_});
    registry_entry_ = entry;

    // Legacy callbacks are always available.
    ensureEventHandler(LV_EVENT_CLICKED);
    ensureEventHandler(LV_EVENT_PRESSED);
    ensureEventHandler(LV_EVENT_RELEASED);

    // Key/focus handling. KEY is registered as PREPROCESS so widget handlers
    // can consume a key before the native widget class handles it.
    ensureEventHandler(static_cast<lv_event_code_t>(LV_EVENT_KEY | LV_EVENT_PREPROCESS));
    ensureEventHandler(LV_EVENT_FOCUSED);
    ensureEventHandler(LV_EVENT_DEFOCUSED);
}

void Widget::objectDeleteWatchdog(lv_event_t *e)
{
    auto *entry = static_cast<WidgetRegistryEntry *>(lv_event_get_user_data(e));
    if (entry == nullptr)
    {
        return;
    }

    auto it = registry().find(entry->obj);
    if (it != registry().end() && it->second == entry)
    {
        registry().erase(it);
    }

    // Copy the list first: handleUnderlyingObjectDeleted() must not observe a
    // half-destroyed entry, and the entry itself is released at the end.
    std::vector<WidgetRegistryEntry::WrapperRef> wrappers = std::move(entry->wrappers);
    entry->wrappers.clear();
    for (const auto &ref : wrappers)
    {
        // An observer of an earlier wrapper may have destroyed this wrapper;
        // the alive token catches that before dereferencing it.
        const auto alive = ref.alive.lock();
        if (ref.widget != nullptr && alive && *alive)
        {
            ref.widget->handleUnderlyingObjectDeleted();
        }
    }

    delete entry;
}

void Widget::handleUnderlyingObjectDeleted()
{
    lv_base_ptr_    = nullptr;
    registry_entry_ = nullptr;

    // Event descriptors are owned by the deleted lv_obj.
    event_dscs_.clear();
    child_event_dscs_.clear();
    registered_codes_.clear();

    for (auto &[code, slots] : event_handlers_)
    {
        (void)code;
        for (auto &slot : slots)
        {
            if (slot.connection)
            {
                slot.connection->connected     = false;
                slot.connection->on_disconnect = nullptr;
            }
        }
    }
    event_handlers_.clear();

    for (auto &slot : key_handlers_)
    {
        if (slot.connection)
        {
            slot.connection->connected     = false;
            slot.connection->on_disconnect = nullptr;
        }
    }
    key_handlers_.clear();

    for (auto &slot : focus_handlers_)
    {
        if (slot.connection)
        {
            slot.connection->connected     = false;
            slot.connection->on_disconnect = nullptr;
        }
    }
    focus_handlers_.clear();

    notifyDestroyedObservers();
    onLvglObjectDeleted();
}

Widget *Widget::fromLvglObject(lv_obj_t *obj)
{
    if (obj == nullptr)
    {
        return nullptr;
    }
    auto *entry = entryFor(obj);
    if (entry == nullptr || entry->wrappers.empty())
    {
        return nullptr;
    }

    const auto &ref   = entry->wrappers.back();
    const auto  alive = ref.alive.lock();
    return alive && *alive ? ref.widget : nullptr;
}

// ---------------------------------------------------------------- keepalive

void Widget::setObjectKeepalive(const std::string &key, std::shared_ptr<void> data)
{
    if (registry_entry_ != nullptr)
    {
        registry_entry_->keepalive[key] = std::move(data);
    }
}

void Widget::clearObjectKeepalive(const std::string &key)
{
    if (registry_entry_ != nullptr)
    {
        registry_entry_->keepalive.erase(key);
    }
}

// ---------------------------------------------------------------- events

std::shared_ptr<detail::ConnectionState> Widget::makeConnection(std::function<void()> onDisconnect)
{
    auto state           = std::make_shared<detail::ConnectionState>();
    state->on_disconnect = std::move(onDisconnect);
    return state;
}

void Widget::ensureEventHandler(lv_event_code_t code)
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }

    const int key = static_cast<int>(code);
    if (std::find(registered_codes_.begin(), registered_codes_.end(), key) != registered_codes_.end())
    {
        return;
    }

    auto *dsc = lv_obj_add_event_cb(lv_base_ptr_, &Widget::eventTrampoline, code, this);
    if (dsc != nullptr)
    {
        registered_codes_.push_back(key);
        event_dscs_.push_back(dsc);
    }
}

void Widget::eventTrampoline(lv_event_t *e)
{
    auto *self = static_cast<Widget *>(lv_event_get_user_data(e));
    if (self != nullptr)
    {
        self->dispatchLvglEvent(e);
    }
}

void Widget::dispatchLvglEvent(lv_event_t *e)
{
    const auto raw_code = lv_event_get_code(e);
    const auto code     = static_cast<lv_event_code_t>(static_cast<int>(raw_code) & ~static_cast<int>(LV_EVENT_PREPROCESS));

    switch (code)
    {
    case LV_EVENT_CLICKED:
        if (clicked_cb_)
        {
            clicked_cb_();
        }
        break;
    case LV_EVENT_PRESSED:
        if (pressed_cb_)
        {
            pressed_cb_();
        }
        break;
    case LV_EVENT_RELEASED: {
        if (released_cb_)
        {
            released_cb_();
        }

        lv_indev_t *indev = lv_indev_active();
        if (indev != nullptr && indev != sys::InputManager::getInstance().virtualIndev()
            && (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD || lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) && lv_indev_get_key(indev) == LV_KEY_ENTER)
        {
            KeyEvent event{Key::Enter, LV_KEY_ENTER, KeyAction::Release, false};
            pending_key_raw_ = 0;
            dispatchKeyEvent(event);
        }
        break;
    }
    case LV_EVENT_LONG_PRESSED: {
        if (long_pressed_cb_)
        {
            long_pressed_cb_();
        }

        lv_indev_t *indev = lv_indev_active();
        if (indev != nullptr && indev != sys::InputManager::getInstance().virtualIndev()
            && (lv_indev_get_type(indev) == LV_INDEV_TYPE_KEYPAD || lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) && lv_indev_get_key(indev) == LV_KEY_ENTER)
        {
            KeyEvent event{Key::Enter, LV_KEY_ENTER, KeyAction::LongPress, false};
            dispatchKeyEvent(event);
        }
        break;
    }
    case LV_EVENT_KEY:
        handleLvglKeyEvent(e);
        break;
    case LV_EVENT_FOCUSED:
        notifyFocus(true);
        break;
    case LV_EVENT_DEFOCUSED:
        synthReleasePendingKey();
        notifyFocus(false);
        break;
    default:
        break;
    }

    auto it = event_handlers_.find(static_cast<int>(code));
    if (it == event_handlers_.end())
    {
        return;
    }

    // Copy: handlers may disconnect themselves during dispatch.
    auto slots = it->second;
    for (auto &slot : slots)
    {
        if (slot.connection && slot.connection->connected && slot.callback)
        {
            slot.callback(e);
        }
    }
}

Connection Widget::onSimpleEvent(lv_event_code_t code, ClickHandler handler)
{
    ensureEventHandler(code);
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id, code]() {
        if (token && *token)
        {
            removeEventHandler(code, id);
        }
    });

    event_handlers_[static_cast<int>(code)].push_back(EventSlot{id, state, [callback = std::move(handler)](lv_event_t *) {
                                                                   if (callback)
                                                                   {
                                                                       callback();
                                                                   }
                                                               }});
    return Connection(state);
}

Connection Widget::onClicked(ClickHandler handler)
{
    return onSimpleEvent(LV_EVENT_CLICKED, std::move(handler));
}

Connection Widget::onPressed(ClickHandler handler)
{
    return onSimpleEvent(LV_EVENT_PRESSED, std::move(handler));
}

Connection Widget::onReleased(ClickHandler handler)
{
    return onSimpleEvent(LV_EVENT_RELEASED, std::move(handler));
}

Connection Widget::onLongPressed(ClickHandler handler)
{
    return onSimpleEvent(LV_EVENT_LONG_PRESSED, std::move(handler));
}

Connection Widget::onKey(KeyHandler handler)
{
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id]() {
        if (token && *token)
        {
            removeKeyHandler(id);
        }
    });

    key_handlers_.push_back(KeySlot{id, state, std::move(handler)});
    return Connection(state);
}

Connection Widget::onFocus(FocusHandler handler)
{
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id]() {
        if (token && *token)
        {
            removeFocusHandler(id);
        }
    });

    focus_handlers_.push_back(FocusSlot{id, state, std::move(handler)});
    return Connection(state);
}

Connection Widget::onBlur(FocusHandler handler)
{
    return onFocus([callback = std::move(handler)](bool focused) {
        if (!focused && callback)
        {
            callback(false);
        }
    });
}

void Widget::removeKeyHandler(std::uint64_t id)
{
    auto found = std::find_if(key_handlers_.begin(), key_handlers_.end(), [id](const KeySlot &slot) { return slot.id == id; });
    if (found == key_handlers_.end())
    {
        return;
    }

    if (found->connection)
    {
        found->connection->connected     = false;
        found->connection->on_disconnect = nullptr;
    }
    key_handlers_.erase(found);
}

void Widget::removeFocusHandler(std::uint64_t id)
{
    auto found = std::find_if(focus_handlers_.begin(), focus_handlers_.end(), [id](const FocusSlot &slot) { return slot.id == id; });
    if (found == focus_handlers_.end())
    {
        return;
    }

    if (found->connection)
    {
        found->connection->connected     = false;
        found->connection->on_disconnect = nullptr;
    }
    focus_handlers_.erase(found);
}

void Widget::invokeKeyHandlers(KeyEvent &event)
{
    auto slots = key_handlers_;
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
}

void Widget::notifyFocus(bool focused)
{
    auto slots = focus_handlers_;
    for (auto &slot : slots)
    {
        if (slot.connection && slot.connection->connected && slot.callback)
        {
            slot.callback(focused);
        }
    }
}

void Widget::handleLvglKeyEvent(lv_event_t *e)
{
    lv_indev_t *indev = lv_indev_active();
    if (indev == nullptr)
    {
        return;
    }

    // Keys from the virtual indev are dispatched by InputManager when the
    // indev is read; do not dispatch them a second time here.
    if (indev == sys::InputManager::getInstance().virtualIndev())
    {
        return;
    }

    const uint32_t raw = lv_indev_get_key(indev);
    if (raw == 0)
    {
        return;
    }

    const uint32_t now    = lv_tick_get();
    KeyAction      action = KeyAction::Press;

    if (pending_key_raw_ == raw && lv_tick_elaps(pending_key_tick_) < kKeyRepeatMs)
    {
        action = KeyAction::Repeat;
    } else if (pending_key_raw_ != 0 && pending_key_raw_ != raw)
    {
        KeyEvent release{keyFromLvgl(pending_key_raw_), pending_key_raw_, KeyAction::Release, false};
        dispatchKeyEvent(release);
    }

    pending_key_raw_  = raw;
    pending_key_tick_ = now;

    KeyEvent event{keyFromLvgl(raw), raw, action, false};
    dispatchKeyEvent(event);

    if (event.consumed)
    {
        lv_event_stop_processing(e);
    }
}

void Widget::synthReleasePendingKey()
{
    if (pending_key_raw_ == 0)
    {
        return;
    }

    const uint32_t raw = pending_key_raw_;
    pending_key_raw_   = 0;

    KeyEvent event{keyFromLvgl(raw), raw, KeyAction::Release, false};
    dispatchKeyEvent(event);
}

void Widget::dispatchKeyEvent(KeyEvent &event)
{
    Widget *current = this;
    int       guard   = 0;

    while (current != nullptr && !event.consumed && guard++ < kMaxAncestorWalk)
    {
        current->invokeKeyHandlers(event);
        if (event.consumed)
        {
            return;
        }
        current = current->getParent();
    }

    if (!event.consumed)
    {
        sys::InputManager::getInstance().dispatchGlobalKey(event);
    }
}

Widget &Widget::setFocusable(bool focusable)
{
    if (focusable_ == focusable)
    {
        return *this;
    }
    focusable_ = focusable;

    if (lv_base_ptr_ != nullptr)
    {
        if (focusable)
        {
            focus_style_ = std::make_shared<core::Style>(core::Theme::instance().focusStyle());
            applyStyle(*focus_style_, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
        } else if (focus_style_)
        {
            removeStyle(*focus_style_, LV_PART_MAIN | LV_STATE_FOCUS_KEY);
            focus_style_.reset();
        }
    }

    sys::InputManager::getInstance().refreshFor(this);
    return *this;
}

bool Widget::requestFocus()
{
    return sys::InputManager::getInstance().requestFocus(this);
}

bool Widget::hasFocus() const
{
    if (lv_base_ptr_ == nullptr)
    {
        return false;
    }
    lv_group_t *group = lv_obj_get_group(lv_base_ptr_);
    return group != nullptr && lv_group_get_focused(group) == lv_base_ptr_;
}

Connection Widget::onDestroyed(std::function<void()> handler)
{
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id]() {
        if (token && *token)
        {
            removeDestroyHandler(id);
        }
    });

    destroy_handlers_.push_back(DestroySlot{id, state, std::move(handler)});
    return Connection(state);
}

lv_event_dsc_t *Widget::addLvglEventHandler(lv_event_cb_t cb, lv_event_code_t filter, void *userData)
{
    if (lv_base_ptr_ == nullptr || cb == nullptr)
    {
        return nullptr;
    }

    auto *dsc = lv_obj_add_event_cb(lv_base_ptr_, cb, filter, userData);
    if (dsc != nullptr)
    {
        event_dscs_.push_back(dsc);
    }
    return dsc;
}

lv_event_dsc_t *Widget::addChildLvglEventHandler(lv_obj_t *child, lv_event_cb_t cb, lv_event_code_t filter, void *userData)
{
    if (child == nullptr || cb == nullptr)
    {
        return nullptr;
    }

    auto *dsc = lv_obj_add_event_cb(child, cb, filter, userData);
    if (dsc != nullptr)
    {
        child_event_dscs_.emplace_back(child, dsc);
    }
    return dsc;
}

void Widget::removeEventHandler(lv_event_code_t code, std::uint64_t id)
{
    auto it = event_handlers_.find(static_cast<int>(code));
    if (it == event_handlers_.end())
    {
        return;
    }

    auto &slots = it->second;
    auto  found = std::find_if(slots.begin(), slots.end(), [id](const EventSlot &slot) { return slot.id == id; });
    if (found == slots.end())
    {
        return;
    }

    if (found->connection)
    {
        found->connection->connected     = false;
        found->connection->on_disconnect = nullptr;
    }
    slots.erase(found);

    if (slots.empty())
    {
        event_handlers_.erase(it);
    }
}

void Widget::removeDestroyHandler(std::uint64_t id)
{
    auto found = std::find_if(destroy_handlers_.begin(), destroy_handlers_.end(), [id](const DestroySlot &slot) { return slot.id == id; });
    if (found == destroy_handlers_.end())
    {
        return;
    }

    if (found->connection)
    {
        found->connection->connected     = false;
        found->connection->on_disconnect = nullptr;
    }
    destroy_handlers_.erase(found);
}

void Widget::notifyDestroyedObservers()
{
    if (destroy_notified_)
    {
        return;
    }
    destroy_notified_ = true;

    auto slots = destroy_handlers_;
    for (auto &slot : slots)
    {
        if (slot.connection && slot.connection->connected && slot.callback)
        {
            slot.callback();
        }
    }

    for (auto &slot : destroy_handlers_)
    {
        if (slot.connection)
        {
            slot.connection->connected     = false;
            slot.connection->on_disconnect = nullptr;
        }
    }
    destroy_handlers_.clear();
}

void Widget::detachEventCallbacks()
{
    if (lv_base_ptr_ != nullptr)
    {
        for (auto *dsc : event_dscs_)
        {
            if (dsc != nullptr)
            {
                lv_obj_remove_event_dsc(lv_base_ptr_, dsc);
            }
        }
        event_dscs_.clear();

        // Defensive sweep: remove any callback that was registered directly with
        // this wrapper as user data (e.g. by older application code).
        for (uint32_t i = lv_obj_get_event_count(lv_base_ptr_); i > 0; --i)
        {
            auto *dsc = lv_obj_get_event_dsc(lv_base_ptr_, i - 1);
            if (dsc != nullptr && lv_event_dsc_get_user_data(dsc) == this)
            {
                lv_obj_remove_event_dsc(lv_base_ptr_, dsc);
            }
        }
    } else
    {
        event_dscs_.clear();
    }

    for (auto &[child, dsc] : child_event_dscs_)
    {
        if (child != nullptr && dsc != nullptr && lv_obj_is_valid(child))
        {
            lv_obj_remove_event_dsc(child, dsc);
        }
    }
    child_event_dscs_.clear();
    registered_codes_.clear();

    for (auto &[code, slots] : event_handlers_)
    {
        (void)code;
        for (auto &slot : slots)
        {
            if (slot.connection)
            {
                slot.connection->connected     = false;
                slot.connection->on_disconnect = nullptr;
            }
        }
    }
    event_handlers_.clear();

    for (auto &slot : key_handlers_)
    {
        if (slot.connection)
        {
            slot.connection->connected     = false;
            slot.connection->on_disconnect = nullptr;
        }
    }
    key_handlers_.clear();

    for (auto &slot : focus_handlers_)
    {
        if (slot.connection)
        {
            slot.connection->connected     = false;
            slot.connection->on_disconnect = nullptr;
        }
    }
    focus_handlers_.clear();
}

void Widget::setOnClickedListener(const std::function<void()> &l)
{
    clicked_cb_ = l;
}

void Widget::setOnPressedListener(const std::function<void()> &l)
{
    pressed_cb_ = l;
}

void Widget::setOnReleasedListener(const std::function<void()> &l)
{
    released_cb_ = l;
}

void Widget::setOnLongPressedListener(const std::function<void()> &l)
{
    long_pressed_cb_ = l;
}

// ---------------------------------------------------------------- geometry

Widget &Widget::setPos(lv_coord_t x, lv_coord_t y)
{
    if (lv_base_ptr_ != nullptr)
    {
#if LVGLPP_ENABLE_RTL
        if (mirror_in_rtl_ && sys::UiConfig::instance().isRtl())
        {
            lv_obj_t  *parent = lv_obj_get_parent(lv_base_ptr_);
            lv_coord_t width  = lv_obj_get_width(lv_base_ptr_);
            if (width <= 0 && requested_width_ > 0 && requested_width_ < kMaxRequestedWidthSanity)
            {
                width = requested_width_;
            }
            lv_coord_t parentWidth = parent != nullptr ? lv_obj_get_content_width(parent) : 0;
            x                      = parentWidth - x - width;
        }
#endif
        // An explicit position overrides a previous alignment request.
        // Use TOP_LEFT (physical) so RTL's DEFAULT -> TOP_RIGHT translation
        // does not add the parent width a second time.
        lv_obj_set_style_align(lv_base_ptr_, LV_ALIGN_TOP_LEFT, 0);
#if LVGLPP_ENABLE_RTL
        has_logical_align_ = false;
#endif
        lv_obj_set_pos(lv_base_ptr_, x, y);
    }
    return *this;
}

Widget &Widget::setPosPhysical(lv_coord_t x, lv_coord_t y)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_align(lv_base_ptr_, LV_ALIGN_TOP_LEFT, 0);
#if LVGLPP_ENABLE_RTL
        has_logical_align_ = false;
#endif
        lv_obj_set_pos(lv_base_ptr_, x, y);
    }
    return *this;
}

lv_coord_t Widget::getX() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_x(lv_base_ptr_) : 0;
}

lv_coord_t Widget::getY() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_y(lv_base_ptr_) : 0;
}

Widget &Widget::setAlignment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
{
#if LVGLPP_ENABLE_RTL
    logical_align_     = align;
    logical_x_         = offsetX;
    logical_y_         = offsetY;
    has_logical_align_ = true;
    applyLogicalAlignment();
#else
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_align(lv_base_ptr_, align, offsetX, offsetY);
    }
#endif
    return *this;
}

Widget &Widget::setAlignmentPhysical(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
{
    if (lv_base_ptr_ != nullptr)
    {
#if LVGLPP_ENABLE_RTL
        has_logical_align_ = false;
#endif
        lv_obj_align(lv_base_ptr_, align, offsetX, offsetY);
    }
    return *this;
}

Widget &Widget::setMirrorInRtl(bool enabled)
{
#if LVGLPP_ENABLE_RTL
    mirror_in_rtl_ = enabled;
    if (enabled && has_logical_align_)
    {
        applyLogicalAlignment();
    }
#else
    (void)enabled;
#endif
    return *this;
}

bool Widget::mirrorInRtl() const
{
#if LVGLPP_ENABLE_RTL
    return mirror_in_rtl_;
#else
    return false;
#endif
}

Widget &Widget::setBaseDir(lv_base_dir_t direction)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_base_dir(lv_base_ptr_, direction, 0);
    }
    return *this;
}

void Widget::applyLogicalAlignment()
{
#if LVGLPP_ENABLE_RTL
    if (lv_base_ptr_ == nullptr || !has_logical_align_)
    {
        return;
    }

    lv_align_t align   = logical_align_;
    lv_coord_t offsetX = logical_x_;

    if (mirror_in_rtl_ && sys::UiConfig::instance().isRtl())
    {
        align   = mirrorAlignForRtl(align);
        offsetX = -offsetX;
    }

    lv_obj_align(lv_base_ptr_, align, offsetX, logical_y_);
#endif
}

Widget &Widget::setAlignmentTo(const Widget &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
{
    if (lv_base_ptr_ != nullptr && item.lvObj() != nullptr)
    {
#if LVGLPP_ENABLE_RTL
        if (mirror_in_rtl_ && sys::UiConfig::instance().isRtl())
        {
            align   = mirrorAlignForRtl(align);
            offsetX = -offsetX;
        }
#endif
        lv_obj_align_to(lv_base_ptr_, item.lvObj(), align, offsetX, offsetY);
    }
    return *this;
}

Widget &Widget::setSize(lv_coord_t width, lv_coord_t height)
{
    if (lv_base_ptr_ != nullptr)
    {
        requested_width_  = width;
        requested_height_ = height;
        lv_obj_set_size(lv_base_ptr_, width, height);
    }
    return *this;
}

Widget &Widget::setWidth(lv_coord_t w)
{
    if (lv_base_ptr_ != nullptr)
    {
        requested_width_ = w;
        lv_obj_set_width(lv_base_ptr_, w);
    }
    return *this;
}

Widget &Widget::setHeight(lv_coord_t h)
{
    if (lv_base_ptr_ != nullptr)
    {
        requested_height_ = h;
        lv_obj_set_height(lv_base_ptr_, h);
    }
    return *this;
}

lv_coord_t Widget::getWidth() const
{
    if (lv_base_ptr_ != nullptr)
    {
        const lv_coord_t width = lv_obj_get_width(lv_base_ptr_);
        if (width != 0)
        {
            return width;
        }
    }
    return requested_width_;
}

lv_coord_t Widget::getHeight() const
{
    if (lv_base_ptr_ != nullptr)
    {
        const lv_coord_t height = lv_obj_get_height(lv_base_ptr_);
        if (height != 0)
        {
            return height;
        }
    }
    return requested_height_;
}

Widget &Widget::setSizePercent(lv_coord_t widthPercent, lv_coord_t heightPercent)
{
    if (lv_base_ptr_ != nullptr)
    {
        requested_width_  = 0;
        requested_height_ = 0;
        lv_obj_set_size(lv_base_ptr_, lv_pct(widthPercent), lv_pct(heightPercent));
    }
    return *this;
}

// ---------------------------------------------------------------- appearance

Widget &Widget::setBgColor(uint32_t clr)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_bg_color(lv_base_ptr_, lv_color_hex(clr), LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setDisabledBgColor(uint32_t clr)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_bg_color(lv_base_ptr_, lv_color_hex(clr), LV_STATE_DISABLED);
    }
    return *this;
}

Widget &Widget::setBgOpacity(lv_opa_t opa)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_bg_opa(lv_base_ptr_, opa, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setOpacity(double opa)
{
    if (lv_base_ptr_ != nullptr)
    {
        const double clamped = std::clamp(opa, 0.0, 1.0);
        lv_obj_set_style_opa(lv_base_ptr_, static_cast<lv_opa_t>(LV_OPA_COVER * clamped), LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setRadius(lv_coord_t radius)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_radius(lv_base_ptr_, radius, LV_STATE_DEFAULT);
    }
    return *this;
}

lv_coord_t Widget::getRadius() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_style_radius(lv_base_ptr_, LV_PART_MAIN) : 0;
}

Widget &Widget::setClip(bool clip)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_clip_corner(lv_base_ptr_, clip, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setBorder(lv_coord_t width, uint32_t color)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_border_width(lv_base_ptr_, width, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(lv_base_ptr_, lv_color_hex(color), LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::applyStyle(const Style &style, lv_style_selector_t selector)
{
    if (lv_base_ptr_ == nullptr)
    {
        return *this;
    }

    style.applyTo(lv_base_ptr_, selector);

    // Keep the shared style state alive for as long as the lv_obj exists.
    auto holder = std::make_shared<Style>(style);
    setObjectKeepalive("style." + std::to_string(next_style_id_++), holder);
    return *this;
}

Widget &Widget::removeStyle(const Style &style, lv_style_selector_t selector)
{
    if (lv_base_ptr_ == nullptr)
    {
        return *this;
    }

    lv_obj_remove_style(lv_base_ptr_, style.raw(), selector);

    if (registry_entry_ != nullptr)
    {
        for (auto it = registry_entry_->keepalive.begin(); it != registry_entry_->keepalive.end();)
        {
            if (it->first.rfind("style.", 0) == 0)
            {
                auto *stored = static_cast<Style *>(it->second.get());
                if (stored != nullptr && stored->raw() == style.raw())
                {
                    it = registry_entry_->keepalive.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }
    return *this;
}

Widget &Widget::setState(lv_state_t state, bool enabled)
{
    if (lv_base_ptr_ != nullptr)
    {
        if (enabled)
        {
            lv_obj_add_state(lv_base_ptr_, state);
        } else
        {
            lv_obj_clear_state(lv_base_ptr_, state);
        }
    }
    return *this;
}

Widget &Widget::addState(lv_state_t state)
{
    return setState(state, true);
}

Widget &Widget::clearState(lv_state_t state)
{
    return setState(state, false);
}

void Widget::setVisible(bool visible)
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }
    if (visible)
    {
        lv_obj_clear_flag(lv_base_ptr_, LV_OBJ_FLAG_HIDDEN);
    } else
    {
        lv_obj_add_flag(lv_base_ptr_, LV_OBJ_FLAG_HIDDEN);
    }
}

bool Widget::isVisible() const
{
    return lv_base_ptr_ != nullptr && !lv_obj_has_flag(lv_base_ptr_, LV_OBJ_FLAG_HIDDEN);
}

void Widget::setEnabled(bool enable)
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }

    // LVGL's pointer/keypad indev already ignores LV_STATE_DISABLED objects.
    // Do not touch LV_OBJ_FLAG_CLICKABLE here: "enabled" and "clickable" are
    // independent properties (images, for example, are non-clickable by
    // default but become click targets when explicitly enabled).
    lv_obj_set_state(lv_base_ptr_, LV_STATE_DISABLED, !enable);
}

bool Widget::isEnabled() const
{
    return lv_base_ptr_ != nullptr && !lv_obj_has_state(lv_base_ptr_, LV_STATE_DISABLED);
}

// ---------------------------------------------------------------- padding / margin

Widget &Widget::setPaddingAll(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_all(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingHor(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_hor(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingVer(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_ver(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingTop(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_top(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingBottom(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_bottom(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingLeft(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_left(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingRight(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_right(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingRow(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_row(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setPaddingColumn(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_column(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setMarginAll(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_top(lv_base_ptr_, margin, LV_STATE_DEFAULT);
        lv_obj_set_style_margin_bottom(lv_base_ptr_, margin, LV_STATE_DEFAULT);
        lv_obj_set_style_margin_left(lv_base_ptr_, margin, LV_STATE_DEFAULT);
        lv_obj_set_style_margin_right(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setMarginHor(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_left(lv_base_ptr_, margin, LV_STATE_DEFAULT);
        lv_obj_set_style_margin_right(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setMarginVer(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_top(lv_base_ptr_, margin, LV_STATE_DEFAULT);
        lv_obj_set_style_margin_bottom(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setMarginTop(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_top(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setMarginBottom(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_bottom(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setMarginLeft(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_left(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

Widget &Widget::setMarginRight(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_right(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

// ---------------------------------------------------------------- tree

Widget *Widget::getParent() const
{
    if (lv_base_ptr_ == nullptr)
    {
        return nullptr;
    }

    lv_obj_t *parent = lv_obj_get_parent(lv_base_ptr_);
    while (parent != nullptr)
    {
        if (Widget *wrapper = fromLvglObject(parent); wrapper != nullptr)
        {
            return wrapper;
        }
        parent = lv_obj_get_parent(parent);
    }
    return nullptr;
}

void Widget::setParent(Widget *parent)
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }

    if (parent != nullptr && parent->lvObj() != nullptr)
    {
        lv_obj_set_parent(lv_base_ptr_, parent->lvObj());
        owns_lv_obj_ = false;
        return;
    }

    if (lv_obj_t *screen = lv_scr_act(); screen != nullptr)
    {
        lv_obj_set_parent(lv_base_ptr_, screen);
        owns_lv_obj_ = true;
    }
}

// ---------------------------------------------------------------- introspection

uint32_t Widget::childCount() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_child_count(lv_base_ptr_) : 0;
}

Widget *Widget::childAt(uint32_t index) const
{
    if (lv_base_ptr_ == nullptr)
    {
        return nullptr;
    }
    return fromLvglObject(lv_obj_get_child(lv_base_ptr_, static_cast<int32_t>(index)));
}

Widget *Widget::findChildInRawTree(lv_obj_t *obj, const std::string &name)
{
    if (obj == nullptr)
    {
        return nullptr;
    }

    if (Widget *wrapper = fromLvglObject(obj); wrapper != nullptr && wrapper->name() == name)
    {
        return wrapper;
    }

    const uint32_t count = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < count; ++i)
    {
        if (Widget *found = findChildInRawTree(lv_obj_get_child(obj, static_cast<int32_t>(i)), name); found != nullptr)
        {
            return found;
        }
    }
    return nullptr;
}

Widget *Widget::findChild(const std::string &n) const
{
    return findChildInRawTree(lv_base_ptr_, n);
}

void Widget::collectWrapperSubtree(lv_obj_t *obj, std::vector<Widget *> &out)
{
    if (obj == nullptr)
    {
        return;
    }

    if (Widget *wrapper = fromLvglObject(obj); wrapper != nullptr)
    {
        out.push_back(wrapper);
    }

    const uint32_t count = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < count; ++i)
    {
        collectWrapperSubtree(lv_obj_get_child(obj, static_cast<int32_t>(i)), out);
    }
}

void Widget::dumpTree(int depth) const
{
    if (lv_base_ptr_ == nullptr)
    {
        std::cout << std::string(static_cast<size_t>(depth) * 2, ' ') << "(detached)" << std::endl;
        return;
    }

    std::function<void(lv_obj_t *, int)> dump = [&](lv_obj_t *obj, int level) {
        const std::string indent(static_cast<size_t>(level) * 2, ' ');
        if (Widget *wrapper = fromLvglObject(obj); wrapper != nullptr)
        {
            std::cout << indent << (wrapper->name().empty() ? "(unnamed)" : wrapper->name()) << " [" << typeid(*wrapper).name() << "]"
                      << " visible=" << wrapper->isVisible() << " pos=(" << wrapper->getX() << "," << wrapper->getY() << ")"
                      << " size=(" << lv_obj_get_width(obj) << "," << lv_obj_get_height(obj) << ")" << std::endl;
        } else
        {
            std::cout << indent << "(unwrapped) size=(" << lv_obj_get_width(obj) << "," << lv_obj_get_height(obj) << ")" << std::endl;
        }

        const uint32_t count = lv_obj_get_child_count(obj);
        for (uint32_t i = 0; i < count; ++i)
        {
            dump(lv_obj_get_child(obj, static_cast<int32_t>(i)), level + 1);
        }
    };

    dump(lv_base_ptr_, depth);
}

std::string Widget::describeState() const
{
    if (lv_base_ptr_ == nullptr)
    {
        return R"({"alive":false})";
    }

    const Rect         r = bounds();
    std::ostringstream os;
    os << "{"
       << R"("name":")" << jsonEscape(_name) << "\","
       << R"("type":")" << jsonEscape(typeid(*this).name()) << "\","
       << R"("visible":)" << (isVisible() ? "true" : "false") << ","
       << R"("enabled":)" << (isEnabled() ? "true" : "false") << ","
       << R"("rect":{"x":)" << r.x << ",\"y\":" << r.y << ",\"w\":" << r.w << ",\"h\":" << r.h << "},"
       << R"("children":)" << childCount() << "}";
    return os.str();
}

Widget::Rect Widget::bounds() const
{
    if (lv_base_ptr_ == nullptr)
    {
        return {0, 0, 0, 0};
    }

    lv_area_t area{};
    lv_obj_get_coords(lv_base_ptr_, &area);
    return {area.x1, area.y1, lv_area_get_width(&area), lv_area_get_height(&area)};
}

// ---------------------------------------------------------------- flex

Widget &Widget::setFlexFlow(lv_flex_flow_t flow)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_flex_flow(lv_base_ptr_, flow);
    }
    return *this;
}

Widget &Widget::setFlexAlign(lv_flex_align_t mainAxis, lv_flex_align_t crossAxis, lv_flex_align_t trackAlign)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_flex_align(lv_base_ptr_, mainAxis, crossAxis, trackAlign);
    }
    return *this;
}

Widget &Widget::setFlexGrow(uint8_t grow)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_flex_grow(lv_base_ptr_, grow);
    }
    return *this;
}

// ---------------------------------------------------------------- grid

Widget::GridState &Widget::ensureGridState()
{
    if (!grid_)
    {
        grid_ = std::make_shared<GridState>();
    }
    return *grid_;
}

void Widget::applyGridTemplates()
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }

    static const lv_coord_t default_track[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    GridState        &grid = ensureGridState();
    const lv_coord_t *cols = (grid.cols.empty() || grid.cols.back() != LV_GRID_TEMPLATE_LAST) ? default_track : grid.cols.data();
    const lv_coord_t *rows = (grid.rows.empty() || grid.rows.back() != LV_GRID_TEMPLATE_LAST) ? default_track : grid.rows.data();

    lv_obj_set_grid_dsc_array(lv_base_ptr_, cols, rows);

    // The arrays are referenced by LVGL until the object is deleted. Keep them
    // alive even if this wrapper is destroyed first.
    setObjectKeepalive("grid", grid_);
}

void Widget::applyGridCell()
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }
    GridState &grid = ensureGridState();
    lv_obj_set_grid_cell(lv_base_ptr_, grid.col_align, static_cast<int32_t>(grid.col_pos), static_cast<int32_t>(grid.col_span), grid.row_align, static_cast<int32_t>(grid.row_pos),
                         static_cast<int32_t>(grid.row_span));
}

Widget &Widget::setGridColumns(const std::string &columnDescriptors)
{
    std::vector<lv_coord_t> tracks;
    std::string             error;
    if (!GridLayout::parseTracks(columnDescriptors, tracks, &error))
    {
        LOG_WARN() << "invalid grid column descriptors: " << error;
        return *this;
    }

    ensureGridState().cols = std::move(tracks);
    applyGridTemplates();
    return *this;
}

Widget &Widget::setGridRows(const std::string &rowDescriptors)
{
    std::vector<lv_coord_t> tracks;
    std::string             error;
    if (!GridLayout::parseTracks(rowDescriptors, tracks, &error))
    {
        LOG_WARN() << "invalid grid row descriptors: " << error;
        return *this;
    }

    ensureGridState().rows = std::move(tracks);
    applyGridTemplates();
    return *this;
}

Widget &Widget::setGridCell(uint32_t column, uint32_t row, uint32_t colSpan, uint32_t rowSpan)
{
    GridState &grid = ensureGridState();
    grid.col_pos    = column;
    grid.row_pos    = row;
    grid.col_span   = colSpan == 0 ? 1 : colSpan;
    grid.row_span   = rowSpan == 0 ? 1 : rowSpan;
    applyGridCell();
    return *this;
}

Widget &Widget::setGridCellColumnAlign(lv_grid_align_t align)
{
    ensureGridState().col_align = align;
    applyGridCell();
    return *this;
}

Widget &Widget::setGridCellRowAlign(lv_grid_align_t align)
{
    ensureGridState().row_align = align;
    applyGridCell();
    return *this;
}

} // namespace lvglpp::widgets
