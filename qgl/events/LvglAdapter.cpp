#include "LvglAdapter.h"
#include "EventBus.h"
#include "MouseEvent.h"
#include "KeyEvent.h"
#include "FocusEvent.h"
#include "../widgets/Widget.h"
#include <iostream>
#include <chrono>
#include <mutex>

namespace QGL {

// LvglEventAdapter 静态成员定义
std::unordered_map<lv_obj_t*, LvglEventAdapter::ObjectBinding> LvglEventAdapter::s_objectBindings;
std::unordered_map<lv_event_code_t, EventType> LvglEventAdapter::s_eventMappingTable;
bool LvglEventAdapter::s_adapterInitialized = false;
bool LvglEventAdapter::s_threadSafeEnabled = false;

// LvglInputAdapter 静态成员定义
std::unordered_map<lv_indev_t*, LvglInputAdapter::InputDeviceState> LvglInputAdapter::s_inputStates;

// LvglObjectLifecycleManager 静态成员定义
std::unordered_map<lv_obj_t*, LvglObjectLifecycleManager::ObjectInfo> LvglObjectLifecycleManager::s_objectInfos;

// LvglEnhancedFeatures 静态成员定义
bool LvglEnhancedFeatures::s_enhancedWindowsEnabled = false;
bool LvglEnhancedFeatures::s_dynamicStylesEnabled = false;
bool LvglEnhancedFeatures::s_asyncDrawingEnabled = false;
bool LvglEnhancedFeatures::s_advancedCachingEnabled = false;
bool LvglEnhancedFeatures::s_performanceMonitoringEnabled = false;
bool LvglEnhancedFeatures::s_threadSafetyEnabled = false;
std::unordered_map<lv_obj_t*, LvglEnhancedFeatures::PerformanceStats> LvglEnhancedFeatures::s_performanceStats;

// 线程安全支持
static std::mutex s_adapterMutex;

const std::unordered_map<lv_event_code_t, EventType>& LvglEventAdapter::getEventMappingTable() {
    if (s_eventMappingTable.empty()) {
        // 初始化 LVGL 到 QGL 事件映射表
        s_eventMappingTable[LV_EVENT_PRESSED] = EventType::MousePress;
        s_eventMappingTable[LV_EVENT_RELEASED] = EventType::MouseRelease;
        s_eventMappingTable[LV_EVENT_CLICKED] = EventType::MouseClick;
        s_eventMappingTable[LV_EVENT_LONG_PRESSED] = EventType::MouseDoubleClick;
        s_eventMappingTable[LV_EVENT_KEY] = EventType::KeyPress;
        s_eventMappingTable[LV_EVENT_FOCUSED] = EventType::Focus;
        s_eventMappingTable[LV_EVENT_DEFOCUSED] = EventType::FocusOut;
        s_eventMappingTable[LV_EVENT_VALUE_CHANGED] = EventType::Widget;
        s_eventMappingTable[LV_EVENT_READY] = EventType::PageCompleted;
        s_eventMappingTable[LV_EVENT_DELETE] = EventType::PageDestroyed;
        s_eventMappingTable[LV_EVENT_DRAW_MAIN] = EventType::Paint;
        s_eventMappingTable[LV_EVENT_DRAW_POST] = EventType::Paint;
        s_eventMappingTable[LV_EVENT_SIZE_CHANGED] = EventType::Resize;
        s_eventMappingTable[LV_EVENT_STYLE_CHANGED] = EventType::Widget;
        s_eventMappingTable[LV_EVENT_LAYOUT_CHANGED] = EventType::Widget;
        
        // LVGL 9.3 新增事件类型
        s_eventMappingTable[LV_EVENT_HIT_TEST] = EventType::Input;
        s_eventMappingTable[LV_EVENT_COVER_CHECK] = EventType::Widget;
        s_eventMappingTable[LV_EVENT_REFR_EXT_DRAW_SIZE] = EventType::Paint;
        s_eventMappingTable[LV_EVENT_DRAW_MAIN_BEGIN] = EventType::Paint;
        s_eventMappingTable[LV_EVENT_DRAW_MAIN_END] = EventType::Paint;
        s_eventMappingTable[LV_EVENT_DRAW_POST_BEGIN] = EventType::Paint;
        s_eventMappingTable[LV_EVENT_DRAW_POST_END] = EventType::Paint;
    }
    return s_eventMappingTable;
}

std::unique_ptr<Event> LvglEventAdapter::convertLvglEvent(lv_event_t* lvglEvent) {
    if (!lvglEvent) {
        return nullptr;
    }
    
    lv_event_code_t eventCode = lv_event_get_code(lvglEvent);
    const auto& mappingTable = getEventMappingTable();
    
    auto it = mappingTable.find(eventCode);
    if (it == mappingTable.end()) {
        // 创建通用的 LVGL 事件
        return std::make_unique<LvglEvent>(lvglEvent);
    }
    
    EventType qglEventType = it->second;
    
    // 根据事件类型创建相应的 QGL 事件
    switch (qglEventType) {
        case EventType::MousePress:
        case EventType::MouseRelease:
        case EventType::MouseClick:
        case EventType::MouseDoubleClick:
            return LvglInputAdapter::createMouseEvent(lvglEvent);
            
        case EventType::KeyPress:
        case EventType::KeyRelease:
            return LvglInputAdapter::createKeyEvent(lvglEvent);
            
        case EventType::TouchPress:
        case EventType::TouchRelease:
        case EventType::TouchMove:
            return LvglInputAdapter::createTouchEvent(lvglEvent);
            
        default:
            // 创建通用事件
            auto event = std::make_unique<Event>(qglEventType);
            event->setTimestamp(Event::getCurrentTimestamp());
            
            // 设置事件源
            lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(lvglEvent));
            if (target) {
                Widget* widget = getAssociatedWidget(target);
                if (widget) {
                    event->setSource(widget);
                }
            }
            
            return std::unique_ptr<Event>(event.release());
    }
}

