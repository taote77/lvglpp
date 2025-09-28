#include "TestFramework.h"
#include "../events/EventBus.h"
#include "../events/MouseEvent.h"
#include "../events/KeyEvent.h"
#include "../events/FocusEvent.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace QGL;

class EventSystemTests : public TestSuite {
public:
    EventSystemTests() : TestSuite("EventSystemTests") {}
    
    void runTests() override {
        testEventBusCreation();
        testBasicEventPublishing();
        testEventSubscription();
        testMouseEventCreation();
        testKeyEventCreation();
        testFocusEventCreation();
        testEventFiltering();
        testAsyncEventProcessing();
        testEventBusStatistics();
        testConnectionManagement();
    }

private:
    void testEventBusCreation() {
        TEST_START("EventBus Creation");
        
        EventBus* bus = EventBus::instance();
        ASSERT_NOT_NULL(bus, "EventBus instance should not be null");
        ASSERT_FALSE(bus->isRunning(), "EventBus should not be running initially");
        
        bus->start();
        ASSERT_TRUE(bus->isRunning(), "EventBus should be running after start()");
        
        bus->stop();
        ASSERT_FALSE(bus->isRunning(), "EventBus should not be running after stop()");
        
        TEST_PASS();
    }
    
    void testBasicEventPublishing() {
        TEST_START("Basic Event Publishing");
        
        EventBus* bus = EventBus::instance();
        bus->start();
        
        bool eventReceived = false;
        auto connection = bus->subscribe(EventType::System, [&eventReceived](Event* event) {
            eventReceived = true;
            ASSERT_EQ(event->type(), EventType::System, "Event type should match");
        });
        
        auto event = Event::create(EventType::System);
        bus->publish(std::move(event));
        bus->processEvents();
        
        ASSERT_TRUE(eventReceived, "Event should have been received");
        
        connection.disconnect();
        bus->stop();
        
        TEST_PASS();
    }
    
    void testEventSubscription() {
        TEST_START("Event Subscription");
        
        EventBus* bus = EventBus::instance();
        bus->start();
        
        int eventCount = 0;
        auto connection1 = bus->subscribe(EventType::Input, [&eventCount](Event* event) {
            eventCount++;
        });
        
        auto connection2 = bus->subscribe(EventType::Input, [&eventCount](Event* event) {
            eventCount++;
        });
        
        // Publish one event, should trigger both subscribers
        auto event = Event::create(EventType::Input);
        bus->publish(std::move(event));
        bus->processEvents();
        
        ASSERT_EQ(eventCount, 2, "Both subscribers should receive the event");
        
        // Disconnect one subscriber
        connection1.disconnect();
        
        // Publish another event
        auto event2 = Event::create(EventType::Input);
        bus->publish(std::move(event2));
        bus->processEvents();
        
        ASSERT_EQ(eventCount, 3, "Only one subscriber should receive the second event");
        
        connection2.disconnect();
        bus->stop();
        
        TEST_PASS();
    }
    
    void testMouseEventCreation() {
        TEST_START("MouseEvent Creation");
        
        Point position(100, 200);
        auto mouseEvent = MouseEvent::createPressEvent(position, MouseButton::LeftButton);
        
        ASSERT_NOT_NULL(mouseEvent.get(), "MouseEvent should be created");
        ASSERT_EQ(mouseEvent->type(), EventType::MousePress, "Event type should be MousePress");
        ASSERT_EQ(mouseEvent->position().x(), 100, "Mouse X position should be 100");
        ASSERT_EQ(mouseEvent->position().y(), 200, "Mouse Y position should be 200");
        ASSERT_TRUE(mouseEvent->isLeftButton(), "Should be left button press");
        
        auto clickEvent = MouseEvent::createClickEvent(position, MouseButton::RightButton, 2);
        ASSERT_EQ(clickEvent->clickCount(), 2, "Click count should be 2");
        ASSERT_TRUE(clickEvent->isRightButton(), "Should be right button click");
        
        TEST_PASS();
    }
    
    void testKeyEventCreation() {
        TEST_START("KeyEvent Creation");
        
        auto keyEvent = KeyEvent::createPressEvent(Key::Key_A, ModifierKey::ControlModifier, "A");
        
        ASSERT_NOT_NULL(keyEvent.get(), "KeyEvent should be created");
        ASSERT_EQ(keyEvent->type(), EventType::KeyPress, "Event type should be KeyPress");
        ASSERT_EQ(keyEvent->key(), Key::Key_A, "Key should be A");
        ASSERT_TRUE(keyEvent->ctrlPressed(), "Ctrl modifier should be pressed");
        ASSERT_EQ(keyEvent->text(), "A", "Text should be 'A'");
        
        String sequence = keyEvent->keySequence();
        ASSERT_TRUE(sequence.find("Ctrl") != String::npos, "Key sequence should contain Ctrl");
        ASSERT_TRUE(sequence.find("A") != String::npos, "Key sequence should contain A");
        
        TEST_PASS();
    }
    
