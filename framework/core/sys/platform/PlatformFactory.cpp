#include "PlatformFactory.h"

#include "core/sys/Application.h"
#include "core/log/log.h"

#include <utility>

#ifndef ESP_PLATFORM
#include <filesystem>
#endif

#if defined(LVGLPP_PLATFORM_SDL)
#include "SdlPlatform.h"
#elif defined(LVGLPP_PLATFORM_FRAMEBUF)
#include "FramebufPlatform.h"
#elif defined(LVGLPP_PLATFORM_DRM)
#include "DrmPlatform.h"
#endif

namespace lvglpp::sys {

const char *platformName(PlatformKind kind) noexcept
{
    switch (kind)
    {
    case PlatformKind::Sdl:
        return "SDL";
    case PlatformKind::Framebuf:
        return "FRAMEBUF";
    case PlatformKind::Drm:
        return "DRM";
    case PlatformKind::Esp32:
        return "ESP32";
    case PlatformKind::Auto:
        break;
    }
    return "AUTO";
}

std::string executableStem(int argc, char **argv)
{
#if defined(ESP_PLATFORM)
    (void)argc;
    (void)argv;
    return "app";
#else
    if (argc > 0 && argv != nullptr && argv[0] != nullptr)
    {
        return std::filesystem::path(argv[0]).stem().string();
    }
    return {};
#endif
}

std::unique_ptr<Platform> createPlatform(int argc, char **argv, PlatformKind kind)
{
#if defined(LVGLPP_PLATFORM_SDL)
    constexpr PlatformKind built = PlatformKind::Sdl;
#elif defined(LVGLPP_PLATFORM_FRAMEBUF)
    constexpr PlatformKind built = PlatformKind::Framebuf;
#elif defined(LVGLPP_PLATFORM_DRM)
    constexpr PlatformKind built = PlatformKind::Drm;
#elif defined(LVGLPP_PLATFORM_ESP32)
    constexpr PlatformKind built = PlatformKind::Esp32;
#else
    constexpr PlatformKind built = PlatformKind::Auto;
#endif

    if (built == PlatformKind::Auto)
    {
        LOG_ERROR() << "no platform backend was compiled in";
        return nullptr;
    }
    if (kind != PlatformKind::Auto && kind != built)
    {
        LOG_ERROR() << "requested platform " << platformName(kind) << " but this build provides " << platformName(built);
        return nullptr;
    }

#if defined(LVGLPP_PLATFORM_SDL)
    return std::make_unique<SdlPlatform>(argc, argv);
#elif defined(LVGLPP_PLATFORM_FRAMEBUF)
    return std::make_unique<FramebufPlatform>(argc, argv);
#elif defined(LVGLPP_PLATFORM_DRM)
    return std::make_unique<DrmPlatform>(argc, argv);
#elif defined(LVGLPP_PLATFORM_ESP32)
    // ESP-IDF has no generic platform: construct EspPlatform with the
    // application's display/touch adapters and pass it to
    // Application(std::unique_ptr<Platform>).
    (void)argc;
    (void)argv;
    LOG_ERROR() << "ESP32 target: construct EspPlatform explicitly instead of createApplication()";
    return nullptr;
#else
    (void)argc;
    (void)argv;
    return nullptr;
#endif
}

std::unique_ptr<Application> createApplication(int argc, char **argv)
{
    auto platform = createPlatform(argc, argv);
    return std::make_unique<Application>(std::move(platform), executableStem(argc, argv));
}

} // namespace lvglpp::sys
