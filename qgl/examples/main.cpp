#include "examples/EventSystemExample.h"
#include <iostream>

/**
 * @brief QGL 事件系统示例主程序
 * 
 * 演示 QGL 事件机制扩展系统的核心功能：
 * 1. 自定义事件定义和使用
 * 2. 事件发布和订阅
 * 3. 页面生命周期事件
 * 4. 应用程序生命周期管理
 * 5. 性能监控和统计
 */
int main(int argc, char* argv[]) {
    std::cout << "=== QGL 事件系统示例程序 ===" << std::endl;
    std::cout << "基于 LVGL 9.3 的现代化 C++ 事件驱动框架" << std::endl;
    std::cout << "支持自定义事件、生命周期管理、性能监控等高级特性\\n" << std::endl;
    
    try {
        // 创建示例应用程序
        QGL::Examples::EventSystemExampleApp app(argc, argv);
        
        // 配置应用程序
        app.setApplicationName("QGL 事件系统示例");
        app.setApplicationVersion("1.0.0");
        app.setApplicationDescription("演示 QGL 事件系统的各种功能和用法");
        app.setApplicationType(QGL::ApplicationType::Desktop); // 桌面演示模式
        
        // 初始化应用程序
        std::cout << "正在初始化应用程序..." << std::endl;
        if (!app.initialize()) {
            std::cerr << "错误: 应用程序初始化失败" << std::endl;
            return -1;
        }
        
        std::cout << "应用程序初始化成功" << std::endl;
        std::cout << "事件系统已启动，开始处理事件..." << std::endl;
        std::cout << "按 Ctrl+C 退出程序\\n" << std::endl;
        
        // 运行应用程序主循环
        // 注意：在实际的GUI应用中，这里会运行主事件循环
        // 由于这是一个控制台演示，我们运行有限的循环来展示事件处理
        
        // 模拟运行一段时间来展示事件处理
        auto startTime = std::chrono::steady_clock::now();
        const int runDurationSeconds = 5; // 运行5秒
        
        while (true) {
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
            
            if (elapsed.count() >= runDurationSeconds) {
                std::cout << "\\n演示时间结束，正在退出..." << std::endl;
                break;
            }
            
            // 处理事件
            if (auto eventBus = app.eventBus()) {
                eventBus->processEvents(10); // 每次处理最多10个事件
            }
            
            // 模拟frame更新
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
        }
        
        // 退出应用程序
        app.quit(0);
        
        std::cout << "应用程序已正常退出" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "严重错误: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "未知错误发生" << std::endl;
        return -1;
    }
}

/**
 * @brief 打印使用说明
 */
void printUsage() {
    std::cout << "QGL 事件系统示例程序使用说明:\\n" << std::endl;
    std::cout << "该程序演示了以下功能:" << std::endl;
    std::cout << "1. 自定义事件的定义和注册" << std::endl;
    std::cout << "2. 类型安全的事件发布和订阅" << std::endl;
    std::cout << "3. 页面生命周期事件管理" << std::endl;
    std::cout << "4. 应用程序生命周期管理" << std::endl;
    std::cout << "5. 事件过滤和优先级处理" << std::endl;
    std::cout << "6. 性能监控和统计分析" << std::endl;
    std::cout << "7. LVGL 9.3 事件系统集成" << std::endl;
    std::cout << "\\n运行参数:" << std::endl;
    std::cout << "  --help          显示此帮助信息" << std::endl;
    std::cout << "  --debug         启用调试模式" << std::endl;
    std::cout << "  --log-level N   设置日志级别 (0-4)" << std::endl;
    std::cout << "  --stats         启用详细统计信息" << std::endl;
    std::cout << "\\n示例:" << std::endl;
    std::cout << "  ./event_system_example --debug --log-level 3 --stats" << std::endl;
    std::cout << std::endl;
}