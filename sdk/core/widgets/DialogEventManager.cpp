#include "DialogEventManager.h"

namespace lvglpp {
namespace widgets {

DialogEventManager::DialogEventManager()
{
    DIALOG_OPEN  = lv_event_code_t(lv_event_register_id());
    DIALOG_CLOSE = lv_event_code_t(lv_event_register_id());
}

lv_event_code_t DialogEventManager::getOpenEventId()
{
    return DIALOG_OPEN;
}

lv_event_code_t DialogEventManager::getCloseEventId()
{
    return DIALOG_CLOSE;
}
} // namespace widgets
} // namespace lvglpp