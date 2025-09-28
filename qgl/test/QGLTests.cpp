#include "TestFramework.h"

using namespace QGL;
using namespace QGL::Test;

// 核心类型测试
TestSuite* createCoreTypeTests() {
    TestSuite* suite = new TestSuite("Core Types");
    
    suite->addTest("Color Construction", []() {
        Color red(255, 0, 0, 255);
        ASSERT_EQUAL(255, red.red());
        ASSERT_EQUAL(0, red.green());
        ASSERT_EQUAL(0, red.blue());
        ASSERT_EQUAL(255, red.alpha());
        
        Color blue("#0000FF");
        ASSERT_EQUAL(0, blue.red());
        ASSERT_EQUAL(0, blue.green());
        ASSERT_EQUAL(255, blue.blue());
    });
    
    suite->addTest("Point Operations", []() {
        Point p1(10, 20);
        Point p2(5, 15);
        Point p3 = p1 + p2;
        
        ASSERT_EQUAL(15, p3.x);
        ASSERT_EQUAL(35, p3.y);
        
        Point p4 = p1 - p2;
        ASSERT_EQUAL(5, p4.x);
        ASSERT_EQUAL(5, p4.y);
    });
    
    suite->addTest("Size Operations", []() {
        Size size(100, 200);
        ASSERT_TRUE(size.isValid());
        ASSERT_FALSE(size.isEmpty());
        
        Size empty(0, 0);
        ASSERT_TRUE(empty.isEmpty());
    });
    
    suite->addTest("Rect Operations", []() {
        Rect rect(10, 20, 100, 50);
        ASSERT_EQUAL(Point(10, 20), rect.topLeft());
        ASSERT_EQUAL(Point(110, 20), rect.topRight());
        ASSERT_EQUAL(Point(10, 70), rect.bottomLeft());
        ASSERT_EQUAL(Point(110, 70), rect.bottomRight());
        ASSERT_EQUAL(Point(60, 45), rect.center());
        
        ASSERT_TRUE(rect.contains(Point(50, 40)));
        ASSERT_FALSE(rect.contains(Point(5, 40)));
    });
    
    return suite;
}

// Variant 测试
TestSuite* createVariantTests() {
    TestSuite* suite = new TestSuite("Variant System");
    
    suite->addTest("Basic Value Storage", []() {
        Variant intVar(42);
        ASSERT_EQUAL(42, intVar.getValue<int>());
        ASSERT_EQUAL(PropertyType::Int, intVar.getType());
        
        Variant strVar(String("Hello"));
        ASSERT_EQUAL(String("Hello"), strVar.getValue<String>());
        ASSERT_EQUAL(PropertyType::String, strVar.getType());
    });
    
    suite->addTest("Type Conversions", []() {
        Variant intVar(42);
        ASSERT_EQUAL(String("42"), intVar.toString());
        ASSERT_EQUAL(42.0f, intVar.toFloat());
        ASSERT_TRUE(intVar.toBool());
        
        Variant boolVar(false);
        ASSERT_EQUAL(0, boolVar.toInt());
        ASSERT_EQUAL(String("false"), boolVar.toString());
    });
    
    suite->addTest("Color Variant", []() {
        Color red(255, 0, 0);
        Variant colorVar = Variant::fromColor(red);
        
        Color retrieved = colorVar.getValue<Color>();
        ASSERT_EQUAL(255, retrieved.red());
        ASSERT_EQUAL(0, retrieved.green());
    });
    
    return suite;
}

// Object 系统测试
TestSuite* createObjectTests() {
    TestSuite* suite = new TestSuite("Object System");
    
    suite->addTest("Object Creation", []() {
        auto obj = std::make_shared<Object>();
        ASSERT_NOT_NULL(obj.get());
        ASSERT_EQUAL(String("Object"), obj->className());
    });
    
    suite->addTest("Property System", []() {
        auto obj = std::make_shared<Object>();
        
        obj->setProperty("test", Variant::fromInt(100));
        ASSERT_EQUAL(100, obj->getProperty("test").toInt());
        
        obj->setProperty("name", Variant::fromString("TestObject"));
        ASSERT_EQUAL(String("TestObject"), obj->getProperty("name").toString());
    });
    
    suite->addTest("Parent-Child Relationships", []() {
        auto parent = std::make_shared<Object>();
        auto child1 = std::make_shared<Object>(parent.get());
        auto child2 = std::make_shared<Object>(parent.get());
        
        ASSERT_EQUAL(parent.get(), child1->parent());
        ASSERT_EQUAL(2, parent->children().size());
        
        child1->setObjectName("Child1");
        Object* found = parent->findChild("Child1");
        ASSERT_EQUAL(child1.get(), found);
    });
    
    return suite;
}

// 信号槽测试
TestSuite* createSignalTests() {
    TestSuite* suite = new TestSuite("Signal-Slot System");
    
    suite->addTest("Basic Signal Connection", []() {
        Signal<int> testSignal;
        int receivedValue = 0;
        
        testSignal.connect([&receivedValue](int value) {
            receivedValue = value;
        });
        
        testSignal.emit(42);
        ASSERT_EQUAL(42, receivedValue);
    });
    
    suite->addTest("Multiple Connections", []() {
        Signal<> testSignal;
        int callCount = 0;
        
        testSignal.connect([&callCount]() { callCount++; });
        testSignal.connect([&callCount]() { callCount++; });
        testSignal.connect([&callCount]() { callCount++; });
        
        testSignal.emit();
        ASSERT_EQUAL(3, callCount);
    });
    
    suite->addTest("Signal Disconnection", []() {
        Signal<> testSignal;
        int callCount = 0;
        
        auto connection = testSignal.connect([&callCount]() { callCount++; });
        
        testSignal.emit();
        ASSERT_EQUAL(1, callCount);
        
        connection.disconnect();
        testSignal.emit();
        ASSERT_EQUAL(1, callCount); // 不应该再增加
    });
    
    return suite;
}

// 布局系统测试
TestSuite* createLayoutTests() {
    TestSuite* suite = new TestSuite("Layout System");
    
    suite->addTest("BoxLayout Creation", []() {
        auto layout = new HBoxLayout();
        ASSERT_NOT_NULL(layout);
        ASSERT_EQUAL(LayoutDirection::LeftToRight, layout->getDirection());
        
        delete layout;
    });
    
    suite->addTest("GridLayout Creation", []() {
        auto layout = new GridLayout();
        ASSERT_NOT_NULL(layout);
        ASSERT_EQUAL(0, layout->count());
        ASSERT_EQUAL(0, layout->rowCount());
        ASSERT_EQUAL(0, layout->columnCount());
        
        delete layout;
    });
    
    suite->addTest("FlexLayout Creation", []() {
        auto layout = new FlexLayout();
        ASSERT_NOT_NULL(layout);
        ASSERT_EQUAL(FlexDirection::Row, layout->getFlexDirection());
        ASSERT_EQUAL(FlexWrap::NoWrap, layout->getFlexWrap());
        
        delete layout;
    });
    
    return suite;
}

// 属性系统测试  
TestSuite* createPropertyTests() {
    TestSuite* suite = new TestSuite("Property System");
    
    suite->addTest("Property Creation", []() {
        Property prop("testProp", 42);
        ASSERT_EQUAL(String("testProp"), prop.getName());
        ASSERT_EQUAL(42, prop.getValue<int>());
        ASSERT_EQUAL(PropertyType::Int, prop.getType());
    });
    
    suite->addTest("Property Validation", []() {
        Property prop("testProp", 0);
        prop.setValidator([](const Variant& value) {
            return value.toInt() >= 0; // 只接受非负数
        });
        
        ASSERT_TRUE(prop.validate(Variant::fromInt(10)));
        ASSERT_FALSE(prop.validate(Variant::fromInt(-5)));
    });
    
    suite->addTest("CSS Parser Basic", []() {
        StyleSheetParser parser;
        std::vector<StyleRule> rules = parser.parse("Widget { background-color: red; width: 100px; }");
        
        ASSERT_EQUAL(1, rules.size());
        ASSERT_EQUAL(1, rules[0].selectors.size());
        ASSERT_EQUAL(SelectorType::Type, rules[0].selectors[0].type);
        ASSERT_EQUAL(String("Widget"), rules[0].selectors[0].value);
        ASSERT_EQUAL(2, rules[0].properties.size());
    });
    
    return suite;
}

// 定时器测试
TestSuite* createTimerTests() {
    TestSuite* suite = new TestSuite("Timer System");
    
    suite->addTest("Timer Creation", []() {
        Timer timer(1000);
        ASSERT_EQUAL(1000, timer.getInterval());
        ASSERT_EQUAL(TimerType::Repeating, timer.getType());
        ASSERT_FALSE(timer.isActive());
    });
    
    suite->addTest("SingleShot Timer", []() {
        bool called = false;
        Timer timer([&called]() { called = true; }, 100, TimerType::SingleShot);
        timer.setRepeatCount(1);
        
        ASSERT_EQUAL(TimerType::SingleShot, timer.getType());
        ASSERT_EQUAL(1, timer.getRepeatCount());
    });
    
    suite->addTest("TimerManager", []() {
        TimerManager& manager = TimerManager::instance();
        int initialCount = manager.getTotalTimerCount();
        
        Timer* timer = manager.createTimer(500);
        ASSERT_EQUAL(initialCount + 1, manager.getTotalTimerCount());
        
        manager.destroyTimer(timer);
        ASSERT_EQUAL(initialCount, manager.getTotalTimerCount());
    });
    
    return suite;
}

// 主测试函数
int main() {
    QGL::initialize();
    
    TestRunner& runner = TestRunner::instance();
    
    // 添加所有测试套件
    runner.addSuite(createCoreTypeTests());
    runner.addSuite(createVariantTests());
    runner.addSuite(createObjectTests());
    runner.addSuite(createSignalTests());
    runner.addSuite(createLayoutTests());
    runner.addSuite(createPropertyTests());
    runner.addSuite(createTimerTests());
    
    // 运行所有测试
    runner.runAll();
    
    QGL::cleanup();
    
    return 0;
}