/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-12

Class:${CLASS}

Description:lvgl对话框基类

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_BASEDIALOG_H
#define ULTRACORE_FOR_MAKERS_BASEDIALOG_H
#include "BaseItem.h"
#include <memory>
namespace heygears
{
    namespace widgets
    {
        class BaseDialog
        {
        public:
            explicit BaseDialog(bool is_sys_dialog = false);
            virtual void open();
            virtual void close();
            bool isOpen();
            void setAutoDel(bool b)
            {
                auto_del_ = b;
            }
            bool getAutoDel() const
            {
                return auto_del_;
            }

            void setOnClicked(const std::function<void()> &func)
            {
                background_item_ptr_->setEnable(true);
                background_item_ptr_->setOnClickedListener(func);
            }

            void setOpacity(double opacity)
            {
                background_item_ptr_->setOpacity(opacity);
            }

        protected:
            BaseItem *getRootItem() const;
            virtual ~BaseDialog() = default;

        private:
            std::shared_ptr<BaseItem> background_item_ptr_;
            bool auto_del_ = true;
        };

    }// namespace widgets
}// namespace heygears

#endif//ULTRACORE_FOR_MAKERS_BASEDIALOG_H