void LvglEventAdapter::registerLvglObject(lv_obj_t* obj, Widget* widget) {
    if (!obj || !widget) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(s_adapterMutex);
    
    if (!s_adapterInitialized) {
        initializeAdapter();
    }
    
    // 创建绑定信息
    ObjectBinding binding;
    binding.widget = widget;
    binding.obj = obj;
    binding.propagationEnabled = true;
    binding.bindTime = std::chrono::steady_clock::now();
    
    s_objectBindings[obj] = binding;
    
    // 添加 LVGL 事件回调
    lv_obj_add_event_cb(obj, objectLvglEventCallback, LV_EVENT_ALL, widget);
    
    // 绑定对象生命周期
    LvglObjectLifecycleManager::bindObject(obj, widget);
}

void LvglEventAdapter::unregisterLvglObject(lv_obj_t* obj) {
    if (!obj) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(s_adapterMutex);
    
    auto it = s_objectBindings.find(obj);
    if (it != s_objectBindings.end()) {
        // 移除 LVGL 事件回调
        lv_obj_remove_event_cb(obj, objectLvglEventCallback);
        
        // 解绑对象生命周期
        LvglObjectLifecycleManager::unbindObject(obj);
        
        // 移除绑定
        s_objectBindings.erase(it);
    }
}

bool LvglEventAdapter::isLvglObjectRegistered(lv_obj_t* obj) {
    std::lock_guard<std::mutex> lock(s_adapterMutex);
    return s_objectBindings.find(obj) != s_objectBindings.end();
}

Widget* LvglEventAdapter::getAssociatedWidget(lv_obj_t* obj) {
    std::lock_guard<std::mutex> lock(s_adapterMutex);
    
    auto it = s_objectBindings.find(obj);
    return (it != s_objectBindings.end()) ? it->second.widget : nullptr;
}

void LvglEventAdapter::bindObjectLifecycle(lv_obj_t* obj, Widget* widget) {
    LvglObjectLifecycleManager::bindObject(obj, widget);
}

void LvglEventAdapter::unbindObjectLifecycle(lv_obj_t* obj) {
    LvglObjectLifecycleManager::unbindObject(obj);
}

void LvglEventAdapter::enableEventPropagation(lv_obj_t* obj, bool enable) {
    std::lock_guard<std::mutex> lock(s_adapterMutex);
    
    auto it = s_objectBindings.find(obj);
    if (it != s_objectBindings.end()) {
        it->second.propagationEnabled = enable;
    }
}

bool LvglEventAdapter::isEventPropagationEnabled(lv_obj_t* obj) {
    std::lock_guard<std::mutex> lock(s_adapterMutex);
    
    auto it = s_objectBindings.find(obj);
    return (it != s_objectBindings.end()) ? it->second.propagationEnabled : false;
}

void LvglEventAdapter::optimizeEventMemory(lv_obj_t* obj) {
    // LVGL 9.3 内存优化
    if (obj) {
        // 清理不必要的事件缓存
        LvglEnhancedFeatures::clearObjectCache(obj);
        LvglEnhancedFeatures::optimizeObjectMemory(obj);
    }
}

void LvglEventAdapter::enableThreadSafeEvents(bool enable) {
    s_threadSafeEnabled = enable;
}

bool LvglEventAdapter::isThreadSafeEventsEnabled() {
    return s_threadSafeEnabled;
}

void LvglEventAdapter::globalLvglEventCallback(lv_event_t* e) {
    // 全局事件处理
    if (!e) return;
    
    lv_obj_t* target = static_cast<lv_obj_t*>(lv_event_get_target(e));
    if (!target) return;
    
    // 转换为 QGL 事件
    auto qglEvent = convertLvglEvent(e);
    if (qglEvent && EventBus::instance()) {
        EventBus::instance()->publish(std::move(qglEvent));
    }
}

void LvglEventAdapter::objectLvglEventCallback(lv_event_t* e) {
    if (!e) return;
    
    lv_obj_t* target = lv_event_get_target(e);
    Widget* widget = static_cast<Widget*>(lv_event_get_user_data(e));
    
    if (!target || !widget) return;
    
    // 检查事件传播
    if (!isEventPropagationEnabled(target)) {
        return;
    }
    
    // 性能监控
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 转换事件并发布
    auto qglEvent = convertLvglEvent(e);
    if (qglEvent) {
        qglEvent->setSource(widget);
        
        if (EventBus::instance()) {
            EventBus::instance()->publish(std::move(qglEvent));
        }
    }
    
    // 更新性能统计
    if (LvglEnhancedFeatures::s_performanceMonitoringEnabled) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        auto& stats = LvglEnhancedFeatures::s_performanceStats[target];
        stats.eventCount++;
        stats.totalEventTime += duration.count();
    }
}

void LvglEventAdapter::initializeAdapter() {
    if (s_adapterInitialized) {
        return;
    }
    
    // 初始化事件映射表
    getEventMappingTable();
    
    // 注册全局事件回调
    // 注意：LVGL 9.3 可能需要不同的全局事件注册方式
    
    s_adapterInitialized = true;
}

void LvglEventAdapter::cleanupAdapter() {
    std::lock_guard<std::mutex> lock(s_adapterMutex);
    
    // 清理所有绑定
    for (auto& [obj, binding] : s_objectBindings) {
        lv_obj_remove_event_cb(obj, objectLvglEventCallback);
        LvglObjectLifecycleManager::unbindObject(obj);
    }
    
    s_objectBindings.clear();
    s_adapterInitialized = false;
}

// LvglEvent 实现
LvglEvent::LvglEvent(lv_event_t* lvglEvent) 
    : Event(EventType::LvglEvent)
    , m_originalLvglEvent(lvglEvent) {
    
    if (lvglEvent) {
        extractLvglEventData(lvglEvent);
    }
}

void LvglEvent::extractLvglEventData(lv_event_t* lvglEvent) {
    m_lvglEventCode = lv_event_get_code(lvglEvent);
    m_lvglTarget = lv_event_get_target(lvglEvent);
    m_lvglCurrentTarget = lv_event_get_current_target(lvglEvent);
    m_lvglUserData = lv_event_get_user_data(lvglEvent);
    
    // LVGL 9.3 新特性
    // m_lvglEventKey = lv_event_get_key(lvglEvent); // 如果有的话
}

lv_indev_t* LvglEvent::getInputDevice() const {
    return lv_indev_get_act();
}

Point LvglEvent::getInputPosition() const {
    lv_point_t point;
    lv_indev_get_point(lv_indev_get_act(), &point);
    return Point(point.x, point.y);
}

lv_indev_type_t LvglEvent::getInputDeviceType() const {
    lv_indev_t* indev = getInputDevice();
    return indev ? lv_indev_get_type(indev) : LV_INDEV_TYPE_NONE;
}

std::unique_ptr<Event> LvglEvent::clone() const {
    // 注意：LVGL 事件可能不能完全克隆，因为原始 lv_event_t 可能已经无效
    auto cloned = std::make_unique<LvglEvent>(nullptr);
    cloned->m_lvglEventCode = m_lvglEventCode;
    cloned->m_lvglTarget = m_lvglTarget;
    cloned->m_lvglCurrentTarget = m_lvglCurrentTarget;
    cloned->m_lvglUserData = m_lvglUserData;
    cloned->m_lvglEventKey = m_lvglEventKey;
    
    // 复制基础事件属性
    cloned->setEventName(eventName());
    cloned->setPayload(payload());
    cloned->setTimestamp(timestamp());
    cloned->setSource(source());
    cloned->setPriority(priority());
    
    return std::unique_ptr<Event>(cloned.release());
}

