#include "TestFramework.h"
#include "core/base/Object.h"
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

TEST(Object_disconnect_signal)
{
    core::Object obj;
    int count = 0;
    core::Object::connect(&obj, "test", [&count]() { count++; });
    obj.disconnect("test");
    obj.emitSignal("test");
    CHECK_EQ(count, 0);
    return true;
}

TEST(Object_disconnect_all)
{
    core::Object obj;
    int a = 0, b = 0;
    core::Object::connect(&obj, "sig1", [&a]() { a++; });
    core::Object::connect(&obj, "sig2", [&b]() { b++; });
    obj.disconnect(); // disconnect all
    obj.emitSignal("sig1");
    obj.emitSignal("sig2");
    CHECK_EQ(a, 0);
    CHECK_EQ(b, 0);
    return true;
}

TEST(Object_connect_with_receiver)
{
    core::Object sender;
    core::Object receiver;
    int count = 0;
    core::Object::connect(&sender, "test", &receiver, [&count]() { count++; });
    sender.emitSignal("test");
    CHECK_EQ(count, 1);
    // Disconnect by receiver
    sender.disconnect("test", &receiver);
    sender.emitSignal("test");
    CHECK_EQ(count, 1); // should not have been called again
    return true;
}

TEST(Object_signal_doesnt_call_dead_slots)
{
    // Verify that emitting a signal with nullptr callbacks doesn't crash
    core::Object obj;
    core::Object::connect(&obj, "test", core::Object::Slot{});
    obj.emitSignal("test");
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

// ---- std::any Tests ----

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

// ---- C++17 Feature Verification Tests ----

TEST(Cpp17_structured_bindings)
{
    std::pair<int, std::string> p{42, "hello"};
    auto [num, str] = p;
    CHECK_EQ(num, 42);
    CHECK(str == "hello");
    return true;
}

TEST(Cpp17_string_view)
{
    std::string_view sv = "hello world";
    CHECK(sv.substr(0, 5) == "hello");
    return true;
}

TEST(Cpp17_filesystem)
{
    std::filesystem::path p = "/tmp/test";
    CHECK(p.filename() == "test");
    return true;
}
