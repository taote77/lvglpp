#ifndef QGL_EVENTS_PAGE_EVENTS_H
#define QGL_EVENTS_PAGE_EVENTS_H

#include "Event.h"
#include "EventFactory.h"
#include "../core/Variant.h"
#include <memory>

namespace QGL {

// 前向声明
class Page;

/**
 * @brief 页面生命周期事件类型
 */
enum class PageLifecycleEventType : uint32_t {
    // 页面创建相关 (3000-3099)
    PageCreated = 3001,         // 页面创建完成
    PageInitialized = 3002,     // 页面初始化完成
    PageCompleted = 3003,       // 页面加载完成
    
    // 页面激活相关 (3100-3199)
    PageAboutToActivate = 3101, // 页面即将激活
    PageActivated = 3102,       // 页面已激活
    PageAboutToDeactivate = 3103, // 页面即将失活
    PageDeactivated = 3104,     // 页面已失活
    
    // 页面暂停/恢复 (3200-3299)
    PageAboutToPause = 3201,    // 页面即将暂停
    PagePaused = 3202,          // 页面已暂停
    PageAboutToResume = 3203,   // 页面即将恢复
    PageResumed = 3204,         // 页面已恢复
    
    // 页面销毁相关 (3300-3399)
    PageAboutToDestroy = 3301,  // 页面即将销毁
    PageDestroying = 3302,      // 页面正在销毁
    PageDestroyed = 3303,       // 页面已销毁
    
    // 页面导航相关 (3400-3499)
    PageNavigateFrom = 3401,    // 从页面导航离开
    PageNavigateTo = 3402,      // 导航到页面
    PageNavigateBack = 3403,    // 返回到页面
    PageNavigateForward = 3404, // 前进到页面
    
    // 页面数据相关 (3500-3599)
    PageDataLoaded = 3501,      // 页面数据加载完成
    PageDataChanged = 3502,     // 页面数据变化
    PageDataSaved = 3503,       // 页面数据保存
    PageDataError = 3504,       // 页面数据错误
    
    // 页面UI相关 (3600-3699)
    PageLayoutChanged = 3601,   // 页面布局变化
    PageStyleChanged = 3602,    // 页面样式变化
    PageSizeChanged = 3603,     // 页面尺寸变化
    PageVisibilityChanged = 3604, // 页面可见性变化
    
    // 自定义页面事件 (3700-3999)
    PageCustomEvent = 3700      // 自定义页面事件起始ID
};

/**
 * @brief 页面生命周期事件基类
 */
class PageLifecycleEvent : public Event {
public:
    explicit PageLifecycleEvent(PageLifecycleEventType lifecycleType, Page* page = nullptr)
        : Event(static_cast<EventType>(lifecycleType)), m_page(page), m_lifecycleType(lifecycleType) {
        setEventName(getEventNameFromType(lifecycleType));
    }
    
    // 页面信息
    Page* page() const { return m_page; }
    void setPage(Page* page) { m_page = page; }
    
    PageLifecycleEventType lifecycleType() const { return m_lifecycleType; }
    
    // 页面标识
    String pageId() const { return getAttribute("pageId").toString(); }
    void setPageId(const String& id) { setAttribute("pageId", id); }
    
    // 页面标题
    String pageTitle() const { return getAttribute("pageTitle").toString(); }
    void setPageTitle(const String& title) { setAttribute("pageTitle", title); }
    
    // 导航参数
    std::map<String, Variant> navigationParams() const {
        // 简化实现：返回空map
        return std::map<String, Variant>();
    }
    void setNavigationParams(const std::map<String, Variant>& params) {
        // 简化实现：暂时不保存参数
        // setAttribute("navigationParams", Variant::fromString("params_placeholder"));
    }
    
    // 页面状态
    String pageState() const { return getAttribute("pageState").toString(); }
    void setPageState(const String& state) { setAttribute("pageState", state); }
    
    // 时间戳信息
    uint64_t lifecycleTimestamp() const { return timestamp(); }
    
    std::unique_ptr<Event> clone() const override {
        auto cloned = std::make_unique<PageLifecycleEvent>(m_lifecycleType, m_page);
        cloned->setEventName(eventName());
        cloned->setPayload(payload());
        cloned->setTimestamp(timestamp());
        cloned->setSource(source());
        cloned->setPriority(priority());
        
        // 复制所有属性
        auto serialized = serialize();
        cloned->deserialize(serialized);
        
        return std::unique_ptr<Event>(cloned.release());
    }
    
