/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-29

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_TOASTMANAGER_H
#define ULTRACORE_FOR_MAKERS_TOASTMANAGER_H
#include "Toast.h"
#include <string>
#include <memory>
namespace heygears
{
    namespace widgets
    {

        class ToastManager
        {
        public:
            enum ToastLevel{
                Succeed,
                Error,
                Warn
            };
            static void showToast(const std::string& msg,ToastLevel level);
        private:
            ToastManager();
            ToastManager(const ToastManager&)=default;
            std::shared_ptr<Toast> toast_impl_;
            static ToastManager* instance_;
            lv_anim_t anim_show_t_{};
            lv_anim_t anim_hide_t_{};
            bool is_running_=false;
        };

    } // heygears
} // widgets

#endif //ULTRACORE_FOR_MAKERS_TOASTMANAGER_H
