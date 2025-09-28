#include "PageManager.h"
#include "../core/Application.h"
#include "../timer/Timer.h"
#include <lvgl.h>
#include <algorithm>
#include <map>

namespace QGL {

// PageManager静态实例
PageManager* PageManager::s_instance = nullptr;

// Page私有数据类
class Page::PagePrivate {
public:
    String pageId;
    String title;
    PageState state = PageState::Inactive;
    std::map<String, Variant> parameters;
    Variant result;
    bool requireAuth = false;
    StringList permissions;
    bool cacheEnabled = true;
    int maxCacheTime = 3600; // 1小时
    
    PagePrivate() = default;
};

// PageTransition私有数据类
class PageTransition::TransitionPrivate {
public:
    int duration = 300;
    TransitionDirection direction = TransitionDirection::None;
    String easing = "ease-in-out";
    bool running = false;
    Timer* animationTimer = nullptr;
    
    TransitionPrivate() = default;
};

// PageManager私有数据类
class PageManager::PageManagerPrivate {
public:
    std::map<String, std::function<Page*()>> pageFactories;
    std::map<String, Page*> cachedPages;
    std::vector<String> pageHistory;
    std::stack<String> pageStack;
    int historyIndex = -1;
    
    Page* currentPage = nullptr;
    Widget* pageContainer = nullptr;
    
    std::shared_ptr<PageTransition> defaultTransition;
    std::map<std::pair<String, String>, std::shared_ptr<PageTransition>> transitions;
    
    bool cacheEnabled = true;
    int maxCachedPages = 10;
    
    std::map<String, Variant> globalParameters;
    std::function<bool(const String&, const StringList&)> authHandler;
    
    bool transitioning = false;
    String homePage = "home";
    
    PageManagerPrivate() = default;
};

// FadeTransition私有数据类
class FadeTransition::FadeTransitionPrivate {
public:
    Timer* timer = nullptr;
    Page* fromPage = nullptr;
    Page* toPage = nullptr;
    bool running = false;
    int elapsed = 0;
    
    FadeTransitionPrivate() = default;
};

// SlideTransition私有数据类
class SlideTransition::SlideTransitionPrivate {
public:
    Timer* timer = nullptr;
    Page* fromPage = nullptr;
    Page* toPage = nullptr;
    bool running = false;
    int elapsed = 0;
    Point startPos;
    Point endPos;
    
