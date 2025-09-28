#ifndef QGL_EVENTS_EVENTBUS_H
#define QGL_EVENTS_EVENTBUS_H

#include "Event.h"
#include "../core/Object.h"
#include "../signals/Signal.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <atomic>
#include <future>

namespace QGL {

// 前向声明
class EventFilter;
class EventHandler;

/**
 * @brief 事件处理器类型定义
 */
using EventHandlerFunction = std::function<void(Event*)>;

/**
 * @brief EventBus专用连接句柄
 */
class EventBusConnection {
public:
    EventBusConnection() = default;
    explicit EventBusConnection(uint64_t id) : m_id(id), m_valid(true) {}
    
    uint64_t id() const { return m_id; }
    bool isValid() const { return m_valid.load(); }
    void disconnect();
    
private:
    uint64_t m_id = 0;
    std::atomic<bool> m_valid{false};
    friend class EventBus;
};
struct EventSubscription {
    uint64_t id;
    EventType eventType;
    String eventName;
    EventHandlerFunction handler;
    Object* receiver = nullptr;
    EventPriority priority = EventPriority::Normal;
    bool isActive = true;
    std::chrono::steady_clock::time_point subscribeTime;
    
    EventSubscription(uint64_t id, EventType type, EventHandlerFunction func, Object* recv = nullptr)
        : id(id), eventType(type), handler(std::move(func)), receiver(recv),
          subscribeTime(std::chrono::steady_clock::now()) {}
          
    EventSubscription(uint64_t id, const String& name, EventHandlerFunction func, Object* recv = nullptr)
        : id(id), eventType(EventType::None), eventName(name), handler(std::move(func)), receiver(recv),
          subscribeTime(std::chrono::steady_clock::now()) {}
};

/**
 * @brief 事件过滤器接口
 */
class EventFilter {
public:
    virtual ~EventFilter() = default;
    virtual bool filterEvent(Event* event) = 0;
    virtual String filterName() const = 0;
};

/**
 * @brief 事件总线统计信息
 */
struct EventBusStatistics {
    size_t subscriberCount = 0;
    size_t activeSubscriberCount = 0;
    size_t totalEventsPublished = 0;
    size_t totalEventsProcessed = 0;
    size_t pendingEventsCount = 0;
    double averageProcessingTime = 0.0;
    std::chrono::steady_clock::time_point startTime;
    
    void reset() {
        subscriberCount = 0;
        activeSubscriberCount = 0;
        totalEventsPublished = 0;
        totalEventsProcessed = 0;
        pendingEventsCount = 0;
        averageProcessingTime = 0.0;
        startTime = std::chrono::steady_clock::now();
    }
};

/**
 * @brief 事件总线核心类
 * 
 * EventBus是QGL事件系统的核心，负责事件的订阅、发布、分发和管理。
 * 
 * 主要功能：
 * - 事件订阅管理：支持类型订阅、名称订阅、优先级订阅
 * - 事件发布机制：同步/异步发布、批量发布、延迟发布
 * - 事件分发策略：优先级分发、条件分发、广播分发
 * - 生命周期管理：自动清理、内存优化、线程安全
 * 
 * 设计特点：
 * - 单例模式：全局唯一的事件总线实例
 * - 线程安全：支持多线程环境下的并发访问
 * - 高性能：优化的事件分发算法和内存管理
 * - 灵活性：支持自定义事件过滤器和处理器
 */
class EventBus : public Object {
public:
    /**
     * @brief 获取事件总线单例实例
     */
    static EventBus* instance();
    
    /**
     * @brief 销毁事件总线实例
     */
    static void destroyInstance();
    
    // 生命周期管理
    void start();
    void stop();
    void pause();
    void resume();
    bool isRunning() const { return m_running.load(); }
    bool isPaused() const { return m_paused.load(); }
    
    // 事件订阅 - 按类型订阅
    EventBusConnection subscribe(EventType eventType, EventHandlerFunction handler, Object* receiver = nullptr);
    EventBusConnection subscribe(EventType eventType, EventHandlerFunction handler, EventPriority priority, Object* receiver = nullptr);
    
    // 事件订阅 - 按名称订阅
    EventBusConnection subscribe(const String& eventName, EventHandlerFunction handler, Object* receiver = nullptr);
    EventBusConnection subscribe(const String& eventName, EventHandlerFunction handler, EventPriority priority, Object* receiver = nullptr);
    
    // 事件订阅 - Lambda 便捷方法
    template<typename T>
    EventBusConnection subscribe(EventType eventType, T&& handler, Object* receiver = nullptr) {
        return subscribe(eventType, EventHandlerFunction(std::forward<T>(handler)), receiver);
    }
    
    template<typename T>
    EventBusConnection subscribe(const String& eventName, T&& handler, Object* receiver = nullptr) {
        return subscribe(eventName, EventHandlerFunction(std::forward<T>(handler)), receiver);
    }
    
    // 事件取消订阅
    void unsubscribe(const EventBusConnection& connection);
    void unsubscribe(EventType eventType, Object* receiver = nullptr);
    void unsubscribe(const String& eventName, Object* receiver = nullptr);
    void unsubscribeAll(Object* receiver);
    void unsubscribeAll(EventType eventType);
    void unsubscribeAll(const String& eventName);
    void unsubscribeAll();
    
    // 事件发布 - 同步发布
    void publish(std::unique_ptr<Event> event);
    void publish(EventType eventType);
    void publish(EventType eventType, const Variant& payload);
    void publish(const String& eventName);
    void publish(const String& eventName, const Variant& payload);
    
    // 事件发布 - 异步发布
    std::future<void> publishAsync(std::unique_ptr<Event> event);
    std::future<void> publishAsync(EventType eventType);
    std::future<void> publishAsync(EventType eventType, const Variant& payload);
    std::future<void> publishAsync(const String& eventName);
    std::future<void> publishAsync(const String& eventName, const Variant& payload);
    
    // 事件发布 - 延迟发布
    void publishDelayed(std::unique_ptr<Event> event, int delayMs);
    void publishDelayed(EventType eventType, int delayMs);
    void publishDelayed(EventType eventType, const Variant& payload, int delayMs);
    void publishDelayed(const String& eventName, int delayMs);
    void publishDelayed(const String& eventName, const Variant& payload, int delayMs);
    
    // 批量事件发布
    void publishBatch(std::vector<std::unique_ptr<Event>> events);
    
    // 事件过滤器管理
    void addEventFilter(std::unique_ptr<EventFilter> filter);
    void removeEventFilter(const String& filterName);
    void clearEventFilters();
    std::vector<String> getEventFilterNames() const;
    
    // 事件队列管理
    void processEvents();
    void processEventsFor(int maxTimeMs);
    void clearEventQueue();
    size_t eventQueueSize() const;
    bool hasQueuedEvents() const { return eventQueueSize() > 0; }
    
    // 统计信息
    EventBusStatistics getStatistics() const;
    void resetStatistics();
    
    // 调试和监控
    void enableDebugMode(bool enable = true) { m_debugMode = enable; }
    bool isDebugModeEnabled() const { return m_debugMode; }
    void setMaxEventQueueSize(size_t maxSize) { m_maxEventQueueSize = maxSize; }
    size_t getMaxEventQueueSize() const { return m_maxEventQueueSize; }
    
    // 线程安全配置
    void enableThreadSafety(bool enable = true) { m_threadSafetyEnabled = enable; }
    bool isThreadSafetyEnabled() const { return m_threadSafetyEnabled; }
    
    // 性能优化配置  
    void enableAsyncProcessing(bool enable = true) { m_asyncProcessingEnabled = enable; }
    bool isAsyncProcessingEnabled() const { return m_asyncProcessingEnabled; }
    
    void setWorkerThreadCount(size_t count) { m_workerThreadCount = count; }
    size_t getWorkerThreadCount() const { return m_workerThreadCount; }
    
    // 信号支持
    SIGNAL(eventPublished, EventType, Event*);
    SIGNAL(eventProcessed, EventType, Event*);
    SIGNAL(subscriptionAdded, EventType, Object*);
    SIGNAL(subscriptionRemoved, EventType, Object*);
    SIGNAL(busStarted);
    SIGNAL(busStopped);
    SIGNAL(busError, const String&);
    
public:
    EventBus();
    virtual ~EventBus();
    
    // 禁用拷贝和移动
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&) = delete;
    EventBus& operator=(EventBus&&) = delete;

private:
    // 内部实现方法
    void initializeEventBus();
    void shutdownEventBus();
    void processEventInternal(Event* event);
    void distributeEvent(Event* event);
    bool shouldFilterEvent(Event* event);
    std::vector<EventSubscription*> getSubscribersForEvent(Event* event);
    void cleanupInactiveSubscriptions();
    uint64_t generateSubscriptionId();
    void startWorkerThreads();
    void stopWorkerThreads();
    void workerThreadLoop();
    void updateStatistics(Event* event, double processingTime);
    
    // 静态实例
    static std::unique_ptr<EventBus> s_instance;
    static std::mutex s_instanceMutex;
    
    // 线程安全
    mutable std::shared_mutex m_mutex;
    std::mutex m_queueMutex;
    std::condition_variable m_eventCondition;
    
    // 订阅管理
    std::unordered_map<EventType, std::vector<std::unique_ptr<EventSubscription>>> m_typeSubscriptions;
    std::unordered_map<String, std::vector<std::unique_ptr<EventSubscription>>> m_nameSubscriptions;
    std::unordered_map<uint64_t, EventSubscription*> m_subscriptionLookup;
    std::unordered_map<Object*, std::unordered_set<uint64_t>> m_receiverSubscriptions;
    
    // 事件队列
    std::queue<std::unique_ptr<Event>> m_eventQueue;
    std::priority_queue<std::pair<std::chrono::steady_clock::time_point, std::unique_ptr<Event>>> m_delayedEventQueue;
    
    // 事件过滤器
    std::vector<std::unique_ptr<EventFilter>> m_eventFilters;
    
    // 工作线程
    std::vector<std::thread> m_workerThreads;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_paused{false};
    std::atomic<bool> m_shouldStop{false};
    
    // 配置选项
    std::atomic<bool> m_threadSafetyEnabled{true};
    std::atomic<bool> m_asyncProcessingEnabled{false};
    std::atomic<bool> m_debugMode{false};
    std::atomic<size_t> m_maxEventQueueSize{10000};
    std::atomic<size_t> m_workerThreadCount{1};
    
    // 统计信息
    mutable EventBusStatistics m_statistics;
    std::atomic<uint64_t> m_nextSubscriptionId{1};
    
    // 性能监控
    std::chrono::steady_clock::time_point m_lastCleanupTime;
    static constexpr std::chrono::seconds CLEANUP_INTERVAL{30};
};

} // namespace QGL

#endif // QGL_EVENTS_EVENTBUS_H