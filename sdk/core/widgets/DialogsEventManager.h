/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LhYang

Date: 24-11-11

Class:DialogsEventManager

Description:

**************************************************************************/


#ifndef ULTRACORE_REFLEX_DIALOGSEVENTMANAGER_H
#define ULTRACORE_REFLEX_DIALOGSEVENTMANAGER_H
#include "lvgl/lvgl.h"

namespace heygears
{
    namespace widgets
    {

        class DialogsEventManager
        {
        public:
            static DialogsEventManager* getInstance()
            {
                static DialogsEventManager instance;
                return &instance;
            }
            DialogsEventManager(const DialogsEventManager&) = delete;
            DialogsEventManager& operator=(const DialogsEventManager&) = delete;
            DialogsEventManager(DialogsEventManager&&) = delete;
            DialogsEventManager& operator=(DialogsEventManager&&) = delete;

            lv_event_code_t getOpenEventId();
            lv_event_code_t getCloseEventId();

        private:
            DialogsEventManager();
            ~DialogsEventManager() = default;

            lv_event_code_t DIALOG_OPEN;
            lv_event_code_t DIALOG_CLOSE;
        };

    } // widgets
} // heygears

#endif //ULTRACORE_REFLEX_DIALOGSEVENTMANAGER_H
