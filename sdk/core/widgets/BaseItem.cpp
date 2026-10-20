#include "BaseItem.h"

#include "GridLayout.h"
#include "core/base/LvObjFactory.h"
#include "core/style/Style.h"
#include "core/style/Theme.h"
#include "core/sys/InputManager.h"

#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <unordered_map>
#include <utility>

namespace lvglpp::widgets {

/**
 * One registry entry per lv_obj. It survives wrapper destruction and is
 * released by the LV_EVENT_DELETE watchdog. Because it can outlive the
 * wrapper it also owns "keepalive" data that LVGL stores by raw pointer
 * (grid templates, image source strings, styles).
 */
struct BaseItemRegistryEntry {
    lv_obj_t                                              *obj     = nullptr;
    BaseItem                                              *wrapper = nullptr;
    std::unordered_map<std::string, std::shared_ptr<void>> keepalive;
};

namespace {

using Registry = std::unordered_map<lv_obj_t *, BaseItemRegistryEntry *>;

Registry &registry()
{
    static Registry map;
    return map;
}

BaseItemRegistryEntry *entryFor(lv_obj_t *obj)
{
    auto it = registry().find(obj);
    return it == registry().end() ? nullptr : it->second;
}

} // namespace

// ---------------------------------------------------------------- lifetime

BaseItem::BaseItem(BaseItem *parentItem) : BaseItem(tools::LvObjFactory::createLvglItem, parentItem)
{}

BaseItem::BaseItem(Creator creator, BaseItem *parentItem)
{
    initItem(creator, parentItem);
}

BaseItem::BaseItem(AdoptExistingTag, lv_obj_t *existing, BaseItem *logicalParent, bool takeOwnership)
{
    (void)logicalParent;
    owns_lv_obj_ = takeOwnership;
    attachToLvglObject(existing);
}

BaseItem::~BaseItem()
{
    destroying_ = true;
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
        // The entry may outlive the wrapper; the watchdog will release it.
        registry_entry_->wrapper = nullptr;
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

void BaseItem::initItem(const Creator &creator, BaseItem *parentItem)
{
    lv_obj_t *expected_parent  = lv_scr_act();
    BaseItem *effective_parent = parentItem;

    if (effective_parent != nullptr)
    {
        expected_parent = effective_parent->getLvglItem();
        if (expected_parent == nullptr)
        {
            LOG_WARN() << "BaseItem parent has no lv_obj; attaching to the active screen";
            effective_parent = nullptr;
            expected_parent  = lv_scr_act();
        }
    }

    if (!creator)
    {
        LOG_ERROR() << "BaseItem created without a creator";
        return;
    }

    lv_obj_t *obj = creator(expected_parent);
    if (obj == nullptr)
    {
        LOG_ERROR() << "BaseItem creator returned nullptr";
        return;
    }

    // A creator may ignore `expected_parent` and attach to lv_layer_top()/sys().
    // Such objects are not owned by the LVGL parent we expected, so the wrapper
    // must delete them.
    owns_lv_obj_ = (effective_parent == nullptr) || (lv_obj_get_parent(obj) != expected_parent);
    attachToLvglObject(obj);
}

void BaseItem::attachToLvglObject(lv_obj_t *obj)
{
    lv_base_ptr_ = obj;
    if (obj == nullptr)
    {
        return;
    }

    auto *entry     = new BaseItemRegistryEntry();
    entry->obj      = obj;
    entry->wrapper  = this;
    registry_entry_ = entry;

    auto existing = registry().find(obj);
    if (existing != registry().end() && existing->second != nullptr && existing->second->wrapper != nullptr)
    {
        LOG_WARN() << "multiple BaseItem wrappers for one lv_obj; the newest is canonical";
    }
    registry()[obj] = entry;

    // The watchdog is registered with the Entry as user data (never `this`), so
    // the wrapper destructor's callback sweep cannot remove it.
    lv_obj_add_event_cb(obj, &BaseItem::objectDeleteWatchdog, LV_EVENT_DELETE, entry);

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

void BaseItem::objectDeleteWatchdog(lv_event_t *e)
{
    auto *entry = static_cast<BaseItemRegistryEntry *>(lv_event_get_user_data(e));
    if (entry == nullptr)
    {
        return;
    }

    BaseItem *wrapper = entry->wrapper;
    entry->wrapper    = nullptr;
    registry().erase(entry->obj);

    if (wrapper != nullptr)
    {
        wrapper->handleUnderlyingObjectDeleted();
    }

    delete entry;
}

void BaseItem::handleUnderlyingObjectDeleted()
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

BaseItem *BaseItem::fromLvglObject(lv_obj_t *obj)
{
    if (obj == nullptr)
    {
        return nullptr;
    }
    auto *entry = entryFor(obj);
    return entry == nullptr ? nullptr : entry->wrapper;
}

void BaseItem::setLvglItem(lv_obj_t *ptr)
{
    lv_base_ptr_ = ptr;
}

// ---------------------------------------------------------------- keepalive

void BaseItem::setObjectKeepalive(const std::string &key, std::shared_ptr<void> data)
{
    if (registry_entry_ != nullptr)
    {
        registry_entry_->keepalive[key] = std::move(data);
    }
}

void BaseItem::clearObjectKeepalive(const std::string &key)
{
    if (registry_entry_ != nullptr)
    {
        registry_entry_->keepalive.erase(key);
    }
}

// ---------------------------------------------------------------- events

std::shared_ptr<detail::ConnectionState> BaseItem::makeConnection(std::function<void()> onDisconnect)
{
    auto state           = std::make_shared<detail::ConnectionState>();
    state->on_disconnect = std::move(onDisconnect);
    return state;
}

void BaseItem::ensureEventHandler(lv_event_code_t code)
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

    auto *dsc = lv_obj_add_event_cb(lv_base_ptr_, &BaseItem::eventTrampoline, code, this);
    if (dsc != nullptr)
    {
        registered_codes_.push_back(key);
        event_dscs_.push_back(dsc);
    }
}

void BaseItem::eventTrampoline(lv_event_t *e)
{
    auto *self = static_cast<BaseItem *>(lv_event_get_user_data(e));
    if (self != nullptr)
    {
        self->dispatchLvglEvent(e);
    }
}

void BaseItem::dispatchLvglEvent(lv_event_t *e)
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

Connection BaseItem::onClicked(ClickHandler handler)
{
    ensureEventHandler(LV_EVENT_CLICKED);
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id]() {
        if (token && *token)
        {
            removeEventHandler(LV_EVENT_CLICKED, id);
        }
    });

