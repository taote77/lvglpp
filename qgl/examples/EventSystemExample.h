#ifndef QGL_EXAMPLES_EVENT_SYSTEM_EXAMPLE_H
#define QGL_EXAMPLES_EVENT_SYSTEM_EXAMPLE_H

#include "../QGL.h"
#include "../events/Event.h"
#include "../events/EventFactory.h"
#include "../events/PageEvents.h"
#include "../events/LvglAdapter.h"

namespace QGL {
namespace Examples {

/**
 * @brief 自定义用户事件示例
 */

// 1. 定义简单的自定义事件
QGL_DEFINE_EVENT(UserLoginEvent, 10001, "user.login");
QGL_DEFINE_EVENT(DataSavedEvent, 10002, "data.saved");
QGL_DEFINE_EVENT(NetworkErrorEvent, 10003, "network.error");

// 2. 定义复杂的自定义事件
QGL_DEFINE_COMPLEX_EVENT(SensorDataEvent, 10004, "sensor.data")
public:
    SensorDataEvent(int sensorId, float value, const String& unit)
        : CustomEvent(10004, "sensor.data") {
        setSensorId(sensorId);
        setValue(value);
        setUnit(unit);
    }
    
    int getSensorId() const { return getAttribute("sensorId").toInt(); }
    void setSensorId(int id) { setAttribute("sensorId", id); }
    
    float getValue() const { return getAttribute("value").toFloat(); }
    void setValue(float value) { setAttribute("value", value); }
    
    String getUnit() const { return getAttribute("unit").toString(); }
    void setUnit(const String& unit) { setAttribute("unit", unit); }
    
    String className() const override { return "SensorDataEvent"; }
};

// 3. 定义业务逻辑事件
QGL_DEFINE_COMPLEX_EVENT(OrderCreatedEvent, 10005, "order.created")
public:
    OrderCreatedEvent(const String& orderId, double amount, const String& customerId)
        : CustomEvent(10005, "order.created") {
        setOrderId(orderId);
        setAmount(amount);
        setCustomerId(customerId);
    }
    
    String getOrderId() const { return getAttribute("orderId").toString(); }
    void setOrderId(const String& id) { setAttribute("orderId", id); }
    
    double getAmount() const { return getAttribute("amount").toDouble(); }
    void setAmount(double amount) { setAttribute("amount", amount); }
    
    String getCustomerId() const { return getAttribute("customerId").toString(); }
    void setCustomerId(const String& id) { setAttribute("customerId", id); }
    
    String className() const override { return "OrderCreatedEvent"; }
};

/**
 * @brief 示例页面类，演示页面生命周期事件
 */
class ExamplePage : public Page {
    QGL_OBJECT(ExamplePage)

public:
    explicit ExamplePage(Widget* parent = nullptr) : Page(parent) {
        setPageId("example_page");
        setTitle("事件系统示例页面");
        
        // 订阅自定义事件
        setupEventSubscriptions();
    }
    
    ~ExamplePage() override {
        // 清理事件订阅
        cleanupEventSubscriptions();
    }

protected:
    void onCompleted() override {
        Page::onCompleted();
        
        // 发布页面完成事件
        QGL_PUBLISH_PAGE_EVENT(this, PageLifecycleEventType::PageCompleted);
        
        // 发布自定义页面事件
        QGL_PUBLISH_PAGE_CUSTOM_EVENT(this, "pageReady", Variant("页面已准备就绪"));
        
        // 模拟数据加载
        loadPageData();
    }
    
    void onDestroying() override {
        // 发布页面销毁事件
        QGL_PUBLISH_PAGE_EVENT(this, PageLifecycleEventType::PageDestroying);
        
        Page::onDestroying();
    }

private:
    std::vector<EventSubscription> m_subscriptions;
    
    void setupEventSubscriptions() {
        auto eventBus = EventBus::instance();
        if (!eventBus) return;
        
        // 订阅用户登录事件
        m_subscriptions.push_back(
            eventBus->subscribe<UserLoginEvent>([this](const UserLoginEvent& event) {
                handleUserLogin(event);
            })
        );
        
        // 订阅传感器数据事件
        m_subscriptions.push_back(
            eventBus->subscribe<SensorDataEvent>([this](const SensorDataEvent& event) {
                handleSensorData(event);
            })
        );
        
        // 订阅订单创建事件
        m_subscriptions.push_back(
            eventBus->subscribe<OrderCreatedEvent>([this](const OrderCreatedEvent& event) {
                handleOrderCreated(event);
            })
        );
        
        // 订阅页面数据加载事件
        m_subscriptions.push_back(
            eventBus->subscribe("page.dataLoaded", [this](const Event& event) {
                if (auto dataEvent = dynamic_cast<const PageDataEvent*>(&event)) {
                    handlePageDataLoaded(*dataEvent);
                }
            })
        );
        
        // 订阅网络错误事件（条件订阅）
        m_subscriptions.push_back(
            eventBus->subscribe<NetworkErrorEvent>(
                [](const NetworkErrorEvent& event) -> bool {
                    // 只处理严重的网络错误
                    return event.getAttribute("severity").toString() == "critical";
                },
                [this](const NetworkErrorEvent& event) {
                    handleCriticalNetworkError(event);
                }
            )
        );
    }
    
    void cleanupEventSubscriptions() {
        auto eventBus = EventBus::instance();
        if (!eventBus) return;
        
        for (auto& subscription : m_subscriptions) {
            eventBus->unsubscribe(subscription);
        }
        m_subscriptions.clear();
    }
    
    void loadPageData() {
        // 模拟异步数据加载
        auto dataEvent = PageEventFactory::createDataEvent(
            PageLifecycleEventType::PageDataLoaded, 
            this, 
            Variant("模拟的页面数据")
        );
        dataEvent->setDataType("pageContent");
        dataEvent->setAsyncLoad(true);
        dataEvent->setLoadProgress(100);
        
        if (auto eventBus = EventBus::instance()) {
            eventBus->publish(std::move(dataEvent));
        }
    }
    
    // 事件处理方法
    void handleUserLogin(const UserLoginEvent& event) {
        std::cout << "用户登录事件处理: " << event.toString() << std::endl;
        
        // 发布数据保存事件
        auto saveEvent = std::make_unique<DataSavedEvent>();
        saveEvent->setAttribute("dataType", "userSession");
        saveEvent->setAttribute("userId", event.getAttribute("userId"));
        
        if (auto eventBus = EventBus::instance()) {
            eventBus->publish(std::move(saveEvent));
        }
    }
    
    void handleSensorData(const SensorDataEvent& event) {
        std::cout << "传感器数据事件: ID=" << event.getSensorId() 
                  << ", Value=" << event.getValue() 
                  << " " << event.getUnit() << std::endl;
        
        // 检查阈值
        if (event.getValue() > 80.0f) {
            // 发布告警事件
            auto alarmEvent = EventFactory::createCustomEvent("sensor.alarm");
            alarmEvent->setAttribute("sensorId", event.getSensorId());
            alarmEvent->setAttribute("currentValue", event.getValue());
            alarmEvent->setAttribute("threshold", 80.0f);
            alarmEvent->setPriority(EventPriority::High);
            
            if (auto eventBus = EventBus::instance()) {
                eventBus->publish(std::move(alarmEvent));
            }
        }
    }
    
    void handleOrderCreated(const OrderCreatedEvent& event) {
        std::cout << "订单创建事件: ID=" << event.getOrderId() 
                  << ", Amount=" << event.getAmount() 
                  << ", Customer=" << event.getCustomerId() << std::endl;
        
        // 发布数据保存事件
        auto saveEvent = std::make_unique<DataSavedEvent>();
        saveEvent->setAttribute("dataType", "order");
        saveEvent->setAttribute("orderId", event.getOrderId());
        
        if (auto eventBus = EventBus::instance()) {
            eventBus->publish(std::move(saveEvent));
        }
    }
    
    void handlePageDataLoaded(const PageDataEvent& event) {
        std::cout << "页面数据加载完成: " << event.data().toString() << std::endl;
        
        // 更新UI
        updatePageContent(event.data());
    }
    
    void handleCriticalNetworkError(const NetworkErrorEvent& event) {
        std::cout << "严重网络错误: " << event.toString() << std::endl;
        
        // 显示错误提示
        showErrorMessage("网络连接出现严重问题，请检查网络设置");
    }
    
    void updatePageContent(const Variant& data) {
        // 更新页面内容
        std::cout << "更新页面内容: " << data.toString() << std::endl;
    }
    
    void showErrorMessage(const String& message) {
        // 显示错误消息
        std::cout << "错误消息: " << message << std::endl;
    }
};

/**
 * @brief 事件系统示例应用程序
 */
class EventSystemExampleApp : public Application {
    QGL_OBJECT(EventSystemExampleApp)

public:
    explicit EventSystemExampleApp(int argc = 0, char* argv[] = nullptr) 
        : Application(argc, argv) {
        
        setApplicationName("QGL 事件系统示例");
        setApplicationVersion("1.0.0");
        setApplicationDescription("演示 QGL 事件系统的各种功能和用法");
        setApplicationType(ApplicationType::Embedded);
    }

protected:
    bool onInitialize() override {
        if (!Application::onInitialize()) {
            return false;
        }
        
        // 注册自定义事件
        registerCustomEvents();
        
        // 设置事件总线配置
        setupEventBus();
        
        // 创建示例页面
        createExamplePage();
        
        // 设置事件统计和日志
        enableEventMonitoring();
        
        // 模拟一些事件
        simulateEvents();
        
        return true;
    }
    
    void onShutdown() override {
        // 打印事件统计信息
        printEventStatistics();
        
        Application::onShutdown();
    }

private:
    std::unique_ptr<ExamplePage> m_examplePage;
    
    void registerCustomEvents() {
        auto registry = EventRegistry::instance();
        
        // 注册自定义事件
        QGL_REGISTER_EVENT(UserLoginEvent, "user.login");
        QGL_REGISTER_EVENT(DataSavedEvent, "data.saved");
        QGL_REGISTER_EVENT(NetworkErrorEvent, "network.error");
        QGL_REGISTER_EVENT(SensorDataEvent, "sensor.data");
        QGL_REGISTER_EVENT(OrderCreatedEvent, "order.created");
        
        std::cout << "已注册 " << registry->getAllEventNames().size() << " 个自定义事件类型" << std::endl;
    }
    
    void setupEventBus() {
        auto eventBus = this->eventBus();
        if (!eventBus) return;
        
        // 启用事件统计
        eventBus->setStatisticsEnabled(true);
        
        // 启用事件日志
        eventBus->setLoggingEnabled(true);
        eventBus->setLogLevel(3); // 信息级别
        
        // 设置异步处理
        eventBus->setProcessingMode(EventBus::ProcessingMode::Asynchronous);
        eventBus->startAsyncProcessing();
        
        // 添加全局事件过滤器
        eventBus->addGlobalFilter([](const Event& event) -> bool {
            // 过滤掉一些调试事件
            return event.eventName() != "debug.trace";
        });
    }
    
    void createExamplePage() {
        m_examplePage = std::make_unique<ExamplePage>();
        
        // 设置为主页面
        if (auto pageManager = this->pageManager()) {
            pageManager->registerPage("example", [this]() -> Page* {
                return m_examplePage.get();
            });
            
            pageManager->navigateTo("example");
        }
    }
    
    void enableEventMonitoring() {
        auto eventBus = this->eventBus();
        if (!eventBus) return;
        
        // 监控事件发布
        eventBus->eventPublished.connect([](const Event& event) {
            std::cout << "[监控] 事件已发布: " << event.eventName() << std::endl;
        });
        
        // 监控队列大小变化
        eventBus->queueSizeChanged.connect([](size_t size) {
            if (size > 50) {
                std::cout << "[警告] 事件队列大小过大: " << size << std::endl;
            }
        });
    }
    
    void simulateEvents() {
        auto eventBus = this->eventBus();
        if (!eventBus) return;
        
        std::cout << "\\n开始模拟事件..." << std::endl;
        
        // 1. 模拟用户登录
        auto loginEvent = std::make_unique<UserLoginEvent>();
        loginEvent->setAttribute("userId", "user123");
        loginEvent->setAttribute("userName", "张三");
        loginEvent->setAttribute("loginTime", Event::getCurrentTimestamp());
        eventBus->publish(std::move(loginEvent));
        
        // 2. 模拟传感器数据
        for (int i = 0; i < 5; ++i) {
            auto sensorEvent = std::make_unique<SensorDataEvent>(
                i + 1, 
                75.0f + i * 5.0f, 
                "°C"
            );
            eventBus->publish(std::move(sensorEvent));
        }
        
        // 3. 模拟订单创建
        auto orderEvent = std::make_unique<OrderCreatedEvent>(
            "ORD20240101001", 
            299.99, 
            "CUST123456"
        );
        eventBus->publish(std::move(orderEvent));
        
        // 4. 模拟网络错误
        auto networkEvent = std::make_unique<NetworkErrorEvent>();
        networkEvent->setAttribute("errorCode", 500);
        networkEvent->setAttribute("errorMessage", "服务器内部错误");
        networkEvent->setAttribute("severity", "critical");
        eventBus->publish(std::move(networkEvent));
        
        // 5. 模拟页面自定义事件
        QGL_PUBLISH_PAGE_CUSTOM_EVENT(m_examplePage.get(), "userAction", Variant("点击按钮"));
        
        std::cout << "事件模拟完成\\n" << std::endl;
    }
    
    void printEventStatistics() {
        auto eventBus = this->eventBus();
        if (!eventBus || !eventBus->isStatisticsEnabled()) {
            return;
        }
        
        const auto& stats = eventBus->getStatistics();
        
        std::cout << "\\n=== 事件系统统计信息 ===" << std::endl;
        std::cout << "总发布事件数: " << stats.totalEventsPublished << std::endl;
        std::cout << "总处理事件数: " << stats.totalEventsProcessed << std::endl;
        std::cout << "丢弃事件数: " << stats.totalEventsDropped << std::endl;
        std::cout << "当前队列大小: " << stats.currentQueueSize << std::endl;
        std::cout << "最大队列大小: " << stats.maxQueueSize << std::endl;
        std::cout << "平均处理时间: " << stats.averageProcessingTime << " ms" << std::endl;
        std::cout << "最大处理时间: " << stats.maxProcessingTime << " ms" << std::endl;
        
        std::cout << "\\n按事件类型统计:" << std::endl;
        for (const auto& [eventType, count] : stats.eventTypeCount) {
            std::cout << "  事件类型 " << eventType << ": " << count << " 次" << std::endl;
        }
        
        std::cout << "\\n按事件名称统计:" << std::endl;
        for (const auto& [eventName, count] : stats.eventNameCount) {
            std::cout << "  事件名称 \"" << eventName << "\": " << count << " 次" << std::endl;
        }
        
        std::cout << "========================\\n" << std::endl;
    }
};

} // namespace Examples
} // namespace QGL

#endif // QGL_EXAMPLES_EVENT_SYSTEM_EXAMPLE_H