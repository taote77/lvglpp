#pragma once

#include "../widgets/Widget.h"
#include "../core/Types.h"
#include "../signals/Signal.h"
#include "../properties/Property.h"
#include <vector>
#include <stack>
#include <memory>
#include <functional>

namespace QGL {

// 前向声明
class Page;
class PageTransition;

/**
 * @brief 页面转场类型
 */
enum class TransitionType {
    None,           // 无转场
    Fade,           // 淡入淡出
    Slide,          // 滑动
    Push,           // 推入
    Cover,          // 覆盖
    Reveal,         // 揭示
    Flip,           // 翻转
    Cube,           // 立方体
    Custom          // 自定义
};

/**
 * @brief 转场方向
 */
enum class TransitionDirection {
    Left,
    Right,
    Up,
    Down,
    None
};

/**
 * @brief 页面状态
 */
enum class PageState {
    Inactive,       // 未激活
    Activating,     // 正在激活
    Active,         // 激活状态
    Deactivating    // 正在去激活
};

/**
 * @brief 页面基类
 * 
 * Page是所有页面的基类，提供页面生命周期管理、
 * 参数传递、状态保存等功能。
 */
class Page : public Widget {
    QGL_OBJECT(Page)

public:
    explicit Page(Widget* parent = nullptr);
    ~Page() override;

    // 页面标识
    void setPageId(const String& id);
    String pageId() const;
    
    void setTitle(const String& title);
    String title() const;
    
    // 页面状态
    PageState state() const;
    
    // 页面参数
    void setParameter(const String& key, const Variant& value);
    Variant parameter(const String& key) const;
    void clearParameters();
    
    // 页面结果
    void setResult(const Variant& result);
    Variant result() const;
    
    // 页面访问控制
    void setRequireAuthentication(bool require);
    bool requireAuthentication() const;
    
    void setPermissions(const StringList& permissions);
    StringList permissions() const;
    
    // 页面缓存策略
    void setCacheEnabled(bool enabled);
    bool isCacheEnabled() const;
    
    void setMaxCacheTime(int seconds);
    int maxCacheTime() const;

    // 信号
    SIGNAL(aboutToActivate);                    // 即将激活
    SIGNAL(activated);                          // 已激活
    SIGNAL(aboutToDeactivate);                  // 即将去激活
    SIGNAL(deactivated);                        // 已去激活
    SIGNAL(parametersChanged);                  // 参数改变
    SIGNAL(resultChanged, const Variant&);     // 结果改变

protected:
    // 生命周期钩子（子类可重写）
    virtual void onActivate() {}               // 页面激活时调用
    virtual void onDeactivate() {}             // 页面去激活时调用
    virtual void onParametersChanged() {}      // 参数改变时调用
    virtual void onResultRequested() {}        // 请求结果时调用
    
    // 页面验证
    virtual bool canActivate() const { return true; }
    virtual bool canDeactivate() const { return true; }
    
    // 状态管理
    void setState(PageState state);

private:
    class PagePrivate;
    std::unique_ptr<PagePrivate> d_ptr;
    
    friend class PageManager;
};

/**
 * @brief 页面转场效果基类
 */
class PageTransition : public Object {
    QGL_OBJECT(PageTransition)

public:
    explicit PageTransition(Object* parent = nullptr);
    virtual ~PageTransition() = default;

    // 转场配置
    void setDuration(int duration);
    int duration() const;
    
    void setDirection(TransitionDirection direction);
    TransitionDirection direction() const;
    
    void setEasing(const String& easing);
    String easing() const;

    // 转场执行
    virtual void start(Page* fromPage, Page* toPage) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;

    // 信号
    SIGNAL(started);
    SIGNAL(finished);
    SIGNAL(progressChanged, double);

protected:
    virtual void updateProgress(double progress);

private:
    class TransitionPrivate;
    std::unique_ptr<TransitionPrivate> d_ptr;
};

/**
 * @brief 页面管理器
 * 
 * PageManager负责管理应用程序中的所有页面，
 * 提供页面导航、历史记录管理、转场效果等功能。
 */
class PageManager : public Object {
    QGL_OBJECT(PageManager)

public:
    explicit PageManager(Object* parent = nullptr);
    ~PageManager() override;

    // 单例访问
    static PageManager* instance();
    static void setInstance(PageManager* manager);

    // 页面注册
    void registerPage(const String& pageId, std::function<Page*()> factory);
    void unregisterPage(const String& pageId);
    bool isPageRegistered(const String& pageId) const;
    
    // 页面创建
    Page* createPage(const String& pageId) const;
    
    // 页面导航
    bool navigateTo(const String& pageId, const std::map<String, Variant>& parameters = {});
    bool goBack();
    bool goForward();
    void goHome();
    
    // 当前页面
    Page* currentPage() const;
    String currentPageId() const;
    
    // 历史记录
    void clearHistory();
    int historySize() const;
    bool canGoBack() const;
    bool canGoForward() const;
    
    StringList getPageHistory() const;
    
    // 页面堆栈
    void pushPage(const String& pageId, const std::map<String, Variant>& parameters = {});
    bool popPage();
    void popToPage(const String& pageId);
    void clearPageStack();
    
    // 转场效果
    void setDefaultTransition(std::shared_ptr<PageTransition> transition);
    std::shared_ptr<PageTransition> defaultTransition() const;
    
    void setTransition(const String& fromPageId, const String& toPageId, 
                      std::shared_ptr<PageTransition> transition);
    
    // 页面预加载
    void preloadPage(const String& pageId);
    void preloadPages(const StringList& pageIds);
    
    // 页面缓存
    void setCacheEnabled(bool enabled);
    bool isCacheEnabled() const;
    
    void setMaxCachedPages(int count);
    int maxCachedPages() const;
    
    void clearPageCache();
    void clearPageCache(const String& pageId);
    
    // 全局参数
    void setGlobalParameter(const String& key, const Variant& value);
    Variant globalParameter(const String& key) const;
    void clearGlobalParameters();
    
    // 页面访问控制
    void setAuthenticationHandler(std::function<bool(const String&, const StringList&)> handler);
    
    // 页面容器
    void setPageContainer(Widget* container);
    Widget* pageContainer() const;

    // 信号
    SIGNAL(pageChanged, const String&, const String&);     // 页面改变 (from, to)
    SIGNAL(navigationRequested, const String&);             // 请求导航
    SIGNAL(navigationFailed, const String&, const String&); // 导航失败 (pageId, reason)
    SIGNAL(historyChanged);                                 // 历史记录改变
    SIGNAL(transitionStarted, const String&, const String&); // 转场开始
    SIGNAL(transitionFinished, const String&, const String&); // 转场完成

protected:
    // 内部导航方法
    bool doNavigate(const String& pageId, const std::map<String, Variant>& parameters, bool addToHistory);
    
    // 页面生命周期管理
    void activatePage(Page* page);
    void deactivatePage(Page* page);
    
    // 转场管理
    void startTransition(Page* fromPage, Page* toPage);
    void finishTransition();
    
    // 权限检查
    bool checkPagePermissions(const String& pageId) const;

private:
    class PageManagerPrivate;
    std::unique_ptr<PageManagerPrivate> d_ptr;
    
    static PageManager* s_instance;
    
    void initializePageManager();
};

/**
 * @brief 淡入淡出转场效果
 */
class FadeTransition : public PageTransition {
    QGL_OBJECT(FadeTransition)

public:
    explicit FadeTransition(Object* parent = nullptr);
    
    void start(Page* fromPage, Page* toPage) override;
    void stop() override;
    bool isRunning() const override;

private:
    class FadeTransitionPrivate;
    std::unique_ptr<FadeTransitionPrivate> d_ptr;
};

/**
 * @brief 滑动转场效果
 */
class SlideTransition : public PageTransition {
    QGL_OBJECT(SlideTransition)

public:
    explicit SlideTransition(Object* parent = nullptr);
    
    void start(Page* fromPage, Page* toPage) override;
    void stop() override;
    bool isRunning() const override;

private:
    class SlideTransitionPrivate;
    std::unique_ptr<SlideTransitionPrivate> d_ptr;
};

/**
 * @brief 页面导航助手宏
 */
#define QGL_NAVIGATE_TO(pageId, ...) \
    QGL::PageManager::instance()->navigateTo(pageId, ##__VA_ARGS__)

#define QGL_GO_BACK() \
    QGL::PageManager::instance()->goBack()

#define QGL_CURRENT_PAGE() \
    QGL::PageManager::instance()->currentPage()

#define QGL_REGISTER_PAGE(pageId, pageClass) \
    QGL::PageManager::instance()->registerPage(pageId, []() -> QGL::Page* { \
        return new pageClass(); \
    })

} // namespace QGL