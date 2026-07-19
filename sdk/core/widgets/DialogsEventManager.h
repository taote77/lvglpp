#ifndef LVGLPP_DIALOGSEVENTMANAGER_H
#define LVGLPP_DIALOGSEVENTMANAGER_H
#include "lvgl/lvgl.h"

namespace lvglpp {
namespace widgets {

class DialogsEventManager
{
public:
    static DialogsEventManager *getInstance()
    {
        static DialogsEventManager instance;
        return &instance;
    }
    DialogsEventManager(const DialogsEventManager &)            = delete;
    DialogsEventManager &operator=(const DialogsEventManager &) = delete;
    DialogsEventManager(DialogsEventManager &&)                 = delete;
    DialogsEventManager &operator=(DialogsEventManager &&)      = delete;

    lv_event_code_t getOpenEventId();
    lv_event_code_t getCloseEventId();

private:
    DialogsEventManager();
    ~DialogsEventManager() = default;

    lv_event_code_t DIALOG_OPEN;
    lv_event_code_t DIALOG_CLOSE;
};

} // namespace widgets
} // namespace lvglpp

#endif // LVGLPP_DIALOGSEVENTMANAGER_H
