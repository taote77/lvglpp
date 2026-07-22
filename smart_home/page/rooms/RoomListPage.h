#ifndef SMART_HOME_ROOM_LIST_PAGE_H
#define SMART_HOME_ROOM_LIST_PAGE_H

#include "core/sys/BaseActivity.h"
#include "core/widgets/LvText.h"
#include "core/widgets/PushButton.h"
#include <memory>

namespace w = ::lvglpp::widgets;

namespace lvglpp::ui::page {

/// Room list page showing all rooms with device status
class RoomListPage : public sys::BaseActivity
{
public:
    RoomListPage();
    ~RoomListPage() override;

protected:
    void onCreate(std::any arg) override;
    void onNotifyUI(const sys::Event &evt) override;
    void onDestroy() override;

private:
    void showRoomDetail(const std::string &roomName);

    std::unique_ptr<w::LvText> _title;
};

} // namespace lvglpp::ui::page

#endif
