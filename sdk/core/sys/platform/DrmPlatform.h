#ifndef LVGLPP_DRMPLATFORM_H
#define LVGLPP_DRMPLATFORM_H

#if defined(LVGLPP_PLATFORM_DRM)

#include "core/sys/platform/Platform.h"

#include <cstdint>

#include <lvgl.h>

namespace lvglpp::sys {

class DrmPlatform final : public Platform
{
public:
    DrmPlatform(int argc, char **argv);
    ~DrmPlatform() override;

    const char *name() const noexcept override
    {
        return "DRM";
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

#endif // LVGLPP_PLATFORM_DRM

#endif // LVGLPP_DRMPLATFORM_H
