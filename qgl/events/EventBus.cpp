#include "EventBus.h"
#include <algorithm>
#include <iostream>
#include <chrono>
#include <shared_mutex>

namespace QGL {

// 静态成员定义
std::unique_ptr<EventBus> EventBus::s_instance = nullptr;
std::mutex EventBus::s_instanceMutex;

// EventBusConnection 实现
void EventBusConnection::disconnect() {
    if (m_valid.load()) {
        m_valid = false;
        // 暂时简化，避免循环依赖
    }
}

// EventBus 实现
EventBus* EventBus::instance() {
    std::lock_guard<std::mutex> lock(s_instanceMutex);
    if (!s_instance) {
        s_instance = std::unique_ptr<EventBus>(new EventBus());
        s_instance->initializeEventBus();
    }
    return s_instance.get();
}

void EventBus::destroyInstance() {
    std::lock_guard<std::mutex> lock(s_instanceMutex);
    if (s_instance) {
        s_instance->shutdownEventBus();
        s_instance.reset();
    }
}

EventBus::EventBus() : Object() {
    setObjectName("EventBus");
    m_statistics.startTime = std::chrono::steady_clock::now();
    m_lastCleanupTime = std::chrono::steady_clock::now();
}

EventBus::~EventBus() {
    shutdownEventBus();
}

void EventBus::initializeEventBus() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    if (m_running.load()) {
        return; // 已经初始化
    }
    
    m_running = true;
    m_paused = false;
    m_shouldStop = false;
    
    // 发送启动信号
    busStarted.emit();
}

void EventBus::shutdownEventBus() {
    if (!m_running.load()) {
        return;
    }
    
    m_shouldStop = true;
    m_running = false;
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    // 清理所有订阅
    unsubscribeAll();
    
    // 发送停止信号
    busStopped.emit();
}

void EventBus::start() {
    if (!m_running.load()) {
        initializeEventBus();
    } else {
        resume();
    }
}

void EventBus::stop() {
    shutdownEventBus();
}

void EventBus::pause() {
    m_paused = true;
}

void EventBus::resume() {
    m_paused = false;
}

EventBusConnection EventBus::subscribe(EventType eventType, EventHandlerFunction handler, Object* receiver) {
    return subscribe(eventType, std::move(handler), EventPriority::Normal, receiver);
}

EventBusConnection EventBus::subscribe(EventType eventType, EventHandlerFunction handler, EventPriority priority, Object* receiver) {
    if (!handler) {
        return EventBusConnection(); // 无效连接
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    uint64_t subscriptionId = generateSubscriptionId();
    auto subscription = std::make_unique<EventSubscription>(subscriptionId, eventType, std::move(handler), receiver);
    subscription->priority = priority;
    
    // 添加到类型订阅表
    m_typeSubscriptions[eventType].push_back(std::move(subscription));
    
    // 添加到查找表
    m_subscriptionLookup[subscriptionId] = m_typeSubscriptions[eventType].back().get();
    
    // 发送订阅添加信号
    subscriptionAdded.emit(eventType, receiver);
    
    return EventBusConnection(subscriptionId);
}

EventBusConnection EventBus::subscribe(const String& eventName, EventHandlerFunction handler, Object* receiver) {
    return subscribe(eventName, std::move(handler), EventPriority::Normal, receiver);
}

EventBusConnection EventBus::subscribe(const String& eventName, EventHandlerFunction handler, EventPriority priority, Object* receiver) {
    if (!handler || eventName.empty()) {
        return EventBusConnection(); // 无效连接
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    uint64_t subscriptionId = generateSubscriptionId();
    auto subscription = std::make_unique<EventSubscription>(subscriptionId, eventName, std::move(handler), receiver);
    subscription->priority = priority;
    
    // 添加到名称订阅表
    m_nameSubscriptions[eventName].push_back(std::move(subscription));
    
    // 添加到查找表
    m_subscriptionLookup[subscriptionId] = m_nameSubscriptions[eventName].back().get();
    
    return EventBusConnection(subscriptionId);
}

void EventBus::unsubscribe(const EventBusConnection& connection) {
    if (!connection.isValid()) {
        return;
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    auto it = m_subscriptionLookup.find(connection.id());
    if (it != m_subscriptionLookup.end()) {
        EventSubscription* subscription = it->second;
        subscription->isActive = false;
        m_subscriptionLookup.erase(it);
    }
}

void EventBus::unsubscribeAll() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    // 清理所有订阅
    m_typeSubscriptions.clear();
    m_nameSubscriptions.clear();
    m_subscriptionLookup.clear();
    m_receiverSubscriptions.clear();
}

void EventBus::publish(std::unique_ptr<Event> event) {
    if (!event || !m_running.load()) {
        return;
    }
    
    // 简化处理，直接同步处理
    processEventInternal(event.get());
    
    // 发送事件发布信号
    eventPublished.emit(event->type(), event.get());
}

void EventBus::publish(EventType eventType) {
    auto event = Event::create(eventType);
    publish(std::move(event));
}

void EventBus::publish(EventType eventType, const Variant& payload) {
    auto event = Event::create(eventType, payload);
    publish(std::move(event));
}

void EventBus::processEvents() {
    // 简化实现
}

void EventBus::processEventInternal(Event* event) {
    if (!event || m_paused.load()) {
        return;
    }
    
    try {
        // 分发事件
        distributeEvent(event);
        
        // 发送事件处理完成信号
        eventProcessed.emit(event->type(), event);
        
    } catch (const std::exception& e) {
        String errorMsg = String("Event processing error: ") + e.what();
        busError.emit(errorMsg);
    }
}

void EventBus::distributeEvent(Event* event) {
    auto subscribers = getSubscribersForEvent(event);
    
    // 分发事件给所有订阅者
    for (EventSubscription* subscription : subscribers) {
        if (!subscription->isActive || event->isConsumed()) {
            continue;
        }
        
        try {
            subscription->handler(event);
        } catch (const std::exception& e) {
            String errorMsg = String("Event handler error: ") + e.what();
            busError.emit(errorMsg);
        }
    }
}

std::vector<EventSubscription*> EventBus::getSubscribersForEvent(Event* event) {
    std::vector<EventSubscription*> subscribers;
    
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    // 获取类型订阅者
    auto typeIt = m_typeSubscriptions.find(event->type());
    if (typeIt != m_typeSubscriptions.end()) {
        for (const auto& subscription : typeIt->second) {
            if (subscription->isActive) {
                subscribers.push_back(subscription.get());
            }
        }
    }
    
    return subscribers;
}

uint64_t EventBus::generateSubscriptionId() {
    return m_nextSubscriptionId.fetch_add(1);
}

EventBusStatistics EventBus::getStatistics() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_statistics;
}

void EventBus::resetStatistics() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_statistics.reset();
}

size_t EventBus::eventQueueSize() const {
    return 0; // 简化实现
}

void EventBus::addEventFilter(std::unique_ptr<EventFilter> filter) {
    if (!filter) return;
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_eventFilters.push_back(std::move(filter));
}

void EventBus::removeEventFilter(const String& filterName) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    // 简化实现
}

void EventBus::clearEventFilters() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_eventFilters.clear();
}

} // namespace QGL