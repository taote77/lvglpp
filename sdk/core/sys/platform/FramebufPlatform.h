#ifndef LVGLPP_FRAMEBUFPLATFORM_H
#define LVGLPP_FRAMEBUFPLATFORM_H

#if defined(LVGLPP_PLATFORM_FRAMEBUF)

#include "core/sys/platform/Platform.h"

#include <cstdint>

#include <lvgl.h>

namespace lvglpp::sys {

class FramebufPlatform final : public Platform
{
public:
    FramebufPlatform(int argc, char **argv);
    ~FramebufPlatform() override;

    const char *name() const noexcept override
    {
        return "FRAMEBUF";
    }

    bool init() override;
    void shutdown() override;

    lv_display_t *display() const override
    {
        return display_;
    }

    void setTheme(lv_theme_t *theme) override;
    void simulatePointerClick(std::int32_t, std::int32_t) override
    {}

    bool providesTick() const noexcept override
    {
        return true;
    }

private:
    lv_display_t *display_ = nullptr;
};

} // namespace lvglpp::sys

#endif // LVGLPP_PLATFORM_FRAMEBUF

#endif // LVGLPP_FRAMEBUFPLATFORM_H