// LvglInputAdapter 实现
std::unique_ptr<InputEvent> LvglInputAdapter::convertInputEvent(lv_event_t* lvglEvent) {
    if (!lvglEvent) {
        return nullptr;
    }
    
    lv_indev_t* indev = lv_indev_get_act();
    if (!indev) {
        return nullptr;
    }
    
    lv_indev_type_t indevType = lv_indev_get_type(indev);
    lv_event_code_t eventCode = lv_event_get_code(lvglEvent);
    
    switch (indevType) {
        case LV_INDEV_TYPE_POINTER:
            return createMouseEvent(lvglEvent);
            
        case LV_INDEV_TYPE_KEYPAD:
            return createKeyEvent(lvglEvent);
            
        default:
            return createTouchEvent(lvglEvent);
    }
}

std::unique_ptr<MouseEvent> LvglInputAdapter::createMouseEvent(lv_event_t* lvglEvent) {
    lv_event_code_t eventCode = lv_event_get_code(lvglEvent);
    Point position = getLvglInputPosition(lvglEvent);
    int button = getMouseButton(lvglEvent);
    
    EventType qglEventType;
    switch (eventCode) {
        case LV_EVENT_PRESSED:
            qglEventType = EventType::MousePress;
            break;
        case LV_EVENT_RELEASED:
            qglEventType = EventType::MouseRelease;
            break;
        case LV_EVENT_CLICKED:
            qglEventType = EventType::MouseClick;
            break;
        case LV_EVENT_LONG_PRESSED:
            qglEventType = EventType::MouseDoubleClick;
            break;
        default:
            qglEventType = EventType::MouseMove;
            break;
    }
    
    auto mouseEvent = std::make_unique<MouseEvent>(qglEventType, position, button);
    mouseEvent->setTimestamp(Event::getCurrentTimestamp());
    
    return mouseEvent;
}

std::unique_ptr<KeyEvent> LvglInputAdapter::createKeyEvent(lv_event_t* lvglEvent) {
    lv_event_code_t eventCode = lv_event_get_code(lvglEvent);
    int keyCode = getKeyCode(lvglEvent);
    String keyText = getKeyText(lvglEvent);
    
    EventType qglEventType = (eventCode == LV_EVENT_KEY) ? EventType::KeyPress : EventType::KeyRelease;
    
    auto keyEvent = std::make_unique<KeyEvent>(qglEventType, keyCode, keyText);
    keyEvent->setTimestamp(Event::getCurrentTimestamp());
    
    return keyEvent;
}

std::unique_ptr<InputEvent> LvglInputAdapter::createTouchEvent(lv_event_t* lvglEvent) {
    lv_event_code_t eventCode = lv_event_get_code(lvglEvent);
    
    EventType qglEventType;
    switch (eventCode) {
        case LV_EVENT_PRESSED:
            qglEventType = EventType::TouchPress;
            break;
        case LV_EVENT_RELEASED:
            qglEventType = EventType::TouchRelease;
            break;
        default:
            qglEventType = EventType::TouchMove;
            break;
    }
    
    auto touchEvent = std::make_unique<InputEvent>(qglEventType);
    touchEvent->setTimestamp(Event::getCurrentTimestamp());
    
    Point position = getLvglInputPosition(lvglEvent);
    touchEvent->setAttribute("position", Variant::fromPoint(position));
    
    return touchEvent;
}

Point LvglInputAdapter::getLvglInputPosition(lv_event_t* lvglEvent) {
    lv_point_t point;
    lv_indev_get_point(lv_indev_get_act(), &point);
    return Point(point.x, point.y);
}

int LvglInputAdapter::getMouseButton(lv_event_t* lvglEvent) {
    // LVGL 中通常只有一个主按钮
    return 1; // 左键
}

int LvglInputAdapter::getKeyCode(lv_event_t* lvglEvent) {
    uint32_t key = lv_event_get_key(lvglEvent);
    return static_cast<int>(key);
}

