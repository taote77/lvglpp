/**************************************************************************

Copyright:HeyGears

Author: LnJan

Date:2022-03-17

Class:Application

Description:应用程序管理虚基类

**************************************************************************/

#ifndef LVGL_APPLICATION_H
#define LVGL_APPLICATION_H

#include <string>
#include <memory>
#include <map>
#include <queue>
#include <mutex>
#include <lvgl.h>
#include "Event.h"

namespace heygears
{
    namespace sys
    {
        /**
         * virtual class Application
         */
        class Application
        {
        public:

            enum LanguageType
            {
                English,
                English2Chinese
            };

            Application(int argc, char *argv[]);

            virtual ~Application() = default;

            static const std::string &applicationDirPath()
            {
                return application_dir_path_;
            }

            static const std::string &applicationName()
            {
                return application_name_;
            }

            static void setLanguageType(LanguageType type)
            {
                language_type_ = type;
                LogDebug << "Set Language_type_:" << (language_type_ == LanguageType::English ? "English" : "English2Chinese");
            }

            static LanguageType getLanguageType()
            {
                return language_type_;
            }

            virtual int exec();

            void postEvent(const Event &e);

        protected:
            virtual bool initApp();

            virtual void exit(int c) = 0;

            void handleEvent();

            bool isInit() const
            {
                return is_init_;
            }

            void setInitStatus(bool b)
            {
                is_init_ = b;
            }

        private:
            bool is_init_ = false;
            static std::string application_dir_path_;
            static std::string application_name_;
            static LanguageType language_type_;
            std::queue<Event> event_queue_;
            std::mutex mutex_;
            lv_fs_drv_t file_assert_fs_drv_;
            lv_fs_drv_t mem_assert_fs_drv_;
        };
    }
}

#endif //LVGL_APPLICATION_H
