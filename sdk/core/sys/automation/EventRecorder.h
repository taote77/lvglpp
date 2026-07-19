#ifndef LVGLPP_EVENT_RECORDER_H
#define LVGLPP_EVENT_RECORDER_H

#include "core/widgets/BaseItem.h"
#include <chrono>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace lvglpp::sys {

/// A single recorded UI event
struct RecordedEvent {
    std::string type;       // "click", "press", "release", "value_change", "scroll"
    std::string widgetName; // BaseItem::name()
    std::string widgetType; // typeid(*item).name()
    int x = 0, y = 0;      // screen coordinates
    int value = 0;         // for value_change events
    int64_t timestampMs;   // ms since recording start
};

/// Records UI events for testing, debugging, and AI observation
class EventRecorder
{
public:
    static EventRecorder &getInstance()
    {
        static EventRecorder instance;
        return instance;
    }

    EventRecorder(const EventRecorder &) = delete;
    EventRecorder &operator=(const EventRecorder &) = delete;

    void startRecording();
    void stopRecording();
    bool isRecording() const { return _recording; }

    /// Record an event manually (call from LVGL event callbacks)
    void record(const std::string &type, widgets::BaseItem *item, int value = 0);

    /// Get all recorded events
    const std::vector<RecordedEvent> &events() const { return _events; }

    /// Clear recorded events
    void clear() { _events.clear(); _startTime = {}; }

    /// Export to JSON for AI analysis
    std::string toJson() const;

    /// Save to file
    void saveToFile(const std::string &filepath) const;

    /// Install global LVGL event hook to auto-record all events
    void installGlobalHook();

private:
    EventRecorder() = default;

    bool _recording = false;
    std::chrono::steady_clock::time_point _startTime;
    std::vector<RecordedEvent> _events;
    std::mutex _mutex;
};

/// Replays previously recorded events
class EventReplayer
{
public:
    explicit EventReplayer(const std::vector<RecordedEvent> &events)
        : _events(events) {}

    /// Load events from a JSON file
    static EventReplayer fromFile(const std::string &filepath);

    /// Replay all events with original timing
    /// @param callback called for each event before dispatching (for monitoring)
    void replay(std::function<void(const RecordedEvent &)> callback = {});

private:
    std::vector<RecordedEvent> _events;
};

} // namespace lvglpp::sys

#endif // LVGLPP_EVENT_RECORDER_H
