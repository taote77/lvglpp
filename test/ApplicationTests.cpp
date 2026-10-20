#include "TestFramework.h"
#include "core/sys/Application.h"
#include "core/sys/platform/Platform.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using namespace lvglpp;
using namespace lvglpp::test;

namespace {

class FakePlatform final : public sys::Platform
{
public:
    const char *name() const noexcept override
    {
        return "FAKE";
    }

    bool init() override
    {
        ++init_calls;
        return true;
    }

    void shutdown() override
    {
        ++shutdown_calls;
    }

    lv_display_t *display() const override
    {
        return nullptr;
    }

    void setTheme(lv_theme_t *) override
    {}

    void simulatePointerClick(std::int32_t x, std::int32_t y) override
    {
        ++click_calls;
        last_x = x;
        last_y = y;
    }

    bool providesTick() const noexcept override
    {
        return true;
    }

    int init_calls     = 0;
    int shutdown_calls = 0;
    int click_calls    = 0;
    int last_x         = 0;
    int last_y         = 0;
};

} // namespace

TEST(Application_requestQuit_is_thread_safe)
{
    auto            *fake = new FakePlatform();
    sys::Application app{std::unique_ptr<sys::Platform>(fake)};

    CHECK_FALSE(app.quitRequested());
    CHECK(std::string(app.platformName()) == "FAKE");

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; ++i)
    {
        threads.emplace_back([&app, i]() { app.requestQuit(i + 1); });
    }
    for (auto &thread : threads)
    {
        thread.join();
    }

    CHECK_TRUE(app.quitRequested());

    // Forwarding helpers must not touch LVGL when no display exists.
    app.simulatePointerClick(12, 34);
    CHECK_EQ(fake->click_calls, 1);
    CHECK_EQ(fake->last_x, 12);
    CHECK_EQ(fake->last_y, 34);
    CHECK(app.display() == nullptr);
    return true;
}

TEST(Application_factory_returns_builtin_platform)
{
    char  program[] = "test-app";
    char *argv[]    = {program, nullptr};

    auto app = sys::createApplication(1, argv);
    CHECK(app != nullptr);

#if defined(LVGLPP_PLATFORM_SDL)
    CHECK(std::string(app->platformName()) == "SDL");
#elif defined(LVGLPP_PLATFORM_FRAMEBUF)
    CHECK(std::string(app->platformName()) == "FRAMEBUF");
#elif defined(LVGLPP_PLATFORM_DRM)
    CHECK(std::string(app->platformName()) == "DRM");
#endif

    // The application is intentionally not initialized here; destroying it
    // must not touch LVGL or SDL.
    return true;
}
