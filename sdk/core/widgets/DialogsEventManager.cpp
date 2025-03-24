/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LhYang

Date: 24-11-11

Class:DialogsEventManager

Description:

**************************************************************************/


#include "DialogsEventManager.h"

namespace heygears
{
    namespace widgets
    {
        DialogsEventManager::DialogsEventManager()
        {
            DIALOG_OPEN = lv_event_code_t(lv_event_register_id());
            DIALOG_CLOSE = lv_event_code_t(lv_event_register_id());
        }

        lv_event_code_t DialogsEventManager::getOpenEventId()
        {
            return DIALOG_OPEN;
        }

        lv_event_code_t DialogsEventManager::getCloseEventId()
        {
            return DIALOG_CLOSE;
        }
    } // widgets
} // heygears