    event_handlers_[static_cast<int>(LV_EVENT_CLICKED)].push_back(EventSlot{id, state, [callback = std::move(handler)](lv_event_t *) {
                                                                                if (callback)
                                                                                {
                                                                                    callback();
                                                                                }
                                                                            }});
    return Connection(state);
}

Connection BaseItem::onPressed(ClickHandler handler)
{
    ensureEventHandler(LV_EVENT_PRESSED);
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id]() {
        if (token && *token)
        {
            removeEventHandler(LV_EVENT_PRESSED, id);
        }
    });

    event_handlers_[static_cast<int>(LV_EVENT_PRESSED)].push_back(EventSlot{id, state, [callback = std::move(handler)](lv_event_t *) {
                                                                                if (callback)
                                                                                {
                                                                                    callback();
                                                                                }
                                                                            }});
    return Connection(state);
}

Connection BaseItem::onReleased(ClickHandler handler)
{
    ensureEventHandler(LV_EVENT_RELEASED);
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id]() {
        if (token && *token)
        {
            removeEventHandler(LV_EVENT_RELEASED, id);
        }
    });

    event_handlers_[static_cast<int>(LV_EVENT_RELEASED)].push_back(EventSlot{id, state, [callback = std::move(handler)](lv_event_t *) {
                                                                                 if (callback)
                                                                                 {
                                                                                     callback();
                                                                                 }
                                                                             }});
    return Connection(state);
}

Connection BaseItem::onLongPressed(ClickHandler handler)
{
    ensureEventHandler(LV_EVENT_LONG_PRESSED);
    const auto id    = next_slot_id_++;
    auto       state = makeConnection([this, token = alive_token_, id]() {
        if (token && *token)
        {
            removeEventHandler(LV_EVENT_LONG_PRESSED, id);
        }
    });

    event_handlers_[static_cast<int>(LV_EVENT_LONG_PRESSED)].push_back(EventSlot{id, state, [callback = std::move(handler)](lv_event_t *) {
                                                                                     if (callback)
                                                                                     {
                                                                                         callback();
                                                                                     }
                                                                                 }});
    return Connection(state);
}

Connection BaseItem::onKey(KeyHandler handler)
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

Connection BaseItem::onFocus(FocusHandler handler)
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

Connection BaseItem::onBlur(FocusHandler handler)
{
    return onFocus([callback = std::move(handler)](bool focused) {
        if (!focused && callback)
        {
            callback(false);
        }
    });
}

void BaseItem::removeKeyHandler(std::uint64_t id)
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

void BaseItem::removeFocusHandler(std::uint64_t id)
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

void BaseItem::invokeKeyHandlers(KeyEvent &event)
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

void BaseItem::notifyFocus(bool focused)
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

void BaseItem::handleLvglKeyEvent(lv_event_t *e)
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

    if (pending_key_raw_ == raw && lv_tick_elaps(pending_key_tick_) < 250)
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

void BaseItem::synthReleasePendingKey()
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

void BaseItem::dispatchKeyEvent(KeyEvent &event)
{
    BaseItem *current = this;
    int       guard   = 0;

    while (current != nullptr && !event.consumed && guard++ < 64)
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

BaseItem &BaseItem::setFocusable(bool focusable)
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

bool BaseItem::requestFocus()
{
    return sys::InputManager::getInstance().requestFocus(this);
}

bool BaseItem::hasFocus() const
{
    if (lv_base_ptr_ == nullptr)
    {
        return false;
    }
    lv_group_t *group = lv_obj_get_group(lv_base_ptr_);
    return group != nullptr && lv_group_get_focused(group) == lv_base_ptr_;
}

Connection BaseItem::onDestroyed(std::function<void()> handler)
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

lv_event_dsc_t *BaseItem::addLvglEventHandler(lv_event_cb_t cb, lv_event_code_t filter, void *userData)
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

lv_event_dsc_t *BaseItem::addChildLvglEventHandler(lv_obj_t *child, lv_event_cb_t cb, lv_event_code_t filter, void *userData)
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

void BaseItem::removeEventHandler(lv_event_code_t code, std::uint64_t id)
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

void BaseItem::removeDestroyHandler(std::uint64_t id)
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

void BaseItem::notifyDestroyedObservers()
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

void BaseItem::detachEventCallbacks()
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

void BaseItem::setOnClickedListener(const std::function<void()> &l)
{
    clicked_cb_ = l;
}

void BaseItem::setOnPressedListener(const std::function<void()> &l)
{
    pressed_cb_ = l;
}

void BaseItem::setOnReleasedListener(const std::function<void()> &l)
{
    released_cb_ = l;
}

void BaseItem::setOnLongPressedListener(const std::function<void()> &l)
{
    long_pressed_cb_ = l;
}

// ---------------------------------------------------------------- geometry

BaseItem &BaseItem::setPos(lv_coord_t x, lv_coord_t y)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_pos(lv_base_ptr_, x, y);
    }
    return *this;
}

lv_coord_t BaseItem::getX() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_x(lv_base_ptr_) : 0;
}

lv_coord_t BaseItem::getY() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_y(lv_base_ptr_) : 0;
}

BaseItem &BaseItem::setAlignment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_align(lv_base_ptr_, align, offsetX, offsetY);
    }
    return *this;
}

BaseItem &BaseItem::setAlignmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
{
    if (lv_base_ptr_ != nullptr && item.getLvglItem() != nullptr)
    {
        lv_obj_align_to(lv_base_ptr_, item.getLvglItem(), align, offsetX, offsetY);
    }
    return *this;
}

BaseItem &BaseItem::setSize(lv_coord_t width, lv_coord_t height)
{
    if (lv_base_ptr_ != nullptr)
    {
        requested_width_  = width;
        requested_height_ = height;
        lv_obj_set_size(lv_base_ptr_, width, height);
    }
    return *this;
}

BaseItem &BaseItem::setWidth(lv_coord_t w)
{
    if (lv_base_ptr_ != nullptr)
    {
        requested_width_ = w;
        lv_obj_set_width(lv_base_ptr_, w);
    }
    return *this;
}

BaseItem &BaseItem::setHeight(lv_coord_t h)
{
    if (lv_base_ptr_ != nullptr)
    {
        requested_height_ = h;
        lv_obj_set_height(lv_base_ptr_, h);
    }
    return *this;
}

lv_coord_t BaseItem::getWidth() const
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

lv_coord_t BaseItem::getHeight() const
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

BaseItem &BaseItem::setSizePercent(lv_coord_t widthPercent, lv_coord_t heightPercent)
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

BaseItem &BaseItem::setBgColor(uint32_t clr)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_bg_color(lv_base_ptr_, lv_color_hex(clr), LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setDisableBgColor(uint32_t clr)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_bg_color(lv_base_ptr_, lv_color_hex(clr), LV_STATE_DISABLED);
    }
    return *this;
}

BaseItem &BaseItem::setBgOpacity(lv_opa_t opa)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_bg_opa(lv_base_ptr_, opa, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setOpacity(double opa)
{
    if (lv_base_ptr_ != nullptr)
    {
        const double clamped = std::clamp(opa, 0.0, 1.0);
        lv_obj_set_style_opa(lv_base_ptr_, static_cast<lv_opa_t>(LV_OPA_COVER * clamped), LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setRadius(lv_coord_t radius)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_radius(lv_base_ptr_, radius, LV_STATE_DEFAULT);
    }
    return *this;
}

lv_coord_t BaseItem::getRadius() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_style_radius(lv_base_ptr_, LV_PART_MAIN) : 0;
}

BaseItem &BaseItem::setClip(bool clip)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_clip_corner(lv_base_ptr_, clip, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setBorder(lv_coord_t width, uint32_t color)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_border_width(lv_base_ptr_, width, LV_STATE_DEFAULT);
        lv_obj_set_style_border_color(lv_base_ptr_, lv_color_hex(color), LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::applyStyle(const Style &style, lv_style_selector_t selector)
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

BaseItem &BaseItem::removeStyle(const Style &style, lv_style_selector_t selector)
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

BaseItem &BaseItem::setState(lv_state_t state, bool enabled)
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

BaseItem &BaseItem::addState(lv_state_t state)
{
    return setState(state, true);
}

BaseItem &BaseItem::clearState(lv_state_t state)
{
    return setState(state, false);
}

void BaseItem::setVisible(bool visible)
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

bool BaseItem::getVisible() const
{
    return lv_base_ptr_ != nullptr && !lv_obj_has_flag(lv_base_ptr_, LV_OBJ_FLAG_HIDDEN);
}

void BaseItem::setEnable(bool enable)
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }
    if (enable)
    {
        lv_obj_add_flag(lv_base_ptr_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_state(lv_base_ptr_, LV_STATE_DISABLED);
    } else
    {
        lv_obj_clear_flag(lv_base_ptr_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_state(lv_base_ptr_, LV_STATE_DISABLED);
    }
}

bool BaseItem::getEnable() const
{
    return lv_base_ptr_ != nullptr && lv_obj_has_flag(lv_base_ptr_, LV_OBJ_FLAG_CLICKABLE);
}

// ---------------------------------------------------------------- padding / margin

BaseItem &BaseItem::setPaddingAll(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_all(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingHor(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_hor(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingVer(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_ver(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingTop(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_top(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingBottom(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_bottom(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingLeft(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_left(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingRight(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_right(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingRow(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_row(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setPaddingColumn(lv_coord_t padding)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_pad_column(lv_base_ptr_, padding, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setMarginAll(lv_coord_t margin)
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

BaseItem &BaseItem::setMarginHor(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_left(lv_base_ptr_, margin, LV_STATE_DEFAULT);
        lv_obj_set_style_margin_right(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setMarginVer(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_top(lv_base_ptr_, margin, LV_STATE_DEFAULT);
        lv_obj_set_style_margin_bottom(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setMarginTop(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_top(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setMarginBottom(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_bottom(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setMarginLeft(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_left(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

BaseItem &BaseItem::setMarginRight(lv_coord_t margin)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_style_margin_right(lv_base_ptr_, margin, LV_STATE_DEFAULT);
    }
    return *this;
}

// ---------------------------------------------------------------- tree

BaseItem *BaseItem::getParent() const
{
    if (lv_base_ptr_ == nullptr)
    {
        return nullptr;
    }

    lv_obj_t *parent = lv_obj_get_parent(lv_base_ptr_);
    while (parent != nullptr)
    {
        if (BaseItem *wrapper = fromLvglObject(parent); wrapper != nullptr)
        {
            return wrapper;
        }
        parent = lv_obj_get_parent(parent);
    }
    return nullptr;
}

void BaseItem::setParent(BaseItem *parent)
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }

    if (parent != nullptr && parent->getLvglItem() != nullptr)
    {
        lv_obj_set_parent(lv_base_ptr_, parent->getLvglItem());
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

uint32_t BaseItem::childCount() const
{
    return lv_base_ptr_ != nullptr ? lv_obj_get_child_count(lv_base_ptr_) : 0;
}

BaseItem *BaseItem::childAt(uint32_t index) const
{
    if (lv_base_ptr_ == nullptr)
    {
        return nullptr;
    }
    return fromLvglObject(lv_obj_get_child(lv_base_ptr_, static_cast<int32_t>(index)));
}

BaseItem *BaseItem::findChildInRawTree(lv_obj_t *obj, const std::string &name)
{
    if (obj == nullptr)
    {
        return nullptr;
    }

    if (BaseItem *wrapper = fromLvglObject(obj); wrapper != nullptr && wrapper->name() == name)
    {
        return wrapper;
    }

    const uint32_t count = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < count; ++i)
    {
        if (BaseItem *found = findChildInRawTree(lv_obj_get_child(obj, static_cast<int32_t>(i)), name); found != nullptr)
        {
            return found;
        }
    }
    return nullptr;
}

BaseItem *BaseItem::findChild(const std::string &n) const
{
    return findChildInRawTree(lv_base_ptr_, n);
}

void BaseItem::collectWrapperSubtree(lv_obj_t *obj, std::vector<BaseItem *> &out)
{
    if (obj == nullptr)
    {
        return;
    }

    if (BaseItem *wrapper = fromLvglObject(obj); wrapper != nullptr)
    {
        out.push_back(wrapper);
    }

    const uint32_t count = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < count; ++i)
    {
        collectWrapperSubtree(lv_obj_get_child(obj, static_cast<int32_t>(i)), out);
    }
}

void BaseItem::dumpTree(int depth) const
{
    if (lv_base_ptr_ == nullptr)
    {
        std::cout << std::string(static_cast<size_t>(depth) * 2, ' ') << "(detached)" << std::endl;
        return;
    }

    std::function<void(lv_obj_t *, int)> dump = [&](lv_obj_t *obj, int level) {
        const std::string indent(static_cast<size_t>(level) * 2, ' ');
        if (BaseItem *wrapper = fromLvglObject(obj); wrapper != nullptr)
        {
            std::cout << indent << (wrapper->name().empty() ? "(unnamed)" : wrapper->name()) << " [" << typeid(*wrapper).name() << "]"
                      << " visible=" << wrapper->getVisible() << " pos=(" << wrapper->getX() << "," << wrapper->getY() << ")"
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

std::string BaseItem::describeState() const
{
    if (lv_base_ptr_ == nullptr)
    {
        return R"({"alive":false})";
    }

    const Rect         r = bounds();
    std::ostringstream os;
    os << "{"
       << R"("name":")" << _name << "\","
       << R"("type":")" << typeid(*this).name() << "\","
       << R"("visible":)" << (getVisible() ? "true" : "false") << ","
       << R"("enabled":)" << (getEnable() ? "true" : "false") << ","
       << R"("rect":{"x":)" << r.x << ",\"y\":" << r.y << ",\"w\":" << r.w << ",\"h\":" << r.h << "},"
       << R"("children":)" << childCount() << "}";
    return os.str();
}

BaseItem::Rect BaseItem::bounds() const
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

BaseItem &BaseItem::setFlexFlow(lv_flex_flow_t flow)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_flex_flow(lv_base_ptr_, flow);
    }
    return *this;
}

BaseItem &BaseItem::setFlexAlign(lv_flex_align_t mainAxis, lv_flex_align_t crossAxis, lv_flex_align_t trackAlign)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_flex_align(lv_base_ptr_, mainAxis, crossAxis, trackAlign);
    }
    return *this;
}

BaseItem &BaseItem::setFlexGrow(uint8_t grow)
{
    if (lv_base_ptr_ != nullptr)
    {
        lv_obj_set_flex_grow(lv_base_ptr_, grow);
    }
    return *this;
}

// ---------------------------------------------------------------- grid

BaseItem::GridState &BaseItem::ensureGridState()
{
    if (!grid_)
    {
        grid_ = std::make_shared<GridState>();
    }
    return *grid_;
}

void BaseItem::applyGridTemplates()
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

void BaseItem::applyGridCell()
{
    if (lv_base_ptr_ == nullptr)
    {
        return;
    }
    GridState &grid = ensureGridState();
    lv_obj_set_grid_cell(lv_base_ptr_, grid.col_align, static_cast<int32_t>(grid.col_pos), static_cast<int32_t>(grid.col_span), grid.row_align, static_cast<int32_t>(grid.row_pos),
                         static_cast<int32_t>(grid.row_span));
}

BaseItem &BaseItem::setGridColumns(const std::string &columnDescriptors)
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

BaseItem &BaseItem::setGridRows(const std::string &rowDescriptors)
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

BaseItem &BaseItem::setGridCell(uint32_t column, uint32_t row, uint32_t colSpan, uint32_t rowSpan)
{
    GridState &grid = ensureGridState();
    grid.col_pos    = column;
    grid.row_pos    = row;
    grid.col_span   = colSpan == 0 ? 1 : colSpan;
    grid.row_span   = rowSpan == 0 ? 1 : rowSpan;
    applyGridCell();
    return *this;
}

BaseItem &BaseItem::setGridCellColumnAlign(lv_grid_align_t align)
{
    ensureGridState().col_align = align;
    applyGridCell();
    return *this;
}

BaseItem &BaseItem::setGridCellRowAlign(lv_grid_align_t align)
{
    ensureGridState().row_align = align;
    applyGridCell();
    return *this;
}

} // namespace lvglpp::widgets
