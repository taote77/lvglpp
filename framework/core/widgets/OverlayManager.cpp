#include "OverlayManager.h"

#include <algorithm>

namespace lvglpp::widgets {

OverlayManager &OverlayManager::getInstance()
{
    // Process-lifetime: overlays may outlive static destructors.
    static OverlayManager *instance = new OverlayManager();
    return *instance;
}

OverlayManager::Token OverlayManager::attach(Widget *root, Priority priority, sys::InputManager::BackHandler onBack)
{
    if (root == nullptr)
    {
        return {};
    }

    Entry entry;
    entry.id       = next_id_++;
    entry.priority = priority;
    entry.seq      = next_seq_++;
    entry.root     = root;

    if (priority >= Priority::Popup)
    {
        const auto backPriority = static_cast<sys::BackPriority>(static_cast<std::int32_t>(priority));
        entry.scope             = sys::InputManager::getInstance().pushModalScope(root, std::move(onBack), backPriority);
    }

    entries_.push_back(entry);
    restack();
    return Token{entry.id};
}

void OverlayManager::detach(Token token)
{
    auto it = std::find_if(entries_.begin(), entries_.end(), [token](const Entry &entry) { return entry.id == token.id; });
    if (it == entries_.end())
    {
        return;
    }

    if (it->scope != 0)
    {
        sys::InputManager::getInstance().popScope(it->scope);
    }

    entries_.erase(it);
    restack();
}

void OverlayManager::bringToFront(Token token)
{
    Entry *entry = find(token);
    if (entry == nullptr)
    {
        return;
    }

    entry->seq = next_seq_++;
    restack();
}

bool OverlayManager::isTopmost(Token token) const
{
    const Entry *top = topEntry();
    return top != nullptr && top->id == token.id;
}

OverlayManager::Priority OverlayManager::topPriority() const
{
    const Entry *top = topEntry();
    return top != nullptr ? top->priority : Priority::Toast;
}

int OverlayManager::count(Priority priority) const
{
    return static_cast<int>(std::count_if(entries_.begin(), entries_.end(), [priority](const Entry &entry) { return entry.priority == priority; }));
}

OverlayManager::Token OverlayManager::registerTransient(Widget *root, Priority priority)
{
    if (root == nullptr)
    {
        return {};
    }

    for (const Entry &entry : entries_)
    {
        if (entry.transient && entry.root == root)
        {
            return Token{entry.id};
        }
    }

    Entry entry;
    entry.id        = next_id_++;
    entry.priority  = priority;
    entry.seq       = next_seq_++;
    entry.root      = root;
    entry.transient = true;

    entries_.push_back(entry);
    restack();
    return Token{entry.id};
}

void OverlayManager::unregisterTransient(Token token)
{
    auto it = std::find_if(entries_.begin(), entries_.end(), [token](const Entry &entry) { return entry.transient && entry.id == token.id; });
    if (it == entries_.end())
    {
        return;
    }

    entries_.erase(it);
    restack();
}

void OverlayManager::shutdown()
{
    for (const Entry &entry : entries_)
    {
        if (entry.scope != 0)
        {
            sys::InputManager::getInstance().popScope(entry.scope);
        }
    }
    entries_.clear();
}

OverlayManager::Entry *OverlayManager::find(Token token)
{
    auto it = std::find_if(entries_.begin(), entries_.end(), [token](const Entry &entry) { return entry.id == token.id; });
    return it == entries_.end() ? nullptr : &*it;
}

const OverlayManager::Entry *OverlayManager::find(Token token) const
{
    auto it = std::find_if(entries_.begin(), entries_.end(), [token](const Entry &entry) { return entry.id == token.id; });
    return it == entries_.end() ? nullptr : &*it;
}

const OverlayManager::Entry *OverlayManager::topEntry() const
{
    const Entry *top = nullptr;
    for (const Entry &entry : entries_)
    {
        if (top == nullptr || entry.priority > top->priority || (entry.priority == top->priority && entry.seq > top->seq))
        {
            top = &entry;
        }
    }
    return top;
}

void OverlayManager::restack()
{
    if (restacking_)
    {
        return;
    }
    restacking_ = true;

    std::vector<Entry> ordered = entries_;
    std::stable_sort(ordered.begin(), ordered.end(), [](const Entry &a, const Entry &b) {
        if (a.priority != b.priority)
        {
            return a.priority < b.priority;
        }
        return a.seq < b.seq;
    });

    // Move in ascending order so the highest priority/sequence ends up on top
    // within its LVGL layer. Toast is intentionally not raised last.
    for (const Entry &entry : ordered)
    {
        if (entry.root != nullptr && entry.root->isAttached() && entry.root->lvObj() != nullptr)
        {
            lv_obj_move_foreground(entry.root->lvObj());
        }
    }

    restacking_ = false;
}

} // namespace lvglpp::widgets
