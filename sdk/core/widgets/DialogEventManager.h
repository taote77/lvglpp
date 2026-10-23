#ifndef LVGLPP_DIALOGEVENTMANAGER_H
#define LVGLPP_DIALOGEVENTMANAGER_H
#include "lvgl/lvgl.h"

namespace lvglpp {
namespace widgets {

class DialogEventManager
{
public:
    static DialogEventManager *getInstance()
    {
        static DialogEventManager instance;
        return &instance;
    }
    DialogEventManager(const DialogEventManager &)            = delete;
    DialogEventManager &operator=(const DialogEventManager &) = delete;
    DialogEventManager(DialogEventManager &&)                 = delete;
    DialogEventManager &operator=(DialogEventManager &&)      = delete;

    lv_event_code_t getOpenEventId();
    lv_event_code_t getCloseEventId();

private:
    DialogEventManager();
    ~DialogEventManager() = default;

    lv_event_code_t DIALOG_OPEN;
    lv_event_code_t DIALOG_CLOSE;
};

} // namespace widgets
} // namespace lvglpp

#endif // LVGLPP_DIALOGEVENTMANAGER_H
