
#include "Application.h"
#include "BaseActivity.h"
#include "FileSystemManager.h"
#include "StackView.h"
#include "core/log/log.h"
#include "core/tools/AssetManager.h"
#include "core/tools/Utils.h"
#include <filesystem>
#include <memory>
#include <optional>
#include <thread>
#include <unistd.h>

namespace lvglpp::sys {

std::string Application::application_dir_path_;
std::string Application::application_name_;

Application::LanguageType Application::language_type_ = Application::Chinese;

Application::Application(int argc, char *argv[])
{
    if (argc > 0 && argv != nullptr && argv[0] != nullptr)
    {
        application_name_ = std::filesystem::path(argv[0]).stem().string();
    }
    StackView::getInstance().setApplication(this);
}

bool Application::initApp()
{
    application_dir_path_ = std::filesystem::current_path().string();
    // application_name_ = "";
    LogDebug << "path:" << application_dir_path_;
    lv_init();

    // Mount the application assets before any widget is created. Sources are
    // common.pak / common.asset first, then <exe>.pak / <exe>.asset; the
    // LVGLPP_ASSET_DIR and LVGLPP_ASSET_PACK environment variables override
    // discovery for development and tests. Guard against repeated initApp().
    auto &assets = tools::AssetManager::getInstance();
    if (assets.mountCount() == 0)
    {
        assets.mountDefaults(application_dir_path_, application_name_);
    }
    FileSystemManager::getInstance().registerAssetDriver();

    // init log printf
    lv_log_register_print_cb([](lv_log_level_t level, const char *buf) -> void { LogWarn << "lvgl logs:" << buf; });
#if LV_TICK_CUSTOM == 0
    std::thread tick_thread([]() -> void {
        while (true)
        {
            std::chrono::milliseconds duration(5);
            std::this_thread::sleep_for(duration);

            lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
        }
    });
    tick_thread.detach();
#endif

    setInitStatus(true);
    return true;
}

void Application::initStackView()
{
    StackView::getInstance().start();
    if (StackView::getInstance().depth() == 0)
    {
        StackView::getInstance().pushView(std::make_shared<BaseActivity>());
    }
}

int Application::exec()
{
    if (!isInit())
    {
        initApp();
    }

    if (enable_stack_view_)
    {
        initStackView();
    }

    while (true)
    {
        handleEvent();
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
        usleep(5 * 1000);
    }
    return 0;
}

void Application::postEvent(const Event &evt)
{
    std::lock_guard<std::mutex> lock(mutex_);

    event_queue_.push(evt);
}

void Application::handleEvent()
{
    // Destroy popped views outside the queue lock: activity destructors may
    // post events themselves.
    if (enable_stack_view_)
    {
        StackView::getInstance().clearDeleteVec();
    }

    std::optional<Event> event;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!event_queue_.empty())
        {
            event = event_queue_.front();
            event_queue_.pop();
        }
    }

    // Deliver outside the lock so handlers can call postEvent() again.
    if (event && enable_stack_view_)
    {
        StackView::getInstance().notifyAllUi(*event);
    }
}
} // namespace lvglpp::sys