    SlideTransitionPrivate() = default;
};

// Page实现
Page::Page(Widget* parent)
    : Widget(parent)
    , d_ptr(std::make_unique<PagePrivate>()) {
    setObjectName("Page");
}

Page::~Page() = default;

void Page::setPageId(const String& id) {
    d_ptr->pageId = id;
}

String Page::pageId() const {
    return d_ptr->pageId;
}

void Page::setTitle(const String& title) {
    d_ptr->title = title;
}

String Page::title() const {
    return d_ptr->title;
}

PageState Page::state() const {
    return d_ptr->state;
}

void Page::setParameter(const String& key, const Variant& value) {
    d_ptr->parameters[key] = value;
    parametersChanged.emit();
    onParametersChanged();
}

Variant Page::parameter(const String& key) const {
    auto it = d_ptr->parameters.find(key);
    return it != d_ptr->parameters.end() ? it->second : Variant();
}

void Page::clearParameters() {
    d_ptr->parameters.clear();
    parametersChanged.emit();
    onParametersChanged();
}

void Page::setResult(const Variant& result) {
    d_ptr->result = result;
    resultChanged.emit(result);
}

Variant Page::result() const {
    return d_ptr->result;
}

void Page::setRequireAuthentication(bool require) {
    d_ptr->requireAuth = require;
}

bool Page::requireAuthentication() const {
    return d_ptr->requireAuth;
}

void Page::setPermissions(const StringList& permissions) {
    d_ptr->permissions = permissions;
}

StringList Page::permissions() const {
    return d_ptr->permissions;
}

void Page::setCacheEnabled(bool enabled) {
    d_ptr->cacheEnabled = enabled;
}

bool Page::isCacheEnabled() const {
    return d_ptr->cacheEnabled;
}

void Page::setMaxCacheTime(int seconds) {
    d_ptr->maxCacheTime = seconds;
}

int Page::maxCacheTime() const {
    return d_ptr->maxCacheTime;
}

void Page::setState(PageState state) {
    if (d_ptr->state != state) {
        PageState oldState = d_ptr->state;
        d_ptr->state = state;
        
        switch (state) {
            case PageState::Activating:
                aboutToActivate.emit();
                break;
            case PageState::Active:
                if (oldState == PageState::Activating) {
                    activated.emit();
                    onActivate();
                }
                break;
            case PageState::Deactivating:
                aboutToDeactivate.emit();
                break;
            case PageState::Inactive:
                if (oldState == PageState::Deactivating) {
                    deactivated.emit();
                    onDeactivate();
                }
                break;
        }
    }
}

// PageTransition实现
PageTransition::PageTransition(Object* parent)
    : Object(parent)
    , d_ptr(std::make_unique<TransitionPrivate>()) {
    setObjectName("PageTransition");
}

void PageTransition::setDuration(int duration) {
    d_ptr->duration = std::max(0, duration);
}

int PageTransition::duration() const {
    return d_ptr->duration;
}

void PageTransition::setDirection(TransitionDirection direction) {
    d_ptr->direction = direction;
}

TransitionDirection PageTransition::direction() const {
    return d_ptr->direction;
}

void PageTransition::setEasing(const String& easing) {
    d_ptr->easing = easing;
}

String PageTransition::easing() const {
    return d_ptr->easing;
}

void PageTransition::updateProgress(double progress) {
    progressChanged.emit(progress);
}

// PageManager实现
PageManager::PageManager(Object* parent)
    : Object(parent)
    , d_ptr(std::make_unique<PageManagerPrivate>()) {
    initializePageManager();
}

PageManager::~PageManager() {
    // 清理缓存的页面
    for (auto& pair : d_ptr->cachedPages) {
        delete pair.second;
    }
}

void PageManager::initializePageManager() {
    setObjectName("PageManager");
    
    // 设置默认转场效果
    d_ptr->defaultTransition = std::make_shared<FadeTransition>();
}

PageManager* PageManager::instance() {
    return s_instance;
}

void PageManager::setInstance(PageManager* manager) {
    s_instance = manager;
}

void PageManager::registerPage(const String& pageId, std::function<Page*()> factory) {
    d_ptr->pageFactories[pageId] = factory;
}

void PageManager::unregisterPage(const String& pageId) {
    d_ptr->pageFactories.erase(pageId);
    
    // 清理缓存
    auto it = d_ptr->cachedPages.find(pageId);
    if (it != d_ptr->cachedPages.end()) {
        delete it->second;
        d_ptr->cachedPages.erase(it);
    }
}

bool PageManager::isPageRegistered(const String& pageId) const {
    return d_ptr->pageFactories.find(pageId) != d_ptr->pageFactories.end();
}

Page* PageManager::createPage(const String& pageId) const {
    auto it = d_ptr->pageFactories.find(pageId);
    if (it != d_ptr->pageFactories.end()) {
        Page* page = it->second();
        page->setPageId(pageId);
        return page;
    }
    return nullptr;
}

bool PageManager::navigateTo(const String& pageId, const std::map<String, Variant>& parameters) {
    return doNavigate(pageId, parameters, true);
}

bool PageManager::doNavigate(const String& pageId, const std::map<String, Variant>& parameters, bool addToHistory) {
    if (d_ptr->transitioning) {
        return false;
    }
    
    if (!isPageRegistered(pageId)) {
        navigationFailed.emit(pageId, "Page not registered");
        return false;
    }
    
    // 权限检查
    if (!checkPagePermissions(pageId)) {
        navigationFailed.emit(pageId, "Access denied");
        return false;
    }
    
    // 获取或创建目标页面
    Page* targetPage = nullptr;
    auto cachedIt = d_ptr->cachedPages.find(pageId);
    
    if (cachedIt != d_ptr->cachedPages.end() && cachedIt->second->isCacheEnabled()) {
        targetPage = cachedIt->second;
    } else {
        targetPage = createPage(pageId);
        if (!targetPage) {
            navigationFailed.emit(pageId, "Failed to create page");
            return false;
        }
        
        if (d_ptr->cacheEnabled && targetPage->isCacheEnabled()) {
            d_ptr->cachedPages[pageId] = targetPage;
            
            // 检查缓存大小限制
            if (d_ptr->cachedPages.size() > d_ptr->maxCachedPages) {
                // 移除最旧的缓存页面（简化实现）
                auto oldestIt = d_ptr->cachedPages.begin();
                if (oldestIt->second != targetPage && oldestIt->second != d_ptr->currentPage) {
                    delete oldestIt->second;
                    d_ptr->cachedPages.erase(oldestIt);
                }
            }
        }
    }
    
    // 设置页面参数
    for (const auto& param : parameters) {
        targetPage->setParameter(param.first, param.second);
    }
    
    // 设置全局参数
    for (const auto& param : d_ptr->globalParameters) {
        targetPage->setParameter(param.first, param.second);
    }
    
    // 检查是否可以激活目标页面
    if (!targetPage->canActivate()) {
        navigationFailed.emit(pageId, "Page cannot be activated");
        return false;
    }
    
    // 检查当前页面是否可以去激活
    if (d_ptr->currentPage && !d_ptr->currentPage->canDeactivate()) {
        navigationFailed.emit(pageId, "Current page cannot be deactivated");
        return false;
    }
    
    Page* fromPage = d_ptr->currentPage;
    String fromPageId = fromPage ? fromPage->pageId() : "";
    
    // 更新历史记录
    if (addToHistory) {
        // 清除当前索引之后的历史
        if (d_ptr->historyIndex >= 0 && d_ptr->historyIndex < d_ptr->pageHistory.size() - 1) {
            d_ptr->pageHistory.erase(d_ptr->pageHistory.begin() + d_ptr->historyIndex + 1, 
                                   d_ptr->pageHistory.end());
        }
        
        d_ptr->pageHistory.push_back(pageId);
        d_ptr->historyIndex = d_ptr->pageHistory.size() - 1;
        historyChanged.emit();
    }
    
    // 设置页面容器
    if (d_ptr->pageContainer && targetPage->getLVGLObject()) {
        lv_obj_set_parent(targetPage->getLVGLObject(), d_ptr->pageContainer->getLVGLObject());
    }
    
    // 开始转场
    navigationRequested.emit(pageId);
    startTransition(fromPage, targetPage);
    
    return true;
}

bool PageManager::goBack() {
    if (!canGoBack()) {
        return false;
    }
    
    d_ptr->historyIndex--;
    String pageId = d_ptr->pageHistory[d_ptr->historyIndex];
    return doNavigate(pageId, {}, false);
}

bool PageManager::goForward() {
    if (!canGoForward()) {
        return false;
    }
    
    d_ptr->historyIndex++;
    String pageId = d_ptr->pageHistory[d_ptr->historyIndex];
    return doNavigate(pageId, {}, false);
}

void PageManager::goHome() {
    navigateTo(d_ptr->homePage);
}

Page* PageManager::currentPage() const {
    return d_ptr->currentPage;
}

String PageManager::currentPageId() const {
    return d_ptr->currentPage ? d_ptr->currentPage->pageId() : "";
}

void PageManager::clearHistory() {
    d_ptr->pageHistory.clear();
    d_ptr->historyIndex = -1;
    historyChanged.emit();
}

int PageManager::historySize() const {
    return d_ptr->pageHistory.size();
}

bool PageManager::canGoBack() const {
    return d_ptr->historyIndex > 0;
}

bool PageManager::canGoForward() const {
    return d_ptr->historyIndex >= 0 && d_ptr->historyIndex < d_ptr->pageHistory.size() - 1;
}

StringList PageManager::getPageHistory() const {
    return d_ptr->pageHistory;
}

void PageManager::pushPage(const String& pageId, const std::map<String, Variant>& parameters) {
    if (d_ptr->currentPage) {
        d_ptr->pageStack.push(d_ptr->currentPage->pageId());
    }
    navigateTo(pageId, parameters);
}

bool PageManager::popPage() {
    if (d_ptr->pageStack.empty()) {
        return false;
    }
    
    String pageId = d_ptr->pageStack.top();
    d_ptr->pageStack.pop();
    return navigateTo(pageId);
}

void PageManager::popToPage(const String& pageId) {
    // 弹出到指定页面
    while (!d_ptr->pageStack.empty() && d_ptr->pageStack.top() != pageId) {
        d_ptr->pageStack.pop();
    }
    
    if (!d_ptr->pageStack.empty()) {
        popPage();
    } else {
        navigateTo(pageId);
    }
}

void PageManager::clearPageStack() {
    while (!d_ptr->pageStack.empty()) {
        d_ptr->pageStack.pop();
    }
}

void PageManager::setDefaultTransition(std::shared_ptr<PageTransition> transition) {
    d_ptr->defaultTransition = transition;
}

std::shared_ptr<PageTransition> PageManager::defaultTransition() const {
    return d_ptr->defaultTransition;
}

void PageManager::setTransition(const String& fromPageId, const String& toPageId, 
                               std::shared_ptr<PageTransition> transition) {
    d_ptr->transitions[std::make_pair(fromPageId, toPageId)] = transition;
}

void PageManager::preloadPage(const String& pageId) {
    if (!isPageRegistered(pageId)) {
        return;
    }
    
    auto it = d_ptr->cachedPages.find(pageId);
    if (it == d_ptr->cachedPages.end()) {
        Page* page = createPage(pageId);
        if (page && page->isCacheEnabled()) {
            d_ptr->cachedPages[pageId] = page;
        }
    }
}

void PageManager::preloadPages(const StringList& pageIds) {
    for (const auto& pageId : pageIds) {
        preloadPage(pageId);
    }
}

void PageManager::setCacheEnabled(bool enabled) {
    d_ptr->cacheEnabled = enabled;
    
    if (!enabled) {
        clearPageCache();
    }
}

bool PageManager::isCacheEnabled() const {
    return d_ptr->cacheEnabled;
}

void PageManager::setMaxCachedPages(int count) {
    d_ptr->maxCachedPages = std::max(1, count);
}

int PageManager::maxCachedPages() const {
    return d_ptr->maxCachedPages;
}

void PageManager::clearPageCache() {
    for (auto& pair : d_ptr->cachedPages) {
        if (pair.second != d_ptr->currentPage) {
            delete pair.second;
        }
    }
    d_ptr->cachedPages.clear();
    
    // 保持当前页面
    if (d_ptr->currentPage) {
        d_ptr->cachedPages[d_ptr->currentPage->pageId()] = d_ptr->currentPage;
    }
}

void PageManager::clearPageCache(const String& pageId) {
    auto it = d_ptr->cachedPages.find(pageId);
    if (it != d_ptr->cachedPages.end() && it->second != d_ptr->currentPage) {
        delete it->second;
        d_ptr->cachedPages.erase(it);
    }
}

void PageManager::setGlobalParameter(const String& key, const Variant& value) {
    d_ptr->globalParameters[key] = value;
}

Variant PageManager::globalParameter(const String& key) const {
    auto it = d_ptr->globalParameters.find(key);
    return it != d_ptr->globalParameters.end() ? it->second : Variant();
}

void PageManager::clearGlobalParameters() {
    d_ptr->globalParameters.clear();
}

void PageManager::setAuthenticationHandler(std::function<bool(const String&, const StringList&)> handler) {
    d_ptr->authHandler = handler;
}

void PageManager::setPageContainer(Widget* container) {
    d_ptr->pageContainer = container;
}

Widget* PageManager::pageContainer() const {
    return d_ptr->pageContainer;
}

void PageManager::activatePage(Page* page) {
    if (!page) return;
    
    page->setState(PageState::Activating);
    page->setState(PageState::Active);
    d_ptr->currentPage = page;
    
    if (page->getLVGLObject()) {
        lv_obj_clear_flag(page->getLVGLObject(), LV_OBJ_FLAG_HIDDEN);
    }
}

void PageManager::deactivatePage(Page* page) {
    if (!page) return;
    
    page->setState(PageState::Deactivating);
    page->setState(PageState::Inactive);
    
    if (page->getLVGLObject()) {
        lv_obj_add_flag(page->getLVGLObject(), LV_OBJ_FLAG_HIDDEN);
    }
}

void PageManager::startTransition(Page* fromPage, Page* toPage) {
    d_ptr->transitioning = true;
    
    // 选择转场效果
    std::shared_ptr<PageTransition> transition = d_ptr->defaultTransition;
    
    if (fromPage && toPage) {
        auto key = std::make_pair(fromPage->pageId(), toPage->pageId());
        auto it = d_ptr->transitions.find(key);
        if (it != d_ptr->transitions.end()) {
            transition = it->second;
        }
    }
    
    transitionStarted.emit(fromPage ? fromPage->pageId() : "", 
                         toPage ? toPage->pageId() : "");
    
    if (transition && transition->duration() > 0) {
        // 连接转场完成信号
        transition->finished.connect([this, fromPage, toPage](){ 
            finishTransition();
            if (fromPage) deactivatePage(fromPage);
            if (toPage) activatePage(toPage);
        });
        
        transition->start(fromPage, toPage);
    } else {
        // 无转场效果，直接切换
        if (fromPage) deactivatePage(fromPage);
        if (toPage) activatePage(toPage);
        finishTransition();
    }
}

void PageManager::finishTransition() {
    d_ptr->transitioning = false;
    
    String fromPageId = "";
    String toPageId = "";
    
    if (d_ptr->currentPage) {
        toPageId = d_ptr->currentPage->pageId();
    }
    
    transitionFinished.emit(fromPageId, toPageId);
    pageChanged.emit(fromPageId, toPageId);
}

bool PageManager::checkPagePermissions(const String& pageId) const {
    auto it = d_ptr->pageFactories.find(pageId);
    if (it == d_ptr->pageFactories.end()) {
        return false;
    }
    
    // 创建临时页面来检查权限
    Page* tempPage = it->second();
    bool hasPermission = true;
    
    if (tempPage->requireAuthentication() && d_ptr->authHandler) {
        hasPermission = d_ptr->authHandler(pageId, tempPage->permissions());
    }
    
    delete tempPage;
    return hasPermission;
}

// FadeTransition实现
FadeTransition::FadeTransition(Object* parent)
    : PageTransition(parent)
    , d_ptr(std::make_unique<FadeTransitionPrivate>()) {
    setObjectName("FadeTransition");
}

void FadeTransition::start(Page* fromPage, Page* toPage) {
    if (d_ptr->running) {
        stop();
    }
    
    d_ptr->fromPage = fromPage;
    d_ptr->toPage = toPage;
    d_ptr->running = true;
    d_ptr->elapsed = 0;
    
    started.emit();
    
    // 创建定时器进行动画
    d_ptr->timer = new Timer(16, TimerType::Repeating); // 60 FPS
    d_ptr->timer->timeout.connect([this]() {
        d_ptr->elapsed += 16;
        double progress = static_cast<double>(d_ptr->elapsed) / duration();
        
        if (progress >= 1.0) {
            progress = 1.0;
            stop();
            finished.emit();
        }
        
        updateProgress(progress);
        
        // 应用淡入淡出效果
        if (d_ptr->fromPage && d_ptr->fromPage->getLVGLObject()) {
            lv_obj_set_style_opa(d_ptr->fromPage->getLVGLObject(), 
                                static_cast<lv_opa_t>((1.0 - progress) * 255), 0);
        }
        
        if (d_ptr->toPage && d_ptr->toPage->getLVGLObject()) {
            lv_obj_set_style_opa(d_ptr->toPage->getLVGLObject(), 
                                static_cast<lv_opa_t>(progress * 255), 0);
        }
    });
    
    d_ptr->timer->start();
}

void FadeTransition::stop() {
    if (d_ptr->timer) {
        d_ptr->timer->stop();
        delete d_ptr->timer;
        d_ptr->timer = nullptr;
    }
    
    d_ptr->running = false;
    
    // 恢复不透明度
    if (d_ptr->fromPage && d_ptr->fromPage->getLVGLObject()) {
        lv_obj_set_style_opa(d_ptr->fromPage->getLVGLObject(), LV_OPA_COVER, 0);
    }
    
    if (d_ptr->toPage && d_ptr->toPage->getLVGLObject()) {
        lv_obj_set_style_opa(d_ptr->toPage->getLVGLObject(), LV_OPA_COVER, 0);
    }
}

bool FadeTransition::isRunning() const {
    return d_ptr->running;
}

// SlideTransition实现
SlideTransition::SlideTransition(Object* parent)
    : PageTransition(parent)
    , d_ptr(std::make_unique<SlideTransitionPrivate>()) {
    setObjectName("SlideTransition");
}

void SlideTransition::start(Page* fromPage, Page* toPage) {
    if (d_ptr->running) {
        stop();
    }
    
    d_ptr->fromPage = fromPage;
    d_ptr->toPage = toPage;
    d_ptr->running = true;
    d_ptr->elapsed = 0;
    
    // 计算滑动位置
    // 这里需要根据页面容器的大小来计算
    // 简化实现，假设页面大小为400x300
    int pageWidth = 400;
    int pageHeight = 300;
    
    switch (direction()) {
        case TransitionDirection::Left:
            d_ptr->startPos = Point(0, 0);
            d_ptr->endPos = Point(-pageWidth, 0);
            break;
        case TransitionDirection::Right:
            d_ptr->startPos = Point(0, 0);
            d_ptr->endPos = Point(pageWidth, 0);
            break;
        case TransitionDirection::Up:
            d_ptr->startPos = Point(0, 0);
            d_ptr->endPos = Point(0, -pageHeight);
            break;
        case TransitionDirection::Down:
            d_ptr->startPos = Point(0, 0);
            d_ptr->endPos = Point(0, pageHeight);
            break;
        default:
            d_ptr->startPos = Point(0, 0);
            d_ptr->endPos = Point(0, 0);
            break;
    }
    
    started.emit();
    
    // 创建定时器进行动画
    d_ptr->timer = new Timer(16, TimerType::Repeating); // 60 FPS
    d_ptr->timer->timeout.connect([this]() {
        d_ptr->elapsed += 16;
        double progress = static_cast<double>(d_ptr->elapsed) / duration();
        
        if (progress >= 1.0) {
            progress = 1.0;
            stop();
            finished.emit();
        }
        
        updateProgress(progress);
        
        // 应用滑动效果
        Point currentPos = Point(
            d_ptr->startPos.x + (d_ptr->endPos.x - d_ptr->startPos.x) * progress,
            d_ptr->startPos.y + (d_ptr->endPos.y - d_ptr->startPos.y) * progress
        );
        
        if (d_ptr->fromPage && d_ptr->fromPage->getLVGLObject()) {
            lv_obj_set_pos(d_ptr->fromPage->getLVGLObject(), currentPos.x, currentPos.y);
        }
    });
    
    d_ptr->timer->start();
}

void SlideTransition::stop() {
    if (d_ptr->timer) {
        d_ptr->timer->stop();
        delete d_ptr->timer;
        d_ptr->timer = nullptr;
    }
    
    d_ptr->running = false;
    
    // 恢复位置
    if (d_ptr->fromPage && d_ptr->fromPage->getLVGLObject()) {
        lv_obj_set_pos(d_ptr->fromPage->getLVGLObject(), 0, 0);
    }
    
    if (d_ptr->toPage && d_ptr->toPage->getLVGLObject()) {
        lv_obj_set_pos(d_ptr->toPage->getLVGLObject(), 0, 0);
    }
}

bool SlideTransition::isRunning() const {
    return d_ptr->running;
}

} // namespace QGL