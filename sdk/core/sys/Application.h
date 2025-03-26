/**************************************************************************

Class:Application

Description:应用程序管理虚基类

**************************************************************************/

#ifndef LVGL_APPLICATION_H
#define LVGL_APPLICATION_H

#include "Event.h"
#include <lvgl.h>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

namespace lvglpp::sys {
/**
 * virtual class Application
 */
class Application
{
public:
    enum LanguageType { Chinese, Chinese2English };

    Application(int argc, char *argv[]);

    virtual ~Application() = default;

    static const std::string &applicationDirPath() { return application_dir_path_; }

    static const std::string &applicationName() { return application_name_; }

    static void setLanguageType(LanguageType type)
    {
        language_type_ = type;
        LogDebug << "Set Language_type_:"
                 << (language_type_ == LanguageType::Chinese ? "English" : "English2Chinese");
    }

    static LanguageType getLanguageType() { return language_type_; }

    virtual int exec();

    void postEvent(const Event &evt);

protected:
    virtual bool initApp();

    virtual void exit(int c) = 0;

    void handleEvent();

    bool isInit() const { return is_init_; }

    void setInitStatus(bool b_init) { is_init_ = b_init; }

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
} // namespace lvglpp::sys

#endif // LVGL_APPLICATION_H
