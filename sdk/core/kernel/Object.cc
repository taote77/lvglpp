#include "Object.h"

namespace lvglpp::core {

Object::Object()
{}

Object::~Object()
{}

void Object::connect(const Object *sender, const std::string &signal_name, const Slot &slot)
{
    auto non_const_sender = const_cast<Object *>(sender);
    auto it               = non_const_sender->signals.find(signal_name);
    if (it != non_const_sender->signals.end())
    {
        it->second.push_back(slot);
    } else
    {
        Signal newSignal;
        newSignal.push_back(slot);
        non_const_sender->signals[signal_name] = newSignal;
    }
}

void Object::connect(const Object *sender, const std::string &signal_name, const Object *receiver, const Slot &slot)
{
    auto non_const_sender = const_cast<Object *>(sender);
    auto it               = non_const_sender->signals.find(signal_name);
    if (it != non_const_sender->signals.end())
    {
        it->second.push_back(slot);
    } else
    {
        Signal newSignal;
        newSignal.push_back(slot);
        non_const_sender->signals[signal_name] = newSignal;
    }
}

void Object::emitSignal(const std::string &signal_name)
{
    auto it = signals.find(signal_name);
    if (it != signals.end())
    {
        for (const auto &slot : it->second)
        {
            slot();
        }
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
    return "";
}

} // namespace lvglpp::core