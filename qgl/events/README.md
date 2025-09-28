# QGL 事件机制扩展系统 - 开发文档

## 概述

QGL 事件机制扩展系统是基于 LVGL 9.3 构建的现代化 C++ 事件驱动架构。系统的核心特性是**支持自定义事件**，允许开发者轻松定义和处理业务相关的事件，同时提供完整的应用程序生命周期管理、页面生命周期事件和基础的系统事件支持。

## 核心特性

### 🎯 自定义事件支持
- **类型安全**：编译时类型检查，避免运行时错误
- **灵活定义**：支持简单事件和复杂事件两种定义方式
- **动态注册**：运行时注册自定义事件类型
- **工厂模式**：统一的事件创建和管理

### 🔄 生命周期管理
- **应用程序生命周期**：初始化、运行、睡眠、暂停、停止
- **页面生命周期**：创建、初始化、激活、暂停、销毁
- **嵌入式特性**：电源管理、看门狗、资源监控

### 🚀 高性能事件总线
- **异步处理**：支持同步和异步事件处理模式
- **优先级队列**：事件优先级管理和调度
- **事件过滤**：全局和局部事件过滤机制
- **统计监控**：详细的性能统计和监控

### 🔗 LVGL 9.3 深度集成
- **事件适配**：自动转换 LVGL 事件到 QGL 事件
- **对象生命周期**：智能管理 LVGL 对象生命周期
- **内存优化**：基于 LVGL 9.3 的内存管理优化
- **线程安全**：多线程环境下的安全事件处理

## 快速开始

### 1. 基础设置

```cpp
#include "QGL.h"
#include "examples/EventSystemExample.h"

int main() {
    // 创建应用程序
    QGL::Examples::EventSystemExampleApp app(argc, argv);
    
    // 初始化并运行
    if (!app.initialize()) {
        return -1;
    }
    
    return app.exec();
}
```

### 2. 定义自定义事件

#### 简单事件定义
```cpp
// 使用宏定义简单事件
QGL_DEFINE_EVENT(UserLoginEvent, 10001, "user.login");
QGL_DEFINE_EVENT(DataSavedEvent, 10002, "data.saved");
QGL_DEFINE_EVENT(NetworkErrorEvent, 10003, "network.error");
```

#### 复杂事件定义
```cpp
// 定义包含复杂数据的事件
QGL_DEFINE_COMPLEX_EVENT(SensorDataEvent, 10004, "sensor.data")
public:
    SensorDataEvent(int sensorId, float value, const String& unit)
        : CustomEvent(10004, "sensor.data") {
        setSensorId(sensorId);
        setValue(value);
        setUnit(unit);
    }
    
    // 获取器和设置器
    int getSensorId() const { return getAttribute("sensorId").toInt(); }
    void setSensorId(int id) { setAttribute("sensorId", id); }
    
    float getValue() const { return getAttribute("value").toFloat(); }
    void setValue(float value) { setAttribute("value", value); }
    
    String getUnit() const { return getAttribute("unit").toString(); }
    void setUnit(const String& unit) { setAttribute("unit", unit); }
    
    String className() const override { return "SensorDataEvent"; }
};
```

### 3. 事件发布

#### 创建和发布事件
```cpp
// 方式 1: 直接创建事件对象
auto loginEvent = std::make_unique<UserLoginEvent>();
loginEvent->setAttribute("userId", "user123");
loginEvent->setAttribute("userName", "张三");
EventBus::instance()->publish(std::move(loginEvent));

// 方式 2: 使用工厂方法
auto sensorEvent = std::make_unique<SensorDataEvent>(1, 25.5f, "°C");
EventBus::instance()->publish(std::move(sensorEvent));

// 方式 3: 便捷方法
EventBus::instance()->publish("user.login", userLoginData);
EventBus::instance()->publish(EventType::Custom, customData);
```

### 4. 事件订阅

#### 类型安全订阅
```cpp
// 订阅特定类型的事件
auto subscription = EventBus::instance()->subscribe<UserLoginEvent>(
    [](const UserLoginEvent& event) {
        std::cout << "用户登录: " << event.getAttribute("userName").toString() << std::endl;
    }
);

// 带过滤条件的订阅
auto filteredSubscription = EventBus::instance()->subscribe<SensorDataEvent>(
    [](const SensorDataEvent& event) -> bool {
        // 只处理温度超过30度的传感器数据
        return event.getValue() > 30.0f;
    },
    [](const SensorDataEvent& event) {
        std::cout << "高温警告: " << event.getValue() << event.getUnit() << std::endl;
    }
);
```

#### 按名称订阅
```cpp
// 按事件名称订阅
auto nameSubscription = EventBus::instance()->subscribe(
    "user.login",
    [](const Event& event) {
        // 处理用户登录事件
        handleUserLogin(event);
    }
);
```

#### 对象方法订阅
```cpp
class MyWidget : public Widget {
public:
    void setupEventHandlers() {
        // 订阅到成员函数
        auto subscription = EventBus::instance()->subscribe(
            this, &MyWidget::onUserLogin
        );
    }
    
private:
    void onUserLogin(const UserLoginEvent& event) {
        // 处理用户登录
    }
};
```

### 5. 页面生命周期事件

#### 页面事件处理
```cpp
class MyPage : public Page {
    QGL_OBJECT(MyPage)
    
protected:
    void onCompleted() override {
        Page::onCompleted();
        
        // 发布页面完成事件
        QGL_PUBLISH_PAGE_EVENT(this, PageLifecycleEventType::PageCompleted);
        
        // 发布自定义页面事件
        QGL_PUBLISH_PAGE_CUSTOM_EVENT(this, "pageReady", pageData);
    }
    
    void onDestroying() override {
        // 发布页面销毁事件
        QGL_PUBLISH_PAGE_EVENT(this, PageLifecycleEventType::PageDestroying);
        
        Page::onDestroying();
    }
};
```

#### 订阅页面事件
```cpp
// 订阅页面生命周期事件
EventBus::instance()->subscribe<PageLifecycleEvent>(
    [](const PageLifecycleEvent& event) {
        switch (event.lifecycleType()) {
            case PageLifecycleEventType::PageCompleted:
                std::cout << "页面加载完成: " << event.pageId() << std::endl;
                break;
            case PageLifecycleEventType::PageDestroying:
                std::cout << "页面即将销毁: " << event.pageId() << std::endl;
                break;
        }
    }
);
```

## 高级功能

### 1. 事件优先级

```cpp
// 设置事件优先级
auto criticalEvent = std::make_unique<NetworkErrorEvent>();
criticalEvent->setPriority(EventPriority::Critical);
EventBus::instance()->publish(std::move(criticalEvent));

// 按优先级订阅
auto highPrioritySubscription = EventBus::instance()->subscribe<NetworkErrorEvent>(
    [](const NetworkErrorEvent& event) {
        // 高优先级处理
    },
    EventPriority::High
);
```

### 2. 事件过滤

```cpp
// 添加全局过滤器
EventBus::instance()->addGlobalFilter([](const Event& event) -> bool {
    // 过滤掉调试事件
    return !event.eventName().startsWith("debug.");
});

// 局部过滤器
auto conditionalSubscription = EventBus::instance()->subscribe<SensorDataEvent>(
    [](const SensorDataEvent& event) -> bool {
        // 只处理特定传感器的数据
        return event.getSensorId() == 1;
    },
    [](const SensorDataEvent& event) {
        // 处理传感器1的数据
    }
);
```

### 3. 异步处理模式

```cpp
// 设置异步处理模式
auto eventBus = EventBus::instance();
eventBus->setProcessingMode(EventBus::ProcessingMode::Asynchronous);
eventBus->startAsyncProcessing();

// 配置队列大小
eventBus->setMaxQueueSize(10000);

// 手动处理事件（同步模式下）
eventBus->processEvents(100); // 处理最多100个事件
```

### 4. 性能监控

```cpp
// 启用统计功能
EventBus::instance()->setStatisticsEnabled(true);

// 获取统计信息
const auto& stats = EventBus::instance()->getStatistics();
std::cout << "总发布事件数: " << stats.totalEventsPublished << std::endl;
std::cout << "平均处理时间: " << stats.averageProcessingTime << " ms" << std::endl;

// 启用事件日志
EventBus::instance()->setLoggingEnabled(true);
EventBus::instance()->setLogLevel(3); // 信息级别
```

### 5. LVGL 集成

```cpp
// 注册 LVGL 对象
lv_obj_t* button = lv_btn_create(parent);
MyButton* qglButton = new MyButton();

// 绑定 LVGL 对象到 QGL 控件
QGL_REGISTER_LVGL_OBJECT(button, qglButton);

// 启用 LVGL 9.3 增强特性
QGL_ENABLE_LVGL_ENHANCED_FEATURES();

// 优化 LVGL 对象性能
QGL_OPTIMIZE_LVGL_OBJECT(button);
```

## 应用程序生命周期

### 嵌入式应用程序示例

```cpp
class EmbeddedApp : public Application {
    QGL_OBJECT(EmbeddedApp)
    
public:
    EmbeddedApp(int argc, char* argv[]) : Application(argc, argv) {
        // 配置嵌入式应用特性
        setApplicationType(ApplicationType::Embedded);
        setPowerManagementEnabled(true);
        setWatchdogEnabled(true);
        setResourceMonitoringEnabled(true);
    }
    
protected:
    bool onInitialize() override {
        if (!Application::onInitialize()) {
            return false;
        }
        
        // 初始化硬件
        initializeHardware();
        
        // 设置电源管理
        setupPowerManagement();
        
        return true;
    }
    
    void onLowBattery(double batteryLevel) override {
        if (batteryLevel < 10.0) {
            // 进入省电模式
            sleep();
        }
    }
    
    void onLowMemory(size_t availableMemory) override {
        // 清理缓存
        clearUnusedCache();
        
        // 发布内存警告事件
        auto memoryEvent = EventFactory::createCustomEvent("system.memory.warning");
        memoryEvent->setAttribute("availableMemory", static_cast<uint64_t>(availableMemory));
        eventBus()->publish(std::move(memoryEvent));
    }
};

// 使用便捷宏创建主函数
QGL_EMBEDDED_APPLICATION_MAIN(EmbeddedApp)
```

## 最佳实践

### 1. 事件设计原则

- **单一职责**：每个事件类型只负责一种特定的业务逻辑
- **不变性**：事件对象一旦创建就不应该被修改
- **完整性**：事件应该包含处理所需的所有信息
- **可序列化**：事件应该支持序列化以便日志记录和调试

### 2. 性能优化

- **事件池化**：对于频繁创建的事件使用对象池
- **批量处理**：将相关事件批量处理以提高效率
- **智能过滤**：使用过滤器减少不必要的事件处理
- **异步处理**：对于非关键路径使用异步处理

### 3. 错误处理

- **异常隔离**：事件处理器中的异常不应影响其他处理器
- **重试机制**：对于网络事件等可重试的操作实现重试逻辑
- **降级处理**：在系统资源不足时提供降级处理方案

### 4. 调试和测试

```cpp
// 启用调试功能
#ifdef DEBUG
    EventBus::instance()->setLoggingEnabled(true);
    EventBus::instance()->setLogLevel(4); // 调试级别
    
    // 设置事件拦截器用于调试
    EventBus::instance()->setEventInterceptor([](const Event& event) -> bool {
        std::cout << "[调试] 拦截事件: " << event.toString() << std::endl;
        return true; // 继续处理
    });
#endif

// 单元测试示例
class EventSystemTest {
public:
    void testCustomEvent() {
        auto eventBus = EventBus::instance();
        
        bool eventReceived = false;
        auto subscription = eventBus->subscribe<UserLoginEvent>(
            [&eventReceived](const UserLoginEvent& event) {
                eventReceived = true;
            }
        );
        
        // 发布事件
        auto loginEvent = std::make_unique<UserLoginEvent>();
        eventBus->publish(std::move(loginEvent));
        
        // 处理事件
        eventBus->processEvents();
        
        // 验证结果
        assert(eventReceived);
    }
};
```

## 示例项目

完整的示例项目位于 `examples/EventSystemExample.h`，展示了：

1. **自定义事件定义和使用**
2. **页面生命周期事件处理**
3. **应用程序生命周期管理**
4. **LVGL 事件集成**
5. **性能监控和统计**
6. **错误处理和恢复**

运行示例：
```bash
cd examples
mkdir build && cd build
cmake ..
make
./event_system_example
```

## API 参考

### 核心类

- [`Event`](Event.h): 事件基类
- [`EventBus`](EventBus.h): 事件总线
- [`EventFactory`](EventFactory.h): 事件工厂
- [`EventRegistry`](EventFactory.h): 事件注册表
- [`Application`](Application.h): 应用程序基类
- [`PageLifecycleEvent`](PageEvents.h): 页面生命周期事件
- [`LvglEventAdapter`](LvglAdapter.h): LVGL 事件适配器

### 便捷宏

- `QGL_DEFINE_EVENT`: 定义简单事件
- `QGL_DEFINE_COMPLEX_EVENT`: 定义复杂事件
- `QGL_PUBLISH_PAGE_EVENT`: 发布页面事件
- `QGL_REGISTER_LVGL_OBJECT`: 注册 LVGL 对象
- `QGL_EMBEDDED_APPLICATION_MAIN`: 嵌入式应用主函数

## 故障排除

### 常见问题

1. **事件未被处理**
   - 检查事件订阅是否正确
   - 确认 EventBus 实例存在
   - 验证事件类型匹配

2. **内存泄漏**
   - 确保正确取消事件订阅
   - 检查 LVGL 对象绑定清理
   - 使用智能指针管理事件对象

3. **性能问题**
   - 检查事件队列大小
   - 优化事件处理器逻辑
   - 考虑使用事件过滤器

4. **线程安全**
   - 启用线程安全模式
   - 避免在事件处理器中修改共享状态
   - 使用适当的同步机制

### 调试技巧

```cpp
// 1. 启用详细日志
EventBus::instance()->setLoggingEnabled(true);
EventBus::instance()->setLogLevel(4);

// 2. 监控事件流
EventBus::instance()->eventPublished.connect([](const Event& event) {
    std::cout << "发布: " << event.toString() << std::endl;
});

// 3. 检查统计信息
const auto& stats = EventBus::instance()->getStatistics();
if (stats.totalEventsDropped > 0) {
    std::cout << "警告: " << stats.totalEventsDropped << " 个事件被丢弃" << std::endl;
}
```

## 扩展和定制

QGL 事件系统设计为高度可扩展，你可以：

1. **自定义事件类型**：继承 `Event` 或 `CustomEvent` 创建特定的事件类型
2. **扩展事件总线**：继承 `EventBus` 添加自定义功能
3. **实现自定义适配器**：为其他 GUI 库创建事件适配器
4. **添加中间件**：在事件处理管道中添加自定义中间件

---

有关更多信息和高级用法，请参考源代码和示例项目。如有问题，欢迎提交 Issue 或 Pull Request。