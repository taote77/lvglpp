#include "color_sheet.h"

#include <atomic>

namespace form::design {

namespace {

std::atomic<bool> g_light_theme{false};

} // namespace

namespace detail {

void setRuntimeTheme(bool light) noexcept
{
    g_light_theme.store(light, std::memory_order_relaxed);
}

std::uint32_t resolveColor(std::uint32_t dark, std::uint32_t light) noexcept
{
    return g_light_theme.load(std::memory_order_relaxed) ? light : dark;
}

} // namespace detail

ColorToken::operator std::uint32_t() const noexcept
{
    return detail::resolveColor(dark, light);
}

} // namespace form::design
