#ifndef LVGLPP_APPLICATION_H
#define LVGLPP_APPLICATION_H

#include "Event.h"

#include <lvgl.h>

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace lvglpp::sys {

class Platform;

/**
 * Application lifecycle and event loop.
 *
 * Platform-specific display/input code lives behind the Platform interface;
 * Application owns LVGL initialization, asset mounting, the event queue,
 * signal handling and ordered shutdown.
 *
 * One Application instance per process. Construct through createApplication()
 * or Application(argc, argv); custom subclasses may use the Platform ctor and
 * override onInit()/onShutdown().
 */
class Application
{
public:
    enum LanguageType { Chinese, English, Japanese, Russian, LOCAL };

    Application(int argc, char *argv[]);
    explicit Application(std::unique_ptr<Platform> platform, std::string executableName = {});

    virtual ~Application();

    Application(const Application &)            = delete;
    Application &operator=(const Application &) = delete;

    /// Idempotent. Initializes LVGL, assets, signals, the platform and onInit().
    bool initialize();

    /// Runs the main loop until requestQuit()/SIGINT/SIGTERM, then shuts down.
    int exec();

    /// Idempotent ordered teardown. Safe to call after a failed initialize().
    void shutdown();

    /// Thread-safe. The first requested exit code wins.
    void requestQuit(int code = 0);

    bool quitRequested() const noexcept;
    bool initialized() const noexcept
    {
        return initialized_;
    }

    const char   *platformName() const noexcept;
    lv_display_t *display() const;
    void          setTheme(lv_theme_t *theme);
    void          simulatePointerClick(std::int32_t x, std::int32_t y);

    /// Thread-safe; delivered on the UI thread in exec().
    void postEvent(const Event &evt);

    void setEnableStackView(bool enable);
    void initStackView();

    static const std::string &applicationDirPath()
    {
        return application_dir_path_;
    }

    static const std::string &applicationName()
    {
        return application_name_;
    }

    static void         setLanguageType(LanguageType type);
    static LanguageType getLanguageType()
    {
        return language_type_;
    }

protected:
    /// Runs after LVGL, assets and the platform are ready.
    virtual bool onInit()
    {
        return true;
    }

    /// Runs first during shutdown, while LVGL is still valid.
    virtual void onShutdown()
    {}

private:
    struct SignalState;

    void handleEvents();
    void startTickThread();
    void stopTickThread();
    void installSignalHandlers();
    void restoreSignalHandlers();
    void pollSignal();

    std::unique_ptr<Platform>    platform_;
    std::string                  executable_name_;
    std::thread                  tick_thread_;
    std::atomic<bool>            tick_running_{false};
    std::atomic<bool>            quit_requested_{false};
    std::atomic<int>             exit_code_{0};
    std::queue<Event>            event_queue_;
    std::mutex                   event_mutex_;
    std::unique_ptr<SignalState> signal_state_;
    bool                         enable_stack_view_  = true;
    bool                         initialized_        = false;
    bool                         initialize_started_ = false;
    bool                         shutdown_done_      = false;
    bool                         lv_initialized_     = false;
    bool                         thorvg_initialized_ = false;
    bool                         signals_installed_  = false;

    static std::string  application_dir_path_;
    static std::string  application_name_;
    static LanguageType language_type_;
};

std::unique_ptr<Application> createApplication(int argc, char **argv);

} // namespace lvglpp::sys

#endif // LVGLPP_APPLICATION_H
