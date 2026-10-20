#ifndef LV_OBJECT_H
#define LV_OBJECT_H

#include "Connection.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace lvglpp::core {

/**
 * Minimal named signal/slot object (no LVGL dependency).
 *
 * Connections are safe across destruction:
 *  - a Connection token becomes inert when its sender is destroyed;
 *  - slots connected with a receiver are automatically removed when the
 *    receiver is destroyed;
 *  - destroying a sender marks all of its tokens disconnected.
 */
class Object
{
public:
    using Slot = std::function<void()>;

    Object();
    virtual ~Object();

    Object(const Object &)            = delete;
    Object &operator=(const Object &) = delete;
    Object(Object &&)                 = delete;
    Object &operator=(Object &&)      = delete;

    /// Connect a slot to a named signal on a sender.
    static Connection connect(Object *sender, const std::string &signal_name, const Slot &slot);

    /// Connect with receiver tracking — the connection is removed automatically
    /// when `receiver` is destroyed.
    static Connection connect(Object *sender, const std::string &signal_name, const Object *receiver, const Slot &slot);

    /// Disconnect all slots for a signal, or all slots if signal_name is empty.
    void disconnect(const std::string &signal_name = "");

    /// Disconnect a specific receiver from a signal.
    void disconnect(const std::string &signal_name, const Object *receiver);

    /// Emit a signal, calling all connected slots.
    void emitSignal(const std::string &signal_name);

    /// Set a string property.
    void setProperty(const std::string &property_name, const std::string &value);

    /// Get a string property.
    std::string property(const std::string &property_name) const;

private:
    struct ObjectLifetime {
        bool alive = true;
    };

    struct SlotEntry {
        std::uint64_t                            id = 0;
        Slot                                     callback;
        const Object                            *receiver = nullptr;
        std::weak_ptr<ObjectLifetime>            receiver_lifetime;
        std::shared_ptr<detail::ConnectionState> connection;
    };

    struct OutgoingConnection {
        Object                                  *sender = nullptr;
        std::weak_ptr<ObjectLifetime>            sender_lifetime;
        std::string                              signal_name;
        std::uint64_t                            slot_id = 0;
        std::shared_ptr<detail::ConnectionState> connection;
    };

    void removeSlot(const std::string &signal_name, std::uint64_t id);

    std::map<std::string, std::vector<std::shared_ptr<SlotEntry>>> signals;
    std::map<std::string, std::string>                             properties;

    std::shared_ptr<ObjectLifetime> lifetime_     = std::make_shared<ObjectLifetime>();
    std::uint64_t                   next_slot_id_ = 0;

    /// Receiver-side bookkeeping so we can detach from senders on destruction.
    mutable std::vector<OutgoingConnection> outgoing_connections_;
};

} // namespace lvglpp::core

#endif // LV_OBJECT_H
