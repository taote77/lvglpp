#include "TestFramework.h"
#include "core/kernel/Object.h"
#include "core/sys/Event.h"
#include <any>
#include <optional>
#include <string>

using namespace lvglpp::test;
using namespace lvglpp;

// ---- Object Signal/Slot Tests ----

TEST(Object_connect_and_emit)
{
    core::Object obj;
    int callCount = 0;
    core::Object::connect(&obj, "clicked", [&callCount]() { callCount++; });
    obj.emitSignal("clicked");
    CHECK_EQ(callCount, 1);
    return true;
}

TEST(Object_multiple_slots)
{
    core::Object obj;
    int a = 0, b = 0;
    core::Object::connect(&obj, "update", [&a]() { a = 1; });
    core::Object::connect(&obj, "update", [&b]() { b = 2; });
    obj.emitSignal("update");
    CHECK_EQ(a, 1);
    CHECK_EQ(b, 2);
    return true;
}

TEST(Object_emit_empty_signal)
{
    core::Object obj;
    obj.emitSignal("nonexistent"); // should not crash
    return true;
}

TEST(Object_property_set_get)
{
    core::Object obj;
    obj.setProperty("width", "100");
    CHECK(obj.property("width") == "100");
    CHECK(obj.property("nonexistent") == "");
    return true;
}

// ---- Event Tests ----

TEST(Event_construct_with_data)
{
    sys::Event evt(42, 0, std::any(std::string("hello")));
    CHECK_EQ(evt.getType(), 42);
    CHECK_EQ(evt.getErrorCode(), 0);
    return true;
}

TEST(Event_convertData_int)
{
    sys::Event evt(1, 0, std::any(123));
    auto val = evt.convertData<int>();
    CHECK_TRUE(val.has_value());
    CHECK_EQ(*val, 123);
    return true;
}

TEST(Event_convertData_string)
{
    sys::Event evt(1, 0, std::any(std::string("world")));
    auto val = evt.convertData<std::string>();
    CHECK_TRUE(val.has_value());
    CHECK(*val == "world");
    return true;
}

TEST(Event_convertData_wrong_type)
{
    sys::Event evt(1, 0, std::any(42));
    auto val = evt.convertData<std::string>();
    CHECK_FALSE(val.has_value()); // should return nullopt, not throw
    return true;
}

TEST(Event_empty_constructor)
{
    sys::Event evt(99, -1);
    CHECK_EQ(evt.getType(), 99);
    CHECK_EQ(evt.getErrorCode(), -1);
    return true;
}

// ---- std::any Tests (verify replacement from boost::any) ----

TEST(Any_basic_usage)
{
    std::any a = 42;
    CHECK_EQ(std::any_cast<int>(a), 42);
    a = std::string("hello");
    CHECK(std::any_cast<std::string>(a) == "hello");
    return true;
}

TEST(Any_has_value)
{
    std::any a;
    CHECK_FALSE(a.has_value());
    a = 3.14;
    CHECK_TRUE(a.has_value());
    return true;
}

TEST(Optional_basic)
{
    std::optional<int> opt;
    CHECK_FALSE(opt.has_value());
    opt = 42;
    CHECK_TRUE(opt.has_value());
    CHECK_EQ(*opt, 42);
    return true;
}
