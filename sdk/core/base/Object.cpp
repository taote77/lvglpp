#include "Object.h"

namespace lvglpp::core {

Object::Object()
{}

Object::~Object()
{
    // Remove all slots that reference this object as receiver from all senders
    // Note: This is a best-effort cleanup. Since we don't have a global registry
    // of all Object connections, we can only clean up slots registered on THIS object.
    // For slots where THIS object is the receiver registered on ANOTHER object,
    // the caller must use disconnect() explicitly or use the receiver-tracking connect.
    signals.clear();
}

void Object::connect(Object *sender, const std::string &signal_name, const Slot &slot)
{
    if (!sender) return;
    SlotEntry entry{slot, nullptr};
    sender->signals[signal_name].push_back(std::move(entry));
}

void Object::connect(Object *sender, const std::string &signal_name,
                     const Object *receiver, const Slot &slot)
{
    if (!sender) return;
    SlotEntry entry{slot, receiver};
    sender->signals[signal_name].push_back(std::move(entry));
}

void Object::emitSignal(const std::string &signal_name)
{
    auto it = signals.find(signal_name);
    if (it == signals.end()) return;

    // Copy the vector so slots can safely add/remove connections during iteration
    auto entries = it->second;
    for (const auto &entry : entries) {
        if (entry.callback) {
            entry.callback();
        }
    }
}

void Object::disconnect(const std::string &signal_name)
{
    if (signal_name.empty()) {
        signals.clear();
        return;
    }
    auto it = signals.find(signal_name);
    if (it != signals.end()) {
        signals.erase(it);
    }
}

void Object::disconnect(const std::string &signal_name, const Object *receiver)
{
    if (!receiver) return;
    auto it = signals.find(signal_name);
    if (it == signals.end()) return;

    auto &entries = it->second;
    entries.erase(
        std::remove_if(entries.begin(), entries.end(),
                       [receiver](const SlotEntry &e) { return e.receiver == receiver; }),
        entries.end()
    );

    if (entries.empty()) {
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
    if (it != properties.end()) {
        return it->second;
    }
    return {};
}

} // namespace lvglpp::core
