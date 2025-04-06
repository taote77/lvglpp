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
    using Slot   = std::function<void()>;
    using Signal = std::vector<Slot>;

    Object();
    virtual ~Object();

    static void connect(const Object *sender, const std::string &signal_name, const Slot &slot);

    static void connect(const Object *sender, const std::string &signal_name, const Object *receiver, const Slot &slot);

    void emitSignal(const std::string &signal_name);

    void setProperty(const std::string &signal_name, const std::string &value);

    std::string property(const std::string &property_name) const;

private:
    std::map<std::string, Signal>      signals;
    std::map<std::string, std::string> properties;
};

} // namespace lvglpp::core

#endif // LV_OBJECT_H