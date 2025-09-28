#include "Timer.h"
#include <algorithm>
#include <cmath>
#include <thread>

namespace QGL {

// Timer 实现
Timer::Timer(int interval, TimerType type, Object* parent)
    : Object(parent), m_type(type), m_interval(interval) {
    TimerManager::instance().registerTimer(this);
}

Timer::Timer(std::function<void()> callback, int interval, TimerType type, Object* parent)
    : Object(parent), m_callback(callback), m_type(type), m_interval(interval) {
    TimerManager::instance().registerTimer(this);
}

Timer::~Timer() {
    stop();
    TimerManager::instance().unregisterTimer(this);
}

void Timer::start() {
    if (m_lvTimer) return; // 已经启动
    
    m_remainingCount = m_repeatCount;
    m_executionCount = 0;
    createLvglTimer();
    started.emit();
}

void Timer::stop() {
    if (!m_lvTimer) return; // 已经停止
    
    destroyLvglTimer();
    stopped.emit();
}

void Timer::restart() {
    stop();
    start();
}

bool Timer::isActive() const {
    return m_lvTimer != nullptr;
}

void Timer::setInterval(int milliseconds) {
    bool wasActive = isActive();
    if (wasActive) {
        stop();
    }
    
    m_interval = milliseconds;
    
    if (wasActive) {
        start();
    }
}

void Timer::setType(TimerType type) {
    m_type = type;
    if (m_type == TimerType::SingleShot) {
        m_repeatCount = 1;
    }
}

void Timer::setRepeatCount(int count) {
    m_repeatCount = count;
    if (count == 1) {
        m_type = TimerType::SingleShot;
    } else {
        m_type = TimerType::Repeating;
    }
}

void Timer::setCallback(std::function<void()> callback) {
    m_callback = callback;
}

void Timer::createLvglTimer() {
    if (m_lvTimer) return;
    
    m_lvTimer = lv_timer_create(lvglTimerCallback, m_interval, this);
    
    if (m_type == TimerType::SingleShot) {
        lv_timer_set_repeat_count(m_lvTimer, 1);
    } else if (m_repeatCount > 0) {
        lv_timer_set_repeat_count(m_lvTimer, m_repeatCount);
    }
}

void Timer::destroyLvglTimer() {
    if (m_lvTimer) {
        lv_timer_del(m_lvTimer);
        m_lvTimer = nullptr;
    }
}

void Timer::lvglTimerCallback(lv_timer_t* timer) {
    Timer* timerObj = static_cast<Timer*>(lv_timer_get_user_data(timer));
    if (timerObj) {
        timerObj->handleTimeout();
    }
}

void Timer::handleTimeout() {
    auto startTime = std::chrono::steady_clock::now();
    
    // 发射信号
    timeout.emit();
    
    // 执行回调
    if (m_callback) {
        m_callback();
    }
    
    // 更新统计
    m_executionCount++;
    if (m_remainingCount > 0) {
        m_remainingCount--;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    double executionTime = duration.count() / 1000.0; // 转换为毫秒
    
    updateStatistics(executionTime);
    
    // 检查是否完成
    if (m_type == TimerType::SingleShot || 
        (m_repeatCount > 0 && m_remainingCount <= 0)) {
        destroyLvglTimer();
        finished.emit();
    }
}

void Timer::updateStatistics(double executionTime) {
    m_lastExecutionTime = executionTime;
    
    if (m_executionCount == 1) {
        m_averageExecutionTime = executionTime;
    } else {
        // 使用移动平均
        double alpha = 0.1; // 平滑因子
        m_averageExecutionTime = alpha * executionTime + (1.0 - alpha) * m_averageExecutionTime;
    }
}

Timer* Timer::singleShot(int delay, std::function<void()> callback) {
    Timer* timer = new Timer(callback, delay, TimerType::SingleShot);
    timer->start();
    return timer;
}

Timer* Timer::create(int interval, std::function<void()> callback) {
    return new Timer(callback, interval, TimerType::Repeating);
}

// TimerManager 实现
TimerManager& TimerManager::instance() {
    static TimerManager manager;
    return manager;
}

Timer* TimerManager::createTimer(int interval, TimerType type) {
    Timer* timer = new Timer(interval, type);
    // 已经在 Timer 构造函数中注册了
    return timer;
}

void TimerManager::destroyTimer(Timer* timer) {
    if (timer) {
        unregisterTimer(timer);
        delete timer;
    }
}

void TimerManager::registerTimer(Timer* timer) {
    if (timer && std::find(m_timers.begin(), m_timers.end(), timer) == m_timers.end()) {
        m_timers.push_back(timer);
        markStatsDirty();
    }
}

void TimerManager::unregisterTimer(Timer* timer) {
    auto it = std::find(m_timers.begin(), m_timers.end(), timer);
    if (it != m_timers.end()) {
        m_timers.erase(it);
        m_priorities.erase(timer);
        markStatsDirty();
    }
}

void TimerManager::pauseAll() {
    m_globalPaused = true;
    for (Timer* timer : m_timers) {
        if (timer->isActive()) {
            timer->stop();
        }
    }
}

void TimerManager::resumeAll() {
    if (m_globalPaused) {
        m_globalPaused = false;
        // 注意：这里不会自动重启定时器，需要应用程序处理
    }
}

void TimerManager::stopAll() {
    for (Timer* timer : m_timers) {
        timer->stop();
    }
}

void TimerManager::startAll() {
    if (!m_globalPaused) {
        for (Timer* timer : m_timers) {
            if (!timer->isActive()) {
                timer->start();
            }
        }
    }
}

int TimerManager::getActiveTimerCount() const {
    int count = 0;
    for (Timer* timer : m_timers) {
        if (timer->isActive()) {
            count++;
        }
    }
    return count;
}

int TimerManager::getTotalTimerCount() const {
    return static_cast<int>(m_timers.size());
}

std::vector<Timer*> TimerManager::getActiveTimers() const {
    std::vector<Timer*> activeTimers;
    for (Timer* timer : m_timers) {
        if (timer->isActive()) {
            activeTimers.push_back(timer);
        }
    }
    return activeTimers;
}

std::vector<Timer*> TimerManager::getAllTimers() const {
    return m_timers;
}

TimerManager::TimerStats TimerManager::getStats() const {
    if (m_statsDirty) {
        updateCachedStats();
        m_statsDirty = false;
    }
    return m_cachedStats;
}

void TimerManager::resetStats() {
    for (Timer* timer : m_timers) {
        // 重置每个定时器的统计信息（需要在 Timer 类中添加重置方法）
    }
    markStatsDirty();
}

void TimerManager::setGlobalInterval(int milliseconds) {
    for (Timer* timer : m_timers) {
        timer->setInterval(milliseconds);
    }
}

void TimerManager::scaleAllIntervals(double factor) {
    for (Timer* timer : m_timers) {
        int newInterval = static_cast<int>(timer->getInterval() * factor);
        timer->setInterval(std::max(1, newInterval)); // 确保至少 1ms
    }
}

void TimerManager::setTimerPriority(Timer* timer, int priority) {
    if (timer) {
        m_priorities[timer] = priority;
    }
}

int TimerManager::getTimerPriority(Timer* timer) const {
    auto it = m_priorities.find(timer);
    return (it != m_priorities.end()) ? it->second : 0;
}

void TimerManager::updateCachedStats() const {
    m_cachedStats = TimerStats();
    m_cachedStats.totalTimers = static_cast<int>(m_timers.size());
    
    double totalExecutionTime = 0.0;
    long long totalExecutions = 0;
    m_cachedStats.maxExecutionTime = 0.0;
    m_cachedStats.minExecutionTime = std::numeric_limits<double>::max();
    
    for (Timer* timer : m_timers) {
        if (timer->isActive()) {
            m_cachedStats.activeTimers++;
        }
        
        totalExecutions += timer->getExecutionCount();
        double avgTime = timer->getAverageExecutionTime();
        if (avgTime > 0) {
            totalExecutionTime += avgTime * timer->getExecutionCount();
            m_cachedStats.maxExecutionTime = std::max(m_cachedStats.maxExecutionTime, avgTime);
            m_cachedStats.minExecutionTime = std::min(m_cachedStats.minExecutionTime, avgTime);
        }
    }
    
    m_cachedStats.totalExecutions = totalExecutions;
    if (totalExecutions > 0) {
        m_cachedStats.averageExecutionTime = totalExecutionTime / totalExecutions;
    }
    
    if (m_cachedStats.minExecutionTime == std::numeric_limits<double>::max()) {
        m_cachedStats.minExecutionTime = 0.0;
    }
}

// HighPrecisionTimer 实现
HighPrecisionTimer::HighPrecisionTimer(int microseconds, Object* parent)
    : Object(parent), m_interval(microseconds) {}

HighPrecisionTimer::~HighPrecisionTimer() {
    stop();
}

void HighPrecisionTimer::start() {
    if (m_active) return;
    
    m_active = true;
    m_shouldStop = false;
    m_thread = std::thread(&HighPrecisionTimer::timerLoop, this);
}

void HighPrecisionTimer::stop() {
    if (!m_active) return;
    
    m_shouldStop = true;
    if (m_thread.joinable()) {
        m_thread.join();
    }
    m_active = false;
}

void HighPrecisionTimer::timerLoop() {
    auto nextTime = std::chrono::steady_clock::now() + m_interval;
    
    while (!m_shouldStop) {
        std::this_thread::sleep_until(nextTime);
        
        if (!m_shouldStop) {
            timeout.emit();
            if (m_callback) {
                m_callback();
            }
        }
        
        nextTime += m_interval;
    }
}

// AnimationTimer 实现
AnimationTimer::AnimationTimer(int duration, std::function<void(float)> updateCallback, Object* parent)
    : Timer(16, TimerType::Repeating, parent), // 60 FPS
      m_duration(duration), m_updateCallback(updateCallback) {
    
    setCallback([this]() {
        updateAnimation();
    });
}

void AnimationTimer::updateAnimation() {
    if (m_finished) return;
    
    if (m_executionCount == 1) {
        m_startTime = std::chrono::steady_clock::now();
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
    
    if (elapsed >= m_duration) {
        m_progress = 1.0f;
        m_finished = true;
        stop();
        
        if (m_updateCallback) {
            m_updateCallback(applyEasing(m_progress));
        }
        if (m_finishedCallback) {
            m_finishedCallback();
        }
    } else {
        m_progress = static_cast<float>(elapsed) / m_duration;
        if (m_updateCallback) {
            m_updateCallback(applyEasing(m_progress));
        }
    }
}

float AnimationTimer::applyEasing(float t) const {
    switch (m_easing) {
    case EasingType::Linear:
        return t;
    case EasingType::EaseIn:
        return t * t;
    case EasingType::EaseOut:
        return 1.0f - (1.0f - t) * (1.0f - t);
    case EasingType::EaseInOut:
        return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    case EasingType::BounceOut: {
        if (t < 1.0f / 2.75f) {
            return 7.5625f * t * t;
        } else if (t < 2.0f / 2.75f) {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + 0.75f;
        } else if (t < 2.5f / 2.75f) {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + 0.9375f;
        } else {
            t -= 2.625f / 2.75f;
            return 7.5625f * t * t + 0.984375f;
        }
    }
    default:
        return t;
    }
}

} // namespace QGL