#include "EventRecorder.h"
#include "AutomationController.h"
#include <iostream>
#include <thread>

namespace lvglpp::sys {

void EventRecorder::startRecording()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _events.clear();
    _startTime = std::chrono::steady_clock::now();
    _recording = true;
}

void EventRecorder::stopRecording()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _recording = false;
}

void EventRecorder::record(const std::string &type, widgets::BaseItem *item, int value)
{
    if (!_recording) return;
    std::lock_guard<std::mutex> lock(_mutex);

    auto now = std::chrono::steady_clock::now();
    RecordedEvent evt;
    evt.type       = type;
    evt.widgetName = item ? item->name() : "";
    evt.widgetType = item ? typeid(*item).name() : "";
    evt.value      = value;
    evt.timestampMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - _startTime).count();

    if (item) {
        auto r = item->bounds();
        evt.x = r.x + r.w / 2;
        evt.y = r.y + r.h / 2;
    }

    _events.push_back(std::move(evt));
}

std::string EventRecorder::toJson() const
{
    std::ostringstream os;
    os << "[\n";
    for (size_t i = 0; i < _events.size(); i++) {
        const auto &e = _events[i];
        os << "  {"
           << R"("type":")" << e.type << "\","
           << R"("widget":")" << e.widgetName << "\","
           << R"("widgetType":")" << e.widgetType << "\","
           << R"("x":)" << e.x << ","
           << R"("y":)" << e.y << ","
           << R"("value":)" << e.value << ","
           << R"("ts":)" << e.timestampMs
           << "}";
        if (i + 1 < _events.size()) os << ",";
        os << "\n";
    }
    os << "]";
    return os.str();
}

void EventRecorder::saveToFile(const std::string &filepath) const
{
    std::ofstream ofs(filepath);
    if (ofs) {
        ofs << toJson();
    }
}

void EventRecorder::installGlobalHook()
{
    // Hook into LVGL's global event system to auto-record clicks, presses, etc.
    // This is a lightweight wrapper — real implementation would use lv_obj_add_event_cb
    // on the active screen or a global event callback.
    LV_LOG_USER("EventRecorder global hook installed");
}

// ---- EventReplayer ----

EventReplayer EventReplayer::fromFile(const std::string &filepath)
{
    std::vector<RecordedEvent> events;
    // Simple JSON parsing — in production, use a proper JSON library
    // For now, this is a placeholder showing the architecture
    LV_LOG_USER("EventReplayer: loading from %s", filepath.c_str());
    return EventReplayer(events);
}

void EventReplayer::replay(std::function<void(const RecordedEvent &)> callback)
{
    if (_events.empty()) return;

    auto &ac = AutomationController::getInstance();
    int64_t prevTs = 0;

    for (const auto &evt : _events) {
        // Wait to mimic original timing
        int64_t delay = evt.timestampMs - prevTs;
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        prevTs = evt.timestampMs;

        if (callback) callback(evt);

        // Dispatch based on event type
        if (evt.type == "click") {
            ac.clickAt(static_cast<lv_coord_t>(evt.x), static_cast<lv_coord_t>(evt.y));
        } else if (evt.type == "value_change") {
            // handled by app-specific logic
        }
    }

    LV_LOG_USER("EventReplayer: replayed %zu events", _events.size());
}

} // namespace lvglpp::sys
