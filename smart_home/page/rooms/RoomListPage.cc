#include "RoomListPage.h"
#include "components/DeviceCard.h"
#include "components/RoomCard.h"
#include "core/log/log.h"
#include "core/sys/StackView.h"
#include "core/widgets/BaseItem.h"
#include "theme/form_design.h"
#include <vector>

namespace lvglpp::ui::page {

using namespace form::design;

RoomListPage::RoomListPage() = default;
RoomListPage::~RoomListPage() = default;

void RoomListPage::onCreate(std::any arg) {
    BaseActivity::onCreate(arg);
    getRoot()->setBgColor(CLR_SURFACE);

    _title = std::make_unique<w::LvText>("Rooms", CLR_ON_SURFACE, getRoot(), w::LvText::FontSize28Bold);
    _title->setAligment(LV_ALIGN_TOP_LEFT, 20, 10);

    struct RoomData { const char *name; const char *icon; int devices; float temp; float humidity; };
    std::vector<RoomData> rooms = {
        {"Living Room",  "Sofa", 4, 22.5f, 48.0f},
        {"Bedroom",      "Bed",  3, 20.0f, 52.0f},
        {"Kitchen",      "Cook", 5, 23.0f, 55.0f},
        {"Bathroom",     "Bath", 2, 24.0f, 60.0f},
        {"Hallway",      "Door", 1, 21.0f, 50.0f},
    };

    int y = 50;
    for (auto &r : rooms) {
        auto card = std::make_unique<ui::widgets::RoomCard>(r.name, r.icon, r.devices, r.temp, r.humidity, getRoot());
        card->setAligment(LV_ALIGN_TOP_LEFT, 20, y);
        card->setOnTap([name = std::string(r.name), this]() { showRoomDetail(name); });
        y += 145;
    }

    LOG_INFO() << "RoomListPage created";
}

void RoomListPage::showRoomDetail(const std::string &roomName) {
    LOG_INFO() << "Opening room: " << roomName;
    // Navigate to room detail page
    // sys::StackView::getInstance().pushView(std::make_shared<RoomDetailPage>(roomName));
}

void RoomListPage::onNotifyUI(const sys::Event &) {}
void RoomListPage::onDestroy() {}

} // namespace lvglpp::ui::page
