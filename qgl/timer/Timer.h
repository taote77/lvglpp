#ifndef QGL_TIMER_TIMER_H
#define QGL_TIMER_TIMER_H

#include "../core/Object.h"
#include "../signals/Signal.h"
#include <lvgl.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <functional>
#include <chrono>

namespace QGL {

// 定时器类型
enum class TimerType {
    SingleShot,  // 单次执行
    Repeating    // 重复执行
};

// 定时器类
class Timer : public Object {
    QGL_OBJECT(Timer)
    
public:
    // 构造函数
    Timer(int interval = 1000, TimerType type = TimerType::Repeating, Object* parent = nullptr);
    Timer(std::function<void()> callback, int interval = 1000, TimerType type = TimerType::Repeating, Object* parent = nullptr);
    ~Timer() override;
    
    // 基本控制
    void start();
    void stop();
    void restart();
    bool isActive() const;
    
    // 属性设置
    void setInterval(int milliseconds);
    int getInterval() const { return m_interval; }
    
    void setType(TimerType type);
    TimerType getType() const { return m_type; }
    
    void setRepeatCount(int count); // -1 为无限重复
    int getRepeatCount() const { return m_repeatCount; }
    int getRemainingCount() const { return m_remainingCount; }
    
    // 回调函数
    void setCallback(std::function<void()> callback);
    std::function<void()> getCallback() const { return m_callback; }
    
    // 精度控制
    void setHighPrecision(bool enable) { m_highPrecision = enable; }
    bool isHighPrecision() const { return m_highPrecision; }
    
    // 统计信息
    long long getExecutionCount() const { return m_executionCount; }
    double getAverageExecutionTime() const { return m_averageExecutionTime; }
    double getLastExecutionTime() const { return m_lastExecutionTime; }
    
    // 信号
    SIGNAL(timeout);
    SIGNAL(finished); // 所有重复完成时触发
    SIGNAL(started);
    SIGNAL(stopped);
    
    // 静态便捷方法
    static Timer* singleShot(int delay, std::function<void()> callback);
    static Timer* create(int interval, std::function<void()> callback);


protected:
    
    // 性能统计
    long long m_executionCount = 0;
    
private:
    lv_timer_t* m_lvTimer = nullptr;
    std::function<void()> m_callback;
    TimerType m_type;
    int m_interval;
    int m_repeatCount = -1; // -1 表示无限重复
    int m_remainingCount = -1;
    bool m_highPrecision = false;
    
    double m_averageExecutionTime = 0.0;
    double m_lastExecutionTime = 0.0;
    std::chrono::steady_clock::time_point m_lastStartTime;
    
    // LVGL 回调
    static void lvglTimerCallback(lv_timer_t* timer);
    void handleTimeout();
    
    void updateStatistics(double executionTime);
    void createLvglTimer();
    void destroyLvglTimer();
};

// 定时器管理器
class TimerManager {
public:
    static TimerManager& instance();
    
    // 定时器管理
    Timer* createTimer(int interval, TimerType type = TimerType::Repeating);
    void destroyTimer(Timer* timer);
    void registerTimer(Timer* timer);
    void unregisterTimer(Timer* timer);
    
    // 全局控制
    void pauseAll();
    void resumeAll();
    void stopAll();
    void startAll();
    
    // 统计信息
    int getActiveTimerCount() const;
    int getTotalTimerCount() const;
    std::vector<Timer*> getActiveTimers() const;
    std::vector<Timer*> getAllTimers() const;
    
    // 性能监控
    void enableProfiling(bool enable) { m_profilingEnabled = enable; }
    bool isProfilingEnabled() const { return m_profilingEnabled; }
    
    struct TimerStats {
        int totalTimers = 0;
        int activeTimers = 0;
        long long totalExecutions = 0;
        double averageExecutionTime = 0.0;
        double maxExecutionTime = 0.0;
        double minExecutionTime = 0.0;
    };
    
    TimerStats getStats() const;
    void resetStats();
    
    // 批量操作
    void setGlobalInterval(int milliseconds); // 设置所有定时器的间隔
    void scaleAllIntervals(double factor);    // 缩放所有定时器的间隔
    
    // 优先级控制
    void setTimerPriority(Timer* timer, int priority);
    int getTimerPriority(Timer* timer) const;
    
private:
    TimerManager() = default;
    ~TimerManager() = default;
    
    std::vector<Timer*> m_timers;
    std::map<Timer*, int> m_priorities;
    bool m_profilingEnabled = false;
    bool m_globalPaused = false;
    
    mutable TimerStats m_cachedStats;
    mutable bool m_statsDirty = true;
    
    void markStatsDirty() { m_statsDirty = true; }
    void updateCachedStats() const;
};

// 高精度定时器（基于 std::chrono）
class HighPrecisionTimer : public Object {
    QGL_OBJECT(HighPrecisionTimer)
    
public:
    HighPrecisionTimer(int microseconds = 1000, Object* parent = nullptr);
    ~HighPrecisionTimer() override;
    
    void start();
    void stop();
    bool isActive() const { return m_active; }
    
    void setInterval(int microseconds) { m_interval = std::chrono::microseconds(microseconds); }
    int getInterval() const { return static_cast<int>(m_interval.count()); }
    
    void setCallback(std::function<void()> callback) { m_callback = callback; }
    
    // 信号
    SIGNAL(timeout);
    
private:
    std::chrono::microseconds m_interval;
    std::function<void()> m_callback;
    bool m_active = false;
    
    std::thread m_thread;
    std::atomic<bool> m_shouldStop{false};
    
    void timerLoop();
};

// 动画定时器（专门用于动画）
class AnimationTimer : public Timer {
    QGL_OBJECT(AnimationTimer)
    
public:
    AnimationTimer(int duration, std::function<void(float)> updateCallback, Object* parent = nullptr);
    
    void setDuration(int milliseconds) { m_duration = milliseconds; }
    int getDuration() const { return m_duration; }
    
    void setUpdateCallback(std::function<void(float)> callback) { m_updateCallback = callback; }
    void setFinishedCallback(std::function<void()> callback) { m_finishedCallback = callback; }
    
    float getProgress() const { return m_progress; }
    bool isFinished() const { return m_finished; }
    
    // 缓动函数
    enum class EasingType {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        BounceIn,
        BounceOut
    };
    
    void setEasing(EasingType easing) { m_easing = easing; }
    EasingType getEasing() const { return m_easing; }
    
private:
    int m_duration;
    std::function<void(float)> m_updateCallback;
    std::function<void()> m_finishedCallback;
    std::chrono::steady_clock::time_point m_startTime;
    float m_progress = 0.0f;
    bool m_finished = false;
    EasingType m_easing = EasingType::Linear;
    
    void updateAnimation();
    float applyEasing(float t) const;
};

} // namespace QGL

#endif // QGL_TIMER_TIMER_H