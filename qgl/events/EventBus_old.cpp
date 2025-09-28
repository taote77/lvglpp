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
        EventBus::instance()->unsubscribe(*this);
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
    
    // 重置统计信息
    m_statistics.reset();
    
    // 清理所有订阅和队列
    m_typeSubscriptions.clear();
    m_nameSubscriptions.clear();
    m_subscriptionLookup.clear();
    m_receiverSubscriptions.clear();
    
    // 清空事件队列
    while (!m_eventQueue.empty()) {
        m_eventQueue.pop();
    }
    while (!m_delayedEventQueue.empty()) {
        m_delayedEventQueue.pop();
    }
    
    // 清理事件过滤器
    m_eventFilters.clear();
    
    m_running = true;
    m_paused = false;
    m_shouldStop = false;
    
    // 启动工作线程
    if (m_asyncProcessingEnabled.load()) {
        startWorkerThreads();
    }
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Event bus initialized successfully" << std::endl;
    }
    
    // 发送启动信号
    busStarted.emit();
}

void EventBus::shutdownEventBus() {
    if (!m_running.load()) {
        return;
    }
    
    m_shouldStop = true;
    m_running = false;
    
    // 通知所有工作线程停止
    m_eventCondition.notify_all();
    
    // 停止工作线程
    stopWorkerThreads();
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    // 清理所有订阅
    unsubscribeAll();
    
    // 清空事件队列
    clearEventQueue();
    
    // 清理事件过滤器
    clearEventFilters();
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Event bus shutdown completed" << std::endl;
    }
    
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
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Event bus paused" << std::endl;
    }
}

void EventBus::resume() {
    m_paused = false;
    m_eventCondition.notify_all();
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Event bus resumed" << std::endl;
    }
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
    
    // 记录接收者订阅
    if (receiver) {
        m_receiverSubscriptions[receiver].insert(subscriptionId);
    }
    
    // 更新统计信息
    m_statistics.subscriberCount++;
    m_statistics.activeSubscriberCount++;
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Subscribed to event type: " << static_cast<uint32_t>(eventType) 
                  << ", subscription ID: " << subscriptionId << std::endl;
    }
    
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
    
    // 记录接收者订阅
    if (receiver) {
        m_receiverSubscriptions[receiver].insert(subscriptionId);
    }
    
    // 更新统计信息
    m_statistics.subscriberCount++;
    m_statistics.activeSubscriberCount++;
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Subscribed to event name: " << eventName 
                  << ", subscription ID: " << subscriptionId << std::endl;
    }
    
    // 发送订阅添加信号
    subscriptionAdded.emit(EventType::None, receiver);
    
    return EventBusConnection(subscriptionId);
}

void EventBus::unsubscribe(const EventBusConnection& connection) {
    if (!connection.isValid()) {
        return;
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    auto it = m_subscriptionLookup.find(connection.id());
    if (it == m_subscriptionLookup.end()) {
        return; // 订阅不存在
    }
    
    EventSubscription* subscription = it->second;
    EventType eventType = subscription->eventType;
    Object* receiver = subscription->receiver;
    
    // 标记为非活跃
    subscription->isActive = false;
    m_statistics.activeSubscriberCount--;
    
    // 从查找表中移除
    m_subscriptionLookup.erase(it);
    
    // 从接收者订阅记录中移除
    if (receiver) {
        auto receiverIt = m_receiverSubscriptions.find(receiver);
        if (receiverIt != m_receiverSubscriptions.end()) {
            receiverIt->second.erase(connection.id());
            if (receiverIt->second.empty()) {
                m_receiverSubscriptions.erase(receiverIt);
            }
        }
    }
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Unsubscribed connection ID: " << connection.id() << std::endl;
    }
    
    // 发送订阅移除信号
    subscriptionRemoved.emit(eventType, receiver);
}

void EventBus::unsubscribeAll(Object* receiver) {
    if (!receiver) {
        return;
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    auto receiverIt = m_receiverSubscriptions.find(receiver);
    if (receiverIt == m_receiverSubscriptions.end()) {
        return; // 该接收者没有订阅
    }
    
    // 复制订阅ID列表，避免在迭代时修改容器
    auto subscriptionIds = receiverIt->second;
    
    for (uint64_t subscriptionId : subscriptionIds) {
        auto it = m_subscriptionLookup.find(subscriptionId);
        if (it != m_subscriptionLookup.end()) {
            EventSubscription* subscription = it->second;
            subscription->isActive = false;
            m_statistics.activeSubscriberCount--;
            m_subscriptionLookup.erase(it);
        }
    }
    
    // 清除接收者的所有订阅记录
    m_receiverSubscriptions.erase(receiverIt);
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Unsubscribed all events for receiver: " << receiver << std::endl;
    }
}

void EventBus::unsubscribeAll() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    // 清理所有订阅
    m_typeSubscriptions.clear();
    m_nameSubscriptions.clear();
    m_subscriptionLookup.clear();
    m_receiverSubscriptions.clear();
    
    // 重置统计信息
    m_statistics.subscriberCount = 0;
    m_statistics.activeSubscriberCount = 0;
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Unsubscribed all events" << std::endl;
    }
}

void EventBus::publish(std::unique_ptr<Event> event) {
    if (!event || !m_running.load()) {
        return;
    }
    
    // 设置时间戳
    if (event->timestamp() == 0) {
        event->setTimestamp(Event::getCurrentTimestamp());
    }
    
    if (m_asyncProcessingEnabled.load()) {
        // 异步处理
        std::lock_guard<std::mutex> queueLock(m_queueMutex);
        
        // 检查队列大小限制
        if (m_eventQueue.size() >= m_maxEventQueueSize.load()) {
            if (m_debugMode.load()) {
                std::cout << "[EventBus] Event queue full, dropping event" << std::endl;
            }
            busError.emit("Event queue overflow");
            return;
        }
        
        m_eventQueue.push(std::move(event));
        m_eventCondition.notify_one();
    } else {
        // 同步处理
        processEventInternal(event.get());
    }
    
    m_statistics.totalEventsPublished++;
    
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

std::future<void> EventBus::publishAsync(std::unique_ptr<Event> event) {
    return std::async(std::launch::async, [this, event = std::move(event)]() mutable {
        publish(std::move(event));
    });
}

void EventBus::publishDelayed(std::unique_ptr<Event> event, int delayMs) {
    if (!event || delayMs < 0) {
        return;
    }
    
    auto targetTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);
    
    std::lock_guard<std::mutex> queueLock(m_queueMutex);
    m_delayedEventQueue.push(std::make_pair(targetTime, std::move(event)));
}

void EventBus::processEvents() {
    if (m_paused.load()) {
        return;
    }
    
    // 处理延迟事件
    {
        std::lock_guard<std::mutex> queueLock(m_queueMutex);
        auto now = std::chrono::steady_clock::now();
        
        while (!m_delayedEventQueue.empty() && m_delayedEventQueue.top().first <= now) {
            auto delayedEvent = std::move(m_delayedEventQueue.top().second);
            m_delayedEventQueue.pop();
            
            // 将延迟事件添加到普通队列
            m_eventQueue.push(std::move(delayedEvent));
        }
    }
    
    // 处理普通事件队列
    std::queue<std::unique_ptr<Event>> eventsToProcess;
    {
        std::lock_guard<std::mutex> queueLock(m_queueMutex);
        eventsToProcess.swap(m_eventQueue);
    }
    
    while (!eventsToProcess.empty()) {
        auto event = std::move(eventsToProcess.front());
        eventsToProcess.pop();
        
        processEventInternal(event.get());
    }
    
    // 定期清理非活跃订阅
    auto now = std::chrono::steady_clock::now();
    if (now - m_lastCleanupTime >= CLEANUP_INTERVAL) {
        cleanupInactiveSubscriptions();
        m_lastCleanupTime = now;
    }
}

void EventBus::processEventInternal(Event* event) {
    if (!event || m_paused.load()) {
        return;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // 应用事件过滤器
        if (shouldFilterEvent(event)) {
            return;
        }
        
        // 分发事件
        distributeEvent(event);
        
        // 更新统计信息
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        updateStatistics(event, duration.count() / 1000.0); // 转换为毫秒
        
        // 发送事件处理完成信号
        eventProcessed.emit(event->type(), event);
        
    } catch (const std::exception& e) {
        String errorMsg = String("Event processing error: ") + e.what();
        busError.emit(errorMsg);
        
        if (m_debugMode.load()) {
            std::cout << "[EventBus] " << errorMsg << std::endl;
        }
    }
}

void EventBus::distributeEvent(Event* event) {
    auto subscribers = getSubscribersForEvent(event);
    
    // 按优先级排序
    std::sort(subscribers.begin(), subscribers.end(), 
        [](const EventSubscription* a, const EventSubscription* b) {
            return static_cast<int>(a->priority) < static_cast<int>(b->priority);
        });
    
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
            
            if (m_debugMode.load()) {
                std::cout << "[EventBus] " << errorMsg << std::endl;
            }
        }
    }
}

bool EventBus::shouldFilterEvent(Event* event) {
    for (const auto& filter : m_eventFilters) {
        if (filter->filterEvent(event)) {
            return true; // 事件被过滤
        }
    }
    return false;
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
    
    // 获取名称订阅者
    if (!event->eventName().empty()) {
        auto nameIt = m_nameSubscriptions.find(event->eventName());
        if (nameIt != m_nameSubscriptions.end()) {
            for (const auto& subscription : nameIt->second) {
                if (subscription->isActive) {
                    subscribers.push_back(subscription.get());
                }
            }
        }
    }
    
    return subscribers;
}

void EventBus::cleanupInactiveSubscriptions() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    // 清理类型订阅中的非活跃订阅
    for (auto& [eventType, subscriptions] : m_typeSubscriptions) {
        subscriptions.erase(
            std::remove_if(subscriptions.begin(), subscriptions.end(),
                [](const std::unique_ptr<EventSubscription>& sub) {
                    return !sub->isActive;
                }),
            subscriptions.end()
        );
    }
    
    // 清理名称订阅中的非活跃订阅
    for (auto& [eventName, subscriptions] : m_nameSubscriptions) {
        subscriptions.erase(
            std::remove_if(subscriptions.begin(), subscriptions.end(),
                [](const std::unique_ptr<EventSubscription>& sub) {
                    return !sub->isActive;
                }),
            subscriptions.end()
        );
    }
    
    // 更新统计信息
    size_t totalSubscriptions = 0;
    for (const auto& [eventType, subscriptions] : m_typeSubscriptions) {
        totalSubscriptions += subscriptions.size();
    }
    for (const auto& [eventName, subscriptions] : m_nameSubscriptions) {
        totalSubscriptions += subscriptions.size();
    }
    
    m_statistics.subscriberCount = totalSubscriptions;
}

uint64_t EventBus::generateSubscriptionId() {
    return m_nextSubscriptionId.fetch_add(1);
}

void EventBus::startWorkerThreads() {
    size_t threadCount = m_workerThreadCount.load();
    
    for (size_t i = 0; i < threadCount; ++i) {
        m_workerThreads.emplace_back(&EventBus::workerThreadLoop, this);
    }
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] Started " << threadCount << " worker threads" << std::endl;
    }
}

void EventBus::stopWorkerThreads() {
    // 等待所有工作线程结束
    for (auto& thread : m_workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    m_workerThreads.clear();
    
    if (m_debugMode.load()) {
        std::cout << "[EventBus] All worker threads stopped" << std::endl;
    }
}

void EventBus::workerThreadLoop() {
    while (m_running.load() && !m_shouldStop.load()) {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        
        // 等待事件或停止信号
        m_eventCondition.wait(lock, [this] {
            return !m_eventQueue.empty() || m_shouldStop.load() || !m_running.load();
        });
        
        if (m_shouldStop.load() || !m_running.load()) {
            break;
        }
        
        // 处理队列中的事件
        while (!m_eventQueue.empty() && !m_paused.load()) {
            auto event = std::move(m_eventQueue.front());
            m_eventQueue.pop();
            
            lock.unlock();
            processEventInternal(event.get());
            lock.lock();
        }
    }
}

void EventBus::updateStatistics(Event* event, double processingTime) {
    m_statistics.totalEventsProcessed++;
    
    // 更新平均处理时间
    if (m_statistics.totalEventsProcessed == 1) {
        m_statistics.averageProcessingTime = processingTime;
    } else {
        m_statistics.averageProcessingTime = 
            (m_statistics.averageProcessingTime * (m_statistics.totalEventsProcessed - 1) + processingTime) /
            m_statistics.totalEventsProcessed;
    }
}

EventBusStatistics EventBus::getStatistics() const {
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    EventBusStatistics stats = m_statistics;
    
    // 更新当前队列大小
    {
        std::lock_guard<std::mutex> queueLock(m_queueMutex);
        stats.pendingEventsCount = m_eventQueue.size() + m_delayedEventQueue.size();
    }
    
    return stats;
}

void EventBus::resetStatistics() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_statistics.reset();
}

void EventBus::clearEventQueue() {
    std::lock_guard<std::mutex> queueLock(m_queueMutex);
    
    while (!m_eventQueue.empty()) {
        m_eventQueue.pop();
    }
    
    while (!m_delayedEventQueue.empty()) {
        m_delayedEventQueue.pop();
    }
}

size_t EventBus::eventQueueSize() const {
    std::lock_guard<std::mutex> queueLock(m_queueMutex);
    return m_eventQueue.size() + m_delayedEventQueue.size();
}

void EventBus::addEventFilter(std::unique_ptr<EventFilter> filter) {
    if (!filter) {
        return;
    }
    
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_eventFilters.push_back(std::move(filter));
}

void EventBus::removeEventFilter(const String& filterName) {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    m_eventFilters.erase(
        std::remove_if(m_eventFilters.begin(), m_eventFilters.end(),
            [&filterName](const std::unique_ptr<EventFilter>& filter) {
                return filter->filterName() == filterName;
            }),
        m_eventFilters.end()
    );
}

void EventBus::clearEventFilters() {
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_eventFilters.clear();
}

} // namespace QGL