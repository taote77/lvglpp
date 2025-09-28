#ifndef QGL_EVENTS_LVGL_ADAPTER_H
#define QGL_EVENTS_LVGL_ADAPTER_H

#include "Event.h"
#include "EventFactory.h"
#include "../core/Types.h"
#include <lvgl.h>
#include <unordered_map>
#include <functional>
#include <memory>

namespace QGL {

// 前向声明
class Widget;
class EventBus;

/**
 * @brief LVGL 9.3 事件适配器
 * 
 * 负责在 LVGL 底层事件和 QGL 事件系统之间进行转换和适配，
 * 支持 LVGL 9.3 的新特性和改进。
 */
class LvglEventAdapter {
public:
    /**
     * @brief LVGL事件到QGL事件的映射表
     */
    static const std::unordered_map<lv_event_code_t, EventType>& getEventMappingTable();
    
    /**
     * @brief 将 LVGL 事件转换为 QGL 事件
     */
    static std::unique_ptr<Event> convertLvglEvent(lv_event_t* lvglEvent);
    
    /**
     * @brief 将 QGL 事件转换为 LVGL 事件（如果可能）
     */
    static lv_event_t* convertToLvglEvent(const Event& qglEvent);
    
    /**
     * @brief 注册 LVGL 对象的事件处理
     */
    static void registerLvglObject(lv_obj_t* obj, Widget* widget);
    
    /**
     * @brief 取消注册 LVGL 对象
     */
    static void unregisterLvglObject(lv_obj_t* obj);
    
    /**
     * @brief 检查 LVGL 对象是否已注册
     */
    static bool isLvglObjectRegistered(lv_obj_t* obj);
    
    /**
     * @brief 获取与 LVGL 对象关联的 QGL Widget
     */
    static Widget* getAssociatedWidget(lv_obj_t* obj);
    
    /**
     * @brief LVGL 9.3 增强特性支持
     */
    
    // 新的对象生命周期管理
    static void bindObjectLifecycle(lv_obj_t* obj, Widget* widget);
    static void unbindObjectLifecycle(lv_obj_t* obj);
    
    // 新的事件传播机制
    static void enableEventPropagation(lv_obj_t* obj, bool enable = true);
    static bool isEventPropagationEnabled(lv_obj_t* obj);
    
    // 内存管理优化
    static void optimizeEventMemory(lv_obj_t* obj);
    
    // 线程安全支持
    static void enableThreadSafeEvents(bool enable = true);
    static bool isThreadSafeEventsEnabled();

private:
    // LVGL 全局事件回调
    static void globalLvglEventCallback(lv_event_t* e);
    
    // 对象特定的事件回调
    static void objectLvglEventCallback(lv_event_t* e);
    
    // 初始化适配器
    static void initializeAdapter();
    
    // 清理适配器
    static void cleanupAdapter();
    
    // 内部数据结构
    struct ObjectBinding {
        Widget* widget;
        lv_obj_t* obj;
        bool propagationEnabled;
        std::chrono::steady_clock::time_point bindTime;
    };
    
    static std::unordered_map<lv_obj_t*, ObjectBinding> s_objectBindings;
    static std::unordered_map<lv_event_code_t, EventType> s_eventMappingTable;
    static bool s_adapterInitialized;
    static bool s_threadSafeEnabled;
    
    friend class Widget;
    friend class EventBus;
};

/**
 * @brief LVGL 事件包装器
 * 
 * 封装 LVGL 事件，提供类型安全的访问接口
 */
class LvglEvent : public Event {
public:
    explicit LvglEvent(lv_event_t* lvglEvent);
    ~LvglEvent() override = default;
    
    // LVGL 事件信息
    lv_event_code_t lvglEventCode() const { return m_lvglEventCode; }
    lv_obj_t* lvglTarget() const { return m_lvglTarget; }
    void* lvglUserData() const { return m_lvglUserData; }
    
    // LVGL 9.3 新特性
    lv_obj_t* lvglCurrentTarget() const { return m_lvglCurrentTarget; }
    uint32_t lvglEventKey() const { return m_lvglEventKey; }
    
    // 输入设备信息
    lv_indev_t* getInputDevice() const;
    Point getInputPosition() const;
    lv_indev_type_t getInputDeviceType() const;
    
    // 事件参数获取
    template<typename T>
    T getLvglParam() const {
        return *static_cast<T*>(lv_event_get_param(m_originalLvglEvent));
    }
    
    // 原始 LVGL 事件访问（谨慎使用）
    lv_event_t* originalLvglEvent() const { return m_originalLvglEvent; }
    
    std::unique_ptr<Event> clone() const override;
    String className() const override { return "LvglEvent"; }

private:
    lv_event_t* m_originalLvglEvent;
    lv_event_code_t m_lvglEventCode;
    lv_obj_t* m_lvglTarget;
    lv_obj_t* m_lvglCurrentTarget;
    void* m_lvglUserData;
    uint32_t m_lvglEventKey;
    
    void extractLvglEventData(lv_event_t* lvglEvent);
};

/**
 * @brief LVGL 输入事件适配器
 * 
 * 专门处理 LVGL 输入设备事件的转换和适配
 */
class LvglInputAdapter {
public:
    /**
     * @brief 转换 LVGL 输入事件为 QGL 输入事件
     */
    static std::unique_ptr<InputEvent> convertInputEvent(lv_event_t* lvglEvent);
    
    /**
     * @brief 创建鼠标事件
     */
    static std::unique_ptr<MouseEvent> createMouseEvent(lv_event_t* lvglEvent);
    
    /**
     * @brief 创建键盘事件
     */
    static std::unique_ptr<KeyEvent> createKeyEvent(lv_event_t* lvglEvent);
    
    /**
     * @brief 创建触摸事件
     */
    static std::unique_ptr<InputEvent> createTouchEvent(lv_event_t* lvglEvent);
    
    /**
     * @brief 获取输入设备信息
     */
    static String getInputDeviceInfo(lv_indev_t* indev);
    
    /**
     * @brief LVGL 9.3 手势支持
     */
    static std::unique_ptr<InputEvent> createGestureEvent(lv_event_t* lvglEvent);
    
    /**
     * @brief 多点触控支持
     */
    static std::unique_ptr<InputEvent> createMultiTouchEvent(lv_event_t* lvglEvent);

private:
    // 输入设备状态缓存
    struct InputDeviceState {
        Point lastPosition;
        bool pressed;
        uint32_t lastEventTime;
        int clickCount;
    };
    
    static std::unordered_map<lv_indev_t*, InputDeviceState> s_inputStates;
    
    // 辅助方法
    static EventType mapLvglInputEventToQgl(lv_event_code_t lvglCode, lv_indev_type_t indevType);
    static Point getLvglInputPosition(lv_event_t* lvglEvent);
    static int getMouseButton(lv_event_t* lvglEvent);
    static int getKeyCode(lv_event_t* lvglEvent);
    static String getKeyText(lv_event_t* lvglEvent);
};

/**
 * @brief LVGL 对象生命周期管理器
 * 
 * 基于 LVGL 9.3 的改进对象模型，提供对象生命周期事件管理
 */
class LvglObjectLifecycleManager {
public:
    /**
     * @brief 绑定对象生命周期
     */
    static void bindObject(lv_obj_t* obj, Widget* widget);
    
    /**
     * @brief 解绑对象生命周期
     */
    static void unbindObject(lv_obj_t* obj);
    
    /**
     * @brief 对象创建回调
     */
    static void onObjectCreated(lv_obj_t* obj, Widget* widget);
    
    /**
     * @brief 对象销毁回调
     */
    static void onObjectDestroying(lv_obj_t* obj);
    
    /**
     * @brief 检查对象有效性
     */
    static bool isObjectValid(lv_obj_t* obj);
    
    /**
     * @brief 获取对象创建时间
     */
    static uint64_t getObjectCreateTime(lv_obj_t* obj);
    
    /**
     * @brief LVGL 9.3 内存优化
     */
    static void optimizeObjectMemory(lv_obj_t* obj);
    
    /**
     * @brief 对象引用计数管理
     */
    static void incrementReference(lv_obj_t* obj);
    static void decrementReference(lv_obj_t* obj);
    static int getReferenceCount(lv_obj_t* obj);

private:
    struct ObjectInfo {
        Widget* widget;
        uint64_t createTime;
        int referenceCount;
        bool valid;
    };
    
    static std::unordered_map<lv_obj_t*, ObjectInfo> s_objectInfos;
    static void cleanupInvalidObjects();
};

/**
 * @brief LVGL 9.3 特性增强
 * 
 * 针对 LVGL 9.3 的新特性提供专门的适配和增强功能
 */
class LvglEnhancedFeatures {
public:
    /**
     * @brief 新的窗口系统支持
     */
    static void enableEnhancedWindows(bool enable = true);
    static bool isEnhancedWindowsEnabled();
    
    /**
     * @brief 改进的动画系统集成
     */
    static void registerAnimationCallbacks();
    static void unregisterAnimationCallbacks();
    
    /**
     * @brief 新的样式系统支持
     */
    static void enableDynamicStyles(lv_obj_t* obj, bool enable = true);
    static bool isDynamicStylesEnabled(lv_obj_t* obj);
    
    /**
     * @brief 改进的绘制系统
     */
    static void optimizeDrawing(lv_obj_t* obj);
    static void enableAsyncDrawing(bool enable = true);
    
    /**
     * @brief 新的缓存系统
     */
    static void enableAdvancedCaching(bool enable = true);
    static void clearObjectCache(lv_obj_t* obj);
    
    /**
     * @brief 性能监控
     */
    static void enablePerformanceMonitoring(bool enable = true);
    static void getObjectPerformanceStats(lv_obj_t* obj);
    
    /**
     * @brief 内存使用优化
     */
    static size_t getObjectMemoryUsage(lv_obj_t* obj);
    static void optimizeObjectMemory(lv_obj_t* obj);
    
    /**
     * @brief 线程安全增强
     */
    static void enableThreadSafety(bool enable = true);
    static bool isThreadSafetyEnabled();

private:
    static bool s_enhancedWindowsEnabled;
    static bool s_dynamicStylesEnabled;
    static bool s_asyncDrawingEnabled;
    static bool s_advancedCachingEnabled;
    static bool s_performanceMonitoringEnabled;
    static bool s_threadSafetyEnabled;
    
    // 性能统计
    struct PerformanceStats {
        uint64_t drawCount;
        uint64_t totalDrawTime;
        uint64_t eventCount;
        uint64_t totalEventTime;
        size_t memoryUsage;
    };
    
    static std::unordered_map<lv_obj_t*, PerformanceStats> s_performanceStats;
};

// 便捷宏定义

/**
 * @brief 注册 LVGL 对象事件处理
 */
#define QGL_REGISTER_LVGL_OBJECT(obj, widget) \
    QGL::LvglEventAdapter::registerLvglObject(obj, widget)

/**
 * @brief 取消注册 LVGL 对象
 */
#define QGL_UNREGISTER_LVGL_OBJECT(obj) \
    QGL::LvglEventAdapter::unregisterLvglObject(obj)

/**
 * @brief 启用 LVGL 9.3 增强特性
 */
#define QGL_ENABLE_LVGL_ENHANCED_FEATURES() \
    do { \
        QGL::LvglEnhancedFeatures::enableEnhancedWindows(true); \
        QGL::LvglEnhancedFeatures::enableAdvancedCaching(true); \
        QGL::LvglEnhancedFeatures::enablePerformanceMonitoring(true); \
    } while(0)

/**
 * @brief 优化 LVGL 对象性能
 */
#define QGL_OPTIMIZE_LVGL_OBJECT(obj) \
    do { \
        QGL::LvglEnhancedFeatures::optimizeDrawing(obj); \
        QGL::LvglEnhancedFeatures::optimizeObjectMemory(obj); \
        QGL::LvglObjectLifecycleManager::optimizeObjectMemory(obj); \
    } while(0)

} // namespace QGL

#endif // QGL_EVENTS_LVGL_ADAPTER_H