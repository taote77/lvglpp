#ifndef LV_OBJECT_H
#define LV_OBJECT_H

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace lvglpp::core {

class Object
{
public:
    using Slot = std::function<void()>;

    Object();
    virtual ~Object();

    /// Connect a slot to a named signal on a sender
    static void connect(Object *sender, const std::string &signal_name, const Slot &slot);

    /// Connect with receiver tracking — auto-disconnects when receiver is destroyed
    static void connect(Object *sender, const std::string &signal_name,
                        const Object *receiver, const Slot &slot);

    /// Disconnect all slots for a signal, or all slots if signal_name is empty
    void disconnect(const std::string &signal_name = "");

    /// Disconnect a specific receiver from a signal
    void disconnect(const std::string &signal_name, const Object *receiver);

    /// Emit a signal, calling all connected slots
    void emitSignal(const std::string &signal_name);

    /// Set a string property
    void setProperty(const std::string &property_name, const std::string &value);

    /// Get a string property
    std::string property(const std::string &property_name) const;

private:
    struct SlotEntry {
        Slot              callback;
        const Object     *receiver = nullptr; // nullptr = no receiver tracking
    };

    std::map<std::string, std::vector<SlotEntry>> signals;
    std::map<std::string, std::string>            properties;
};

} // namespace lvglpp::core

#endif // LV_OBJECT_H
