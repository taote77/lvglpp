/**************************************************************************

Class: Application

Description: Abstract base class for application management

**************************************************************************/

#ifndef LVGL_APPLICATION_H
#define LVGL_APPLICATION_H

#include "Event.h"
#include <lvgl.h>
#include <any>
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
    enum LanguageType { Chinese, English, Japanese, Russian, LOCAL };

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
        LogDebug << "Set Language_type_:" << (language_type_ == LanguageType::Chinese ? "English" : "English2Chinese");
    }

    static LanguageType getLanguageType()
    {
        return language_type_;
    }

    void setEnableStackView(bool enable)
    {
        enable_stack_view_ = enable;
    }

    void initStackView();

    virtual int exec();

    void postEvent(const Event &evt);

protected:
    virtual bool initApp();

    virtual void exit(int c) = 0;

    void handleEvent();

    bool isInit() const
    {
        return is_init_;
    }

    void setInitStatus(bool b_init)
    {
        is_init_ = b_init;
    }

private:
    bool                is_init_ = false;
    static std::string  application_dir_path_;
    static std::string  application_name_;
    static LanguageType language_type_;
    std::queue<Event>   event_queue_;
    std::mutex          mutex_;
    bool                enable_stack_view_ = true;
};
} // namespace lvglpp::sys

#endif // LVGL_APPLICATION_H