    String className() const override { return "PageLifecycleEvent"; }

protected:
    static String getEventNameFromType(PageLifecycleEventType type) {
        switch (type) {
            case PageLifecycleEventType::PageCreated: return "page.created";
            case PageLifecycleEventType::PageInitialized: return "page.initialized";
            case PageLifecycleEventType::PageCompleted: return "page.completed";
            case PageLifecycleEventType::PageAboutToActivate: return "page.aboutToActivate";
            case PageLifecycleEventType::PageActivated: return "page.activated";
            case PageLifecycleEventType::PageAboutToDeactivate: return "page.aboutToDeactivate";
            case PageLifecycleEventType::PageDeactivated: return "page.deactivated";
            case PageLifecycleEventType::PageAboutToPause: return "page.aboutToPause";
            case PageLifecycleEventType::PagePaused: return "page.paused";
            case PageLifecycleEventType::PageAboutToResume: return "page.aboutToResume";
            case PageLifecycleEventType::PageResumed: return "page.resumed";
            case PageLifecycleEventType::PageAboutToDestroy: return "page.aboutToDestroy";
            case PageLifecycleEventType::PageDestroying: return "page.destroying";
            case PageLifecycleEventType::PageDestroyed: return "page.destroyed";
            case PageLifecycleEventType::PageNavigateFrom: return "page.navigateFrom";
            case PageLifecycleEventType::PageNavigateTo: return "page.navigateTo";
            case PageLifecycleEventType::PageNavigateBack: return "page.navigateBack";
            case PageLifecycleEventType::PageNavigateForward: return "page.navigateForward";
            case PageLifecycleEventType::PageDataLoaded: return "page.dataLoaded";
            case PageLifecycleEventType::PageDataChanged: return "page.dataChanged";
            case PageLifecycleEventType::PageDataSaved: return "page.dataSaved";
            case PageLifecycleEventType::PageDataError: return "page.dataError";
            case PageLifecycleEventType::PageLayoutChanged: return "page.layoutChanged";
            case PageLifecycleEventType::PageStyleChanged: return "page.styleChanged";
            case PageLifecycleEventType::PageSizeChanged: return "page.sizeChanged";
            case PageLifecycleEventType::PageVisibilityChanged: return "page.visibilityChanged";
            case PageLifecycleEventType::PageCustomEvent: return "page.custom";
            default: return "page.unknown";
        }
    }

private:
    Page* m_page;
    PageLifecycleEventType m_lifecycleType;
};

/**
 * @brief 页面导航事件
 */
class PageNavigationEvent : public PageLifecycleEvent {
public:
    PageNavigationEvent(PageLifecycleEventType navType, Page* fromPage, Page* toPage)
        : PageLifecycleEvent(navType, toPage), m_fromPage(fromPage), m_toPage(toPage) {}
    
    Page* fromPage() const { return m_fromPage; }
    Page* toPage() const { return m_toPage; }
    
    // 导航类型
    String navigationType() const { return getAttribute("navigationType").toString(); }
    void setNavigationType(const String& type) { setAttribute("navigationType", type); }
    
    // 导航原因
    String navigationReason() const { return getAttribute("navigationReason").toString(); }
    void setNavigationReason(const String& reason) { setAttribute("navigationReason", reason); }
    
    // 是否为返回导航
    bool isBackNavigation() const { return getAttribute("isBackNavigation").toBool(); }
    void setBackNavigation(bool isBack) { setAttribute("isBackNavigation", isBack); }
    
    String className() const override { return "PageNavigationEvent"; }

private:
    Page* m_fromPage;
    Page* m_toPage;
};

/**
 * @brief 页面数据事件
 */
class PageDataEvent : public PageLifecycleEvent {
public:
    PageDataEvent(PageLifecycleEventType dataType, Page* page, const Variant& data = Variant())
        : PageLifecycleEvent(dataType, page), m_data(data) {}
    
    const Variant& data() const { return m_data; }
    void setData(const Variant& data) { m_data = data; }
    
    // 数据类型
    String dataType() const { return getAttribute("dataType").toString(); }
    void setDataType(const String& type) { setAttribute("dataType", type); }
    
    // 数据源
    String dataSource() const { return getAttribute("dataSource").toString(); }
    void setDataSource(const String& source) { setAttribute("dataSource", source); }
    
    // 是否为异步加载
    bool isAsyncLoad() const { return getAttribute("isAsyncLoad").toBool(); }
    void setAsyncLoad(bool async) { setAttribute("isAsyncLoad", async); }
    
    // 加载进度（0-100）
    int loadProgress() const { return getAttribute("loadProgress").toInt(); }
    void setLoadProgress(int progress) { setAttribute("loadProgress", progress); }
    
    String className() const override { return "PageDataEvent"; }

private:
    Variant m_data;
};

/**
 * @brief 页面UI事件
 */
class PageUIEvent : public PageLifecycleEvent {
public:
    PageUIEvent(PageLifecycleEventType uiType, Page* page)
        : PageLifecycleEvent(uiType, page) {}
    
    // UI元素标识
    String elementId() const { return getAttribute("elementId").toString(); }
    void setElementId(const String& id) { setAttribute("elementId", id); }
    
    // 变化前的值
    Variant oldValue() const { return getAttribute("oldValue"); }
    void setOldValue(const Variant& value) { setAttribute("oldValue", value); }
    
    // 变化后的值
    Variant newValue() const { return getAttribute("newValue"); }
    void setNewValue(const Variant& value) { setAttribute("newValue", value); }
    
    // 变化类型
    String changeType() const { return getAttribute("changeType").toString(); }
    void setChangeType(const String& type) { setAttribute("changeType", type); }
    
    String className() const override { return "PageUIEvent"; }
};

/**
 * @brief 页面自定义事件
 */
class PageCustomEvent : public PageLifecycleEvent {
public:
    PageCustomEvent(const String& customEventName, Page* page, const Variant& data = Variant())
        : PageLifecycleEvent(PageLifecycleEventType::PageCustomEvent, page), m_customEventName(customEventName) {
        setEventName("page.custom." + customEventName);
        setPayload(data);
    }
    
    String customEventName() const { return m_customEventName; }
    
    String className() const override { return "PageCustomEvent"; }

private:
    String m_customEventName;
};

/**
 * @brief 页面事件工厂
 */
class PageEventFactory {
public:
    // 创建页面生命周期事件
    static std::unique_ptr<PageLifecycleEvent> createLifecycleEvent(
        PageLifecycleEventType type, Page* page = nullptr) {
        return std::make_unique<PageLifecycleEvent>(type, page);
    }
    
    // 创建页面导航事件
    static std::unique_ptr<PageNavigationEvent> createNavigationEvent(
        PageLifecycleEventType navType, Page* fromPage, Page* toPage) {
        return std::make_unique<PageNavigationEvent>(navType, fromPage, toPage);
    }
    
    // 创建页面数据事件
    static std::unique_ptr<PageDataEvent> createDataEvent(
        PageLifecycleEventType dataType, Page* page, const Variant& data = Variant()) {
        return std::make_unique<PageDataEvent>(dataType, page, data);
    }
    
    // 创建页面UI事件
    static std::unique_ptr<PageUIEvent> createUIEvent(
        PageLifecycleEventType uiType, Page* page) {
        return std::make_unique<PageUIEvent>(uiType, page);
    }
    
    // 创建页面自定义事件
    static std::unique_ptr<PageCustomEvent> createCustomEvent(
        const String& customEventName, Page* page, const Variant& data = Variant()) {
        return std::make_unique<PageCustomEvent>(customEventName, page, data);
    }
};

// 预定义页面事件类型

// 页面创建事件
QGL_DEFINE_EVENT(PageCreatedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageCreated), "page.created");
QGL_DEFINE_EVENT(PageInitializedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageInitialized), "page.initialized");
QGL_DEFINE_EVENT(PageCompletedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageCompleted), "page.completed");

// 页面激活事件
QGL_DEFINE_EVENT(PageActivatedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageActivated), "page.activated");
QGL_DEFINE_EVENT(PageDeactivatedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageDeactivated), "page.deactivated");

// 页面销毁事件
QGL_DEFINE_EVENT(PageDestroyingEvent, static_cast<uint32_t>(PageLifecycleEventType::PageDestroying), "page.destroying");
QGL_DEFINE_EVENT(PageDestroyedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageDestroyed), "page.destroyed");

// 页面数据事件
QGL_DEFINE_EVENT(PageDataLoadedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageDataLoaded), "page.dataLoaded");
QGL_DEFINE_EVENT(PageDataChangedEvent, static_cast<uint32_t>(PageLifecycleEventType::PageDataChanged), "page.dataChanged");

// 便捷宏定义

/**
 * @brief 发布页面生命周期事件
 */
#define QGL_PUBLISH_PAGE_EVENT(page, eventType) \
    do { \
        if (auto eventBus = QGL::EventBus::instance()) { \
            auto event = QGL::PageEventFactory::createLifecycleEvent(eventType, page); \
            if (page) { \
                event->setPageId(page->pageId()); \
                event->setPageTitle(page->title()); \
            } \
            eventBus->publish(std::move(event)); \
        } \
    } while(0)

/**
 * @brief 发布页面自定义事件
 */
#define QGL_PUBLISH_PAGE_CUSTOM_EVENT(page, eventName, data) \
    do { \
        if (auto eventBus = QGL::EventBus::instance()) { \
            auto event = QGL::PageEventFactory::createCustomEvent(eventName, page, data); \
            eventBus->publish(std::move(event)); \
        } \
    } while(0)

/**
 * @brief 订阅页面生命周期事件
 */
#define QGL_SUBSCRIBE_PAGE_EVENT(eventType, handler) \
    QGL::EventBus::instance()->subscribe(static_cast<QGL::EventType>(eventType), handler)

} // namespace QGL

#endif // QGL_EVENTS_PAGE_EVENTS_H