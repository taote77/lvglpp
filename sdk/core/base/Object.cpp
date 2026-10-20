#include "Object.h"

#include <algorithm>
#include <utility>

namespace lvglpp::core {

Object::Object() = default;

Object::~Object()
{
    // This object is no longer a valid receiver/sender.
    lifetime_->alive = false;

    // Detach every slot we registered on other senders as a receiver.
    for (auto &outgoing : outgoing_connections_)
    {
        if (auto sender_lifetime = outgoing.sender_lifetime.lock(); sender_lifetime && sender_lifetime->alive && outgoing.sender != nullptr)
        {
            outgoing.sender->removeSlot(outgoing.signal_name, outgoing.slot_id);
        }

        if (outgoing.connection)
        {
            outgoing.connection->connected     = false;
            outgoing.connection->on_disconnect = nullptr;
        }
    }
    outgoing_connections_.clear();

    // Invalidate tokens held by callers for slots on this sender.
    for (auto &[signal_name, entries] : signals)
    {
        (void)signal_name;
        for (auto &entry : entries)
        {
            if (entry->connection)
            {
                entry->connection->connected     = false;
                entry->connection->on_disconnect = nullptr;
            }
        }
    }
    signals.clear();
}

Connection Object::connect(Object *sender, const std::string &signal_name, const Slot &slot)
{
    return connect(sender, signal_name, nullptr, slot);
}

Connection Object::connect(Object *sender, const std::string &signal_name, const Object *receiver, const Slot &slot)
{
    if (sender == nullptr)
    {
        return Connection();
    }

    auto entry        = std::make_shared<SlotEntry>();
    entry->id         = sender->next_slot_id_++;
    entry->callback   = slot;
    entry->receiver   = receiver;
    entry->connection = std::make_shared<detail::ConnectionState>();

    if (receiver != nullptr)
    {
        entry->receiver_lifetime = receiver->lifetime_;

        // Register on the receiver so ~Object can detach from this sender.
        OutgoingConnection outgoing;
        outgoing.sender          = sender;
        outgoing.sender_lifetime = sender->lifetime_;
        outgoing.signal_name     = signal_name;
        outgoing.slot_id         = entry->id;
        outgoing.connection      = entry->connection;
        receiver->outgoing_connections_.push_back(std::move(outgoing));
    }

    // The disconnect callback fires when a caller invokes Connection::disconnect().
    // It captures the sender weakly so it is inert after the sender is destroyed.
    const std::uint64_t slot_id      = entry->id;
    entry->connection->on_disconnect = [sender, sender_lifetime = sender->lifetime_, signal_name, slot_id]() {
        if (sender_lifetime && sender_lifetime->alive)
        {
            sender->removeSlot(signal_name, slot_id);
        }
    };

    sender->signals[signal_name].push_back(std::move(entry));
    return Connection(sender->signals[signal_name].back()->connection);
}

void Object::removeSlot(const std::string &signal_name, std::uint64_t id)
{
    auto it = signals.find(signal_name);
    if (it == signals.end())
    {
        return;
    }

    auto &entries = it->second;
    auto  found   = std::find_if(entries.begin(), entries.end(), [id](const std::shared_ptr<SlotEntry> &entry) { return entry->id == id; });

    if (found == entries.end())
    {
        return;
    }

    // Mark the token disconnected without re-entering on_disconnect.
    if ((*found)->connection)
    {
        (*found)->connection->connected     = false;
        (*found)->connection->on_disconnect = nullptr;
    }
    entries.erase(found);

    if (entries.empty())
    {
        signals.erase(it);
    }
}

void Object::emitSignal(const std::string &signal_name)
{
    auto it = signals.find(signal_name);
    if (it == signals.end())
    {
        return;
    }

    // Copy so slots can safely add/remove connections during iteration.
    auto entries = it->second;

    for (const auto &entry : entries)
    {
        if (!entry->connection || !entry->connection->connected)
        {
            continue;
        }

        if (entry->receiver != nullptr)
        {
            auto receiver_lifetime = entry->receiver_lifetime.lock();
            if (!receiver_lifetime || !receiver_lifetime->alive)
            {
                continue;
            }
        }

        if (entry->callback)
        {
            entry->callback();
        }
    }
}

void Object::disconnect(const std::string &signal_name)
{
    if (signal_name.empty())
    {
        for (auto &[name, entries] : signals)
        {
            (void)name;
            for (auto &entry : entries)
            {
                if (entry->connection)
                {
                    entry->connection->connected     = false;
                    entry->connection->on_disconnect = nullptr;
                }
            }
        }
        signals.clear();
        return;
    }

    auto it = signals.find(signal_name);
    if (it == signals.end())
    {
        return;
    }

    for (auto &entry : it->second)
    {
        if (entry->connection)
        {
            entry->connection->connected     = false;
            entry->connection->on_disconnect = nullptr;
        }
    }
    signals.erase(it);
}

void Object::disconnect(const std::string &signal_name, const Object *receiver)
{
    if (receiver == nullptr)
    {
        return;
    }

    auto it = signals.find(signal_name);
    if (it == signals.end())
    {
        return;
    }

    auto &entries = it->second;
    for (auto &entry : entries)
    {
        if (entry->receiver == receiver && entry->connection)
        {
            entry->connection->connected     = false;
            entry->connection->on_disconnect = nullptr;
        }
    }

    entries.erase(std::remove_if(entries.begin(), entries.end(), [receiver](const std::shared_ptr<SlotEntry> &entry) { return entry->receiver == receiver; }), entries.end());

    if (entries.empty())
    {
        signals.erase(it);
    }
}

void Object::setProperty(const std::string &property_name, const std::string &value)
{
    properties[property_name] = value;
}

std::string Object::property(const std::string &property_name) const
{
    auto it = properties.find(property_name);
    if (it != properties.end())
    {
        return it->second;
    }
    return {};
}

} // namespace lvglpp::core