    void testFocusEventCreation() {
        TEST_START("FocusEvent Creation");
        
        auto focusEvent = FocusEvent::createFocusInEvent(nullptr, FocusReason::Mouse);
        
        ASSERT_NOT_NULL(focusEvent.get(), "FocusEvent should be created");
        ASSERT_EQ(focusEvent->type(), EventType::Focus, "Event type should be Focus");
        ASSERT_EQ(focusEvent->reason(), FocusReason::Mouse, "Focus reason should be Mouse");
        ASSERT_TRUE(focusEvent->gotFocus(), "Should indicate got focus");
        ASSERT_FALSE(focusEvent->lostFocus(), "Should not indicate lost focus");
        
        auto focusOutEvent = FocusEvent::createFocusOutEvent(nullptr, FocusReason::Tab);
        ASSERT_EQ(focusOutEvent->type(), EventType::FocusOut, "Event type should be FocusOut");
        ASSERT_TRUE(focusOutEvent->lostFocus(), "Should indicate lost focus");
        
        TEST_PASS();
    }
    
    void testEventFiltering() {
        TEST_START("Event Filtering");
        
        class TestFilter : public EventFilter {
        public:
            bool filterEvent(Event* event) override {
                return event->type() == EventType::System; // Filter out system events
            }
            String filterName() const override { return "TestFilter"; }
        };
        
        EventBus* bus = EventBus::instance();
        bus->start();
        
        bool eventReceived = false;
        auto connection = bus->subscribe(EventType::System, [&eventReceived](Event* event) {
            eventReceived = true;
        });
        
        // Add filter
        bus->addEventFilter(std::make_unique<TestFilter>());
        
        // Publish system event (should be filtered)
        auto systemEvent = Event::create(EventType::System);
        bus->publish(std::move(systemEvent));
        bus->processEvents();
        
        ASSERT_FALSE(eventReceived, "System event should be filtered out");
        
        // Publish input event (should not be filtered)
        auto inputEvent = Event::create(EventType::Input);
        bus->publish(std::move(inputEvent));
        bus->processEvents();
        
        // Clean up
        bus->clearEventFilters();
        connection.disconnect();
        bus->stop();
        
        TEST_PASS();
    }
    
    void testAsyncEventProcessing() {
        TEST_START("Async Event Processing");
        
        EventBus* bus = EventBus::instance();
        bus->enableAsyncProcessing(true);
        bus->start();
        
        std::atomic<int> eventCount{0};
        auto connection = bus->subscribe(EventType::Input, [&eventCount](Event* event) {
            eventCount.fetch_add(1);
        });
        
        // Publish multiple events asynchronously
        for (int i = 0; i < 10; ++i) {
            auto future = bus->publishAsync(Event::create(EventType::Input));
        }
        
        // Wait a bit for async processing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        bus->processEvents();
        
        ASSERT_EQ(eventCount.load(), 10, "All 10 events should be processed");
        
        connection.disconnect();
        bus->enableAsyncProcessing(false);
        bus->stop();
        
        TEST_PASS();
    }
    
    void testEventBusStatistics() {
        TEST_START("EventBus Statistics");
        
        EventBus* bus = EventBus::instance();
        bus->resetStatistics();
        bus->start();
        
        auto connection = bus->subscribe(EventType::Input, [](Event* event) {
            // Simple event handler
        });
        
        // Publish some events
        for (int i = 0; i < 5; ++i) {
            bus->publish(Event::create(EventType::Input));
        }
        bus->processEvents();
        
        auto stats = bus->getStatistics();
        ASSERT_EQ(stats.totalEventsPublished, 5, "Should have published 5 events");
        ASSERT_EQ(stats.totalEventsProcessed, 5, "Should have processed 5 events");
        ASSERT_GT(stats.subscriberCount, 0, "Should have at least one subscriber");
        
        connection.disconnect();
        bus->stop();
        
        TEST_PASS();
    }
    
    void testConnectionManagement() {
        TEST_START("Connection Management");
        
        EventBus* bus = EventBus::instance();
        bus->start();
        
        bool eventReceived = false;
        
        // Test connection scope
        {
            auto connection = bus->subscribe(EventType::Widget, [&eventReceived](Event* event) {
                eventReceived = true;
            });
            
            ASSERT_TRUE(connection.isValid(), "Connection should be valid");
            
            // Publish event - should be received
            bus->publish(Event::create(EventType::Widget));
            bus->processEvents();
            
            ASSERT_TRUE(eventReceived, "Event should be received");
        } // connection goes out of scope
        
        // Reset flag and publish again
        eventReceived = false;
        bus->publish(Event::create(EventType::Widget));
        bus->processEvents();
        
        ASSERT_FALSE(eventReceived, "Event should not be received after connection destroyed");
        
        bus->stop();
        
        TEST_PASS();
    }
};

// Test execution function
void runEventSystemTests() {
    std::cout << "\n=== QGL Event System Tests ===" << std::endl;
    
    EventSystemTests tests;
    tests.runTests();
    
    std::cout << "\nEvent System Tests Summary:" << std::endl;
    std::cout << "Total Tests: " << tests.getTotalTests() << std::endl;
    std::cout << "Passed: " << tests.getPassedTests() << std::endl;
    std::cout << "Failed: " << tests.getFailedTests() << std::endl;
    
    if (tests.getFailedTests() == 0) {
        std::cout << "✅ All event system tests passed!" << std::endl;
    } else {
        std::cout << "❌ Some event system tests failed!" << std::endl;
    }
}

// Integration with main test runner
#ifdef QGL_TEST_STANDALONE
int main() {
    runEventSystemTests();
    return 0;
}
#endif