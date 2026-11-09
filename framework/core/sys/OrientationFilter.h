#ifndef LVGLPP_ORIENTATION_FILTER_H
#define LVGLPP_ORIENTATION_FILTER_H

#include "UiTypes.h"

#include <chrono>
#include <optional>

namespace lvglpp::sys {

struct OrientationFilterConfig {
    std::uint32_t stable_ms      = 400;
    std::uint32_t cooldown_ms    = 300;
    float         tilt_threshold = 0.55f;
    float         flat_threshold = 0.80f;
    bool          swap_xy        = false;
    bool          invert_x       = false;
    bool          invert_y       = false;
};

/**
 * Converts a gravity/acceleration vector into one of the four orientations.
 *
 * The caller owns the sensor read loop; feed the vector (typically from an
 * accelerometer, optionally fused with a gyro) and forward a non-empty result
 * to UiConfig::requestOrientation().  Pure logic, safe to unit-test.
 *
 * Default device axes: +x right, +y down, +z towards the user.
 *   +y dominant -> Landscape, +x dominant -> Portrait,
 *   -y dominant -> ReverseLandscape, -x dominant -> ReversePortrait.
 * A flat device (|z| dominant) keeps the current orientation.
 */
class OrientationFilter
{
public:
    explicit OrientationFilter(OrientationFilterConfig config = {});
    ~OrientationFilter() = default;

    void setConfig(const OrientationFilterConfig &config);
    const OrientationFilterConfig &config() const;

    /// Returns the debounced orientation only when it changes.
    std::optional<Orientation> update(float gx, float gy, float gz);

    void setCurrentOrientation(Orientation orientation);
    void reset();

private:
    using Clock = std::chrono::steady_clock;

    Orientation classify(float gx, float gy, float gz) const;

    OrientationFilterConfig   config_;
    std::optional<Orientation> current_;
    std::optional<Orientation> candidate_;
    Clock::time_point          candidate_since_{};
    Clock::time_point          last_switch_{};
    bool                       has_switched_ = false;
};

} // namespace lvglpp::sys

#endif // LVGLPP_ORIENTATION_FILTER_H
