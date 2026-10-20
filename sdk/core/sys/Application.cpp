#include "Application.h"

#include "BaseActivity.h"
#include "FileSystemManager.h"
#include "InputManager.h"
#include "platform/Platform.h"
#include "platform/PlatformFactory.h"
#include "StackView.h"
#include "core/log/log.h"
#include "core/tools/AssetManager.h"
#include "core/tools/ImageManager.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <system_error>
#include <utility>

#if defined(__unix__) || defined(__APPLE__)
#include <signal.h>
#endif

#if LV_USE_THORVG
#include "thorvg.h"
#endif

namespace lvglpp::sys {

namespace {

#if defined(__unix__) || defined(__APPLE__)
volatile sig_atomic_t g_pending_signal = 0;

void terminateSignalHandler(int signo)
{
    // Async-signal-safe: only stores the signal number.
    g_pending_signal = signo;
}
#endif

} // namespace

struct Application::SignalState {
#if defined(__unix__) || defined(__APPLE__)
    struct sigaction old_int{};
    struct sigaction old_term{};
#endif
};

std::string               Application::application_dir_path_;
std::string               Application::application_name_;
Application::LanguageType Application::language_type_ = Application::Chinese;

Application::Application(int argc, char **argv) : Application(createPlatform(argc, argv), executableStem(argc, argv))
{}

Application::Application(std::unique_ptr<Platform> platform, std::string executableName) : platform_(std::move(platform)), executable_name_(std::move(executableName))
{
    if (application_name_.empty())
    {
        application_name_ = executable_name_;
    }
    StackView::getInstance().setApplication(this);
}

Application::~Application()
{
    shutdown();
}

bool Application::initialize()
{
    if (initialized_)
    {
        return true;
    }
    if (initialize_started_)
    {
        // A previous initialize() failed; re-initialization is not supported.
        return false;
    }
    if (!platform_)
    {
        LOG_ERROR() << "no platform backend available";
        return false;
    }

    initialize_started_ = true;

    std::error_code ec;
    application_dir_path_ = std::filesystem::current_path(ec).string();
    if (ec)
    {
        LOG_WARN() << "cannot resolve the current working directory: " << ec.message();
    }
    if (application_name_.empty())
    {
        application_name_ = executable_name_;
    }

    lv_init();
    lv_initialized_ = true;

    auto &assets = tools::AssetManager::getInstance();
    if (assets.mountCount() == 0)
    {
        assets.mountDefaults(application_dir_path_, application_name_);
    }
    FileSystemManager::getInstance().registerAssetDriver();

    lv_log_register_print_cb([](lv_log_level_t, const char *buffer) { LogWarn << "lvgl logs:" << buffer; });

    installSignalHandlers();

#if LV_USE_THORVG
    tvg::Initializer::init(tvg::CanvasEngine::Sw, 0);
    thorvg_initialized_ = true;
#endif

    platform_->setQuitHandler([this]() { requestQuit(0); });

    if (!platform_->init())
    {
        LOG_ERROR() << "platform initialization failed";
        shutdown();
        return false;
    }
    if (platform_->display() == nullptr)
    {
        LOG_ERROR() << "platform did not create a display";
        shutdown();
        return false;
    }

    if (!platform_->providesTick())
    {
        startTickThread();
    }

    // Requires the default display created by the platform.
    InputManager::getInstance().install();

    if (!onInit())
    {
        LOG_ERROR() << "application onInit() failed";
        shutdown();
        return false;
    }

    initialized_ = true;
    return true;
}

int Application::exec()
{
    if (!initialized_ && !initialize())
    {
        return EXIT_FAILURE;
    }

    if (enable_stack_view_)
    {
        initStackView();
    }

    while (!quit_requested_.load(std::memory_order_acquire))
    {
        pollSignal();
        if (quit_requested_.load(std::memory_order_acquire))
        {
            break;
        }

        handleEvents();

        const std::uint32_t idle = lv_timer_handler();
        if (quit_requested_.load(std::memory_order_acquire))
        {
            break;
        }

        std::uint32_t delay = (idle == LV_NO_TIMER_READY) ? 5u : std::min<std::uint32_t>(idle, 5u);
        if (delay == 0)
        {
            delay = 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    LOG_DEBUG() << "main loop exiting";
    const int code = exit_code_.load(std::memory_order_acquire);
    shutdown();
    LOG_DEBUG() << "shutdown complete, exit code " << code;
    return code;
}

void Application::shutdown()
{
    if (shutdown_done_)
    {
        return;
    }
    shutdown_done_ = true;

    if (!initialize_started_)
    {
        StackView::getInstance().setApplication(nullptr);
        return;
    }

    LOG_DEBUG() << "shutdown: onShutdown";
    onShutdown();

    stopTickThread();
    LOG_DEBUG() << "shutdown: tick stopped";

    if (enable_stack_view_)
    {
        StackView::getInstance().shutdown();
    }

    LOG_DEBUG() << "shutdown: image manager";
    tools::ImageManager::shutdown();
    LOG_DEBUG() << "shutdown: input manager";
    InputManager::getInstance().shutdown();

    if (platform_)
    {
        platform_->shutdown();
    }

    LOG_DEBUG() << "shutdown: lv_deinit";
    if (lv_initialized_)
    {
        lv_deinit();
        lv_initialized_ = false;
    }

#if LV_USE_THORVG
    if (thorvg_initialized_)
    {
        tvg::Initializer::term(tvg::CanvasEngine::Sw);
        thorvg_initialized_ = false;
    }
#endif

    tools::AssetManager::getInstance().reset();
    StackView::getInstance().setApplication(nullptr);
    platform_.reset();

    restoreSignalHandlers();

    initialized_ = false;
}

void Application::requestQuit(int code)
{
    bool expected = false;
    if (quit_requested_.compare_exchange_strong(expected, true, std::memory_order_acq_rel))
    {
        exit_code_.store(code, std::memory_order_release);
    }
}

bool Application::quitRequested() const noexcept
{
    return quit_requested_.load(std::memory_order_acquire);
}

void Application::initStackView()
{
    StackView::getInstance().start();
    if (StackView::getInstance().depth() == 0)
    {
        StackView::getInstance().pushView(std::make_shared<BaseActivity>());
    }
}

void Application::handleEvents()
{
    if (enable_stack_view_)
    {
        StackView::getInstance().clearDeleteVec();
    }

    std::queue<Event> pending;
    {
        std::lock_guard<std::mutex> lock(event_mutex_);
        pending.swap(event_queue_);
    }

    if (!enable_stack_view_)
    {
        return;
    }

    while (!pending.empty())
    {
        StackView::getInstance().notifyAllUi(pending.front());
        pending.pop();
    }
}

void Application::postEvent(const Event &evt)
{
    std::lock_guard<std::mutex> lock(event_mutex_);
    event_queue_.push(evt);
}

void Application::setEnableStackView(bool enable)
{
    enable_stack_view_ = enable;
}

const char *Application::platformName() const noexcept
{
    return platform_ ? platform_->name() : "none";
}

lv_display_t *Application::display() const
{
    return platform_ ? platform_->display() : nullptr;
}

void Application::setTheme(lv_theme_t *theme)
{
    if (platform_)
    {
        platform_->setTheme(theme);
    }
}

void Application::simulatePointerClick(std::int32_t x, std::int32_t y)
{
    if (platform_)
    {
        platform_->simulatePointerClick(x, y);
    }
}

void Application::setLanguageType(LanguageType type)
{
    language_type_   = type;
    const char *name = "Chinese";
    switch (type)
    {
    case Chinese:
        name = "Chinese";
        break;
    case English:
        name = "English";
        break;
    case Japanese:
        name = "Japanese";
        break;
    case Russian:
        name = "Russian";
        break;
    case LOCAL:
        name = "LOCAL";
        break;
    }
    LogDebug << "Set language type: " << name;
}

void Application::startTickThread()
{
    if (tick_thread_.joinable())
    {
        return;
    }

    tick_running_.store(true, std::memory_order_release);
    tick_thread_ = std::thread([this]() {
        while (tick_running_.load(std::memory_order_acquire))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (!tick_running_.load(std::memory_order_acquire))
            {
                break;
            }
            lv_tick_inc(5);
        }
    });
}

void Application::stopTickThread()
{
    tick_running_.store(false, std::memory_order_release);
    if (tick_thread_.joinable())
    {
        tick_thread_.join();
    }
}

void Application::installSignalHandlers()
{
#if defined(__unix__) || defined(__APPLE__)
    if (signals_installed_)
    {
        return;
    }

    signal_state_    = std::make_unique<SignalState>();
    g_pending_signal = 0;

    struct sigaction action{};
    action.sa_handler = &terminateSignalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0; // interrupt sleeps so the loop reacts promptly

    const int int_result  = sigaction(SIGINT, &action, &signal_state_->old_int);
    const int term_result = sigaction(SIGTERM, &action, &signal_state_->old_term);
    if (int_result != 0 || term_result != 0)
    {
        LOG_WARN() << "failed to install SIGINT/SIGTERM handlers";
        signal_state_.reset();
        return;
    }
    signals_installed_ = true;
#endif
}

void Application::restoreSignalHandlers()
{
#if defined(__unix__) || defined(__APPLE__)
    if (!signals_installed_ || !signal_state_)
    {
        return;
    }
    sigaction(SIGINT, &signal_state_->old_int, nullptr);
    sigaction(SIGTERM, &signal_state_->old_term, nullptr);
    signals_installed_ = false;
    signal_state_.reset();
#endif
}

void Application::pollSignal()
{
#if defined(__unix__) || defined(__APPLE__)
    const int signo = static_cast<int>(g_pending_signal);
    if (signo != 0)
    {
        g_pending_signal = 0;
        LOG_DEBUG() << "received termination signal " << signo;
        requestQuit(128 + signo);
    }
#endif
}

} // namespace lvglpp::sys
