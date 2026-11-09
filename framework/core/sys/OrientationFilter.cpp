#include "OrientationFilter.h"

#include <cmath>
#include <utility>

namespace lvglpp::sys {

OrientationFilter::OrientationFilter(OrientationFilterConfig config) : config_(config)
{}

void OrientationFilter::setConfig(const OrientationFilterConfig &config)
{
    config_ = config;
}

const OrientationFilterConfig &OrientationFilter::config() const
{
    return config_;
}

Orientation OrientationFilter::classify(float gx, float gy, float gz) const
{
    (void)gz;

    if (config_.swap_xy)
    {
        std::swap(gx, gy);
    }
    if (config_.invert_x)
    {
        gx = -gx;
    }
    if (config_.invert_y)
    {
        gy = -gy;
    }

    if (std::fabs(gx) > std::fabs(gy))
    {
        return gx >= 0.0f ? Orientation::Portrait : Orientation::ReversePortrait;
    }
    return gy >= 0.0f ? Orientation::Landscape : Orientation::ReverseLandscape;
}

std::optional<Orientation> OrientationFilter::update(float gx, float gy, float gz)
{
    const auto now = Clock::now();

    const float planar = std::sqrt(gx * gx + gy * gy);

    // Device lying flat (or nearly flat): keep the last stable orientation.
    if (planar < config_.tilt_threshold || std::fabs(gz) > config_.flat_threshold)
    {
        candidate_.reset();
        return std::nullopt;
    }

    const Orientation candidate = classify(gx, gy, gz);

    if (current_ && *current_ == candidate)
    {
        candidate_.reset();
        return std::nullopt;
    }

    if (!candidate_ || *candidate_ != candidate)
    {
        candidate_        = candidate;
        candidate_since_  = now;
        return std::nullopt;
    }

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - candidate_since_).count()
        < static_cast<long long>(config_.stable_ms))
    {
        return std::nullopt;
    }

    if (has_switched_
        && std::chrono::duration_cast<std::chrono::milliseconds>(now - last_switch_).count()
               < static_cast<long long>(config_.cooldown_ms))
    {
        return std::nullopt;
    }

    current_     = candidate;
    last_switch_ = now;
    has_switched_ = true;
    candidate_.reset();
    return current_;
}

void OrientationFilter::setCurrentOrientation(Orientation orientation)
{
    current_      = orientation;
    candidate_.reset();
    has_switched_ = false;
}

void OrientationFilter::reset()
{
    current_.reset();
    candidate_.reset();
    has_switched_ = false;
}

} // namespace lvglpp::sys
