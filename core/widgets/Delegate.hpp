/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-22

Class:${CLASS}

Description:列表控件代理基类

**************************************************************************/

#ifndef LVGL_XFJ_DEMO_DELEGATE_HPP
#define LVGL_XFJ_DEMO_DELEGATE_HPP

#include "ScrollItem.h"

namespace heygears
{
    namespace widgets
    {
        template<class T>
        class Delegate : public ScrollItem
        {
        public:
            explicit Delegate(BaseItem *parent = nullptr) : ScrollItem(parent)
            {
            }

            ~Delegate() override;

            void initItem(const T &t)
            {
                draw(t);
            }

            bool updateItem(const T &t)
            {
                return updateData(t);
            }

        protected:
            virtual void draw(const T &) = 0;
            virtual bool updateData(const T&)
            {
                return false;
            }

        private:

        };

        template<class T>
        Delegate<T>::~Delegate()
        {
            if (getLvglItem() != nullptr && lv_obj_is_valid(getLvglItem()))
            {
                //LV_LOG_USER("Delegate delete!! %ld",getLvglItem());
                lv_obj_del(getLvglItem());
                setLvglItem(nullptr);
            }
        }

    }
} // widgets

#endif //LVGL_XFJ_DEMO_DELEGATE_HPP