String LvglInputAdapter::getKeyText(lv_event_t* lvglEvent) {
    uint32_t key = lv_event_get_key(lvglEvent);
    
    // 简单的键码到字符转换
    if (key >= 32 && key <= 126) {
        return String(1, static_cast<char>(key));
    }
    
    return String();
}

// LvglObjectLifecycleManager 实现
void LvglObjectLifecycleManager::bindObject(lv_obj_t* obj, Widget* widget) {
    if (!obj || !widget) {
        return;
    }
    
    ObjectInfo info;
    info.widget = widget;
    info.createTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    info.referenceCount = 1;
    info.valid = true;
    
    s_objectInfos[obj] = info;
    
    // 添加销毁事件回调
    lv_obj_add_event_cb(obj, [](lv_event_t* e) {
        if (lv_event_get_code(e) == LV_EVENT_DELETE) {
            lv_obj_t* obj = lv_event_get_target(e);
            onObjectDestroying(obj);
        }
    }, LV_EVENT_DELETE, nullptr);
}

void LvglObjectLifecycleManager::unbindObject(lv_obj_t* obj) {
    auto it = s_objectInfos.find(obj);
    if (it != s_objectInfos.end()) {
        it->second.valid = false;
        s_objectInfos.erase(it);
    }
}

void LvglObjectLifecycleManager::onObjectCreated(lv_obj_t* obj, Widget* widget) {
    bindObject(obj, widget);
}

void LvglObjectLifecycleManager::onObjectDestroying(lv_obj_t* obj) {
    auto it = s_objectInfos.find(obj);
    if (it != s_objectInfos.end()) {
        it->second.valid = false;
        
        // 发布对象销毁事件
        if (EventBus::instance()) {
            auto destroyEvent = EventFactory::createEvent(EventType::PageDestroyed);
            destroyEvent->setSource(it->second.widget);
            EventBus::instance()->publish(std::move(destroyEvent));
        }
    }
}

bool LvglObjectLifecycleManager::isObjectValid(lv_obj_t* obj) {
    auto it = s_objectInfos.find(obj);
    return (it != s_objectInfos.end()) && it->second.valid;
}

uint64_t LvglObjectLifecycleManager::getObjectCreateTime(lv_obj_t* obj) {
    auto it = s_objectInfos.find(obj);
    return (it != s_objectInfos.end()) ? it->second.createTime : 0;
}

// LvglEnhancedFeatures 实现
void LvglEnhancedFeatures::enableEnhancedWindows(bool enable) {
    s_enhancedWindowsEnabled = enable;
}

bool LvglEnhancedFeatures::isEnhancedWindowsEnabled() {
    return s_enhancedWindowsEnabled;
}

void LvglEnhancedFeatures::enableDynamicStyles(lv_obj_t* obj, bool enable) {
    s_dynamicStylesEnabled = enable;
    // 实际实现需要与 LVGL 9.3 的样式系统集成
}

void LvglEnhancedFeatures::optimizeDrawing(lv_obj_t* obj) {
    if (!obj) return;
    
    // LVGL 9.3 绘制优化
    // 这里需要调用 LVGL 9.3 的具体优化 API
}

void LvglEnhancedFeatures::enableAdvancedCaching(bool enable) {
    s_advancedCachingEnabled = enable;
}

void LvglEnhancedFeatures::clearObjectCache(lv_obj_t* obj) {
    if (!obj) return;
    
    // 清理对象缓存
    lv_obj_invalidate(obj);
}

void LvglEnhancedFeatures::enablePerformanceMonitoring(bool enable) {
    s_performanceMonitoringEnabled = enable;
    if (!enable) {
        s_performanceStats.clear();
    }
}

size_t LvglEnhancedFeatures::getObjectMemoryUsage(lv_obj_t* obj) {
    if (!obj) return 0;
    
    // 估算对象内存使用量
    // 这需要与 LVGL 9.3 的内存管理系统集成
    return sizeof(lv_obj_t); // 简化实现
}

void LvglEnhancedFeatures::optimizeObjectMemory(lv_obj_t* obj) {
    if (!obj) return;
    
    // LVGL 9.3 内存优化
    // 清理不必要的缓存和资源
    clearObjectCache(obj);
}

void LvglEnhancedFeatures::enableThreadSafety(bool enable) {
    s_threadSafetyEnabled = enable;
}

bool LvglEnhancedFeatures::isThreadSafetyEnabled() {
    return s_threadSafetyEnabled;
}

} // namespace QGL