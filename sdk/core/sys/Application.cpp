
#include "Application.h"
#include "BaseActivity.h"
#include "Navigators.h"
#include "core/tools/MemoryResManager.h"
#include "core/tools/Utils.h"
#include "core/log/log.h"
#include <boost/dll.hpp>
#include <thread>

namespace heygears::sys {

std::string Application::application_dir_path_;
std::string Application::application_name_;

Application::LanguageType Application::language_type_ = Application::Chinese;

Application::Application(int argc, char *argv[])
{
    Navigators::getInstance()->setApplication(this);
}

bool Application::initApp()
{
    auto path =
            boost::dll::program_location()
                    .parent_path(); // boost::filesystem::initial_path<boost::filesystem::path>();
    application_dir_path_ = path.string();
    // application_name_ = "";
    LogDebug << "path:" << application_dir_path_;
    lv_init();
    // register app filesystem
    lv_fs_drv_init(&file_assert_fs_drv_);
    file_assert_fs_drv_.letter = tools::Utils::getAssetetter();
    file_assert_fs_drv_.cache_size = 2 * 1024;
    file_assert_fs_drv_.open_cb = [](struct _lv_fs_drv_t *drv, const char *path,
                                     lv_fs_mode_t mode) -> void * {
        std::string open_file_path;
        open_file_path = tools::Utils::getAssetDirRoot();
        open_file_path += ("/" + std::string(path));
        return fopen(open_file_path.c_str(), "rb");
    };
    file_assert_fs_drv_.close_cb = [](struct _lv_fs_drv_t *drv, void *file_p) -> lv_fs_res_t {
        if (file_p == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        fclose((FILE *)file_p);
        return LV_FS_RES_OK;
    };
    file_assert_fs_drv_.read_cb = [](struct _lv_fs_drv_t *drv, void *file_p, void *buf,
                                     uint32_t btr, uint32_t *br) -> lv_fs_res_t {
        if (file_p == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        memset(buf, 0, btr);
        (*br) = fread(buf, 1, btr, (FILE *)file_p);
        return LV_FS_RES_OK;
    };
    file_assert_fs_drv_.write_cb = [](struct _lv_fs_drv_t *drv, void *file_p, const void *buf,
                                      uint32_t btw, uint32_t *bw) -> lv_fs_res_t {
        if (file_p == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        (*bw) = fwrite(buf, 1, btw, (FILE *)file_p);
        return LV_FS_RES_OK;
    };
    file_assert_fs_drv_.tell_cb = [](struct _lv_fs_drv_t *drv, void *file_p,
                                     uint32_t *pos_p) -> lv_fs_res_t {
        if (file_p == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        (*pos_p) = ftell((FILE *)file_p);
        return LV_FS_RES_OK;
    };
    file_assert_fs_drv_.seek_cb = [](struct _lv_fs_drv_t *drv, void *file_p, uint32_t pos,
                                     lv_fs_whence_t whence) -> lv_fs_res_t {
        if (file_p == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        fseek((FILE *)file_p, pos, whence);
        return LV_FS_RES_OK;
    };
    lv_fs_drv_register(&file_assert_fs_drv_);
    LogDebug << "register lvgl file system finished!!!";

    // register app memory filesystem
    lv_fs_drv_init(&mem_assert_fs_drv_);
    mem_assert_fs_drv_.letter = tools::Utils::getMemAssetLetter();
    mem_assert_fs_drv_.cache_size = 32 * 1024;
    mem_assert_fs_drv_.open_cb = [](struct _lv_fs_drv_t *drv, const char *path,
                                    lv_fs_mode_t mode) -> void * {
        return (void *)tools::MemoryResManager::getInstance()->getResourceByUrl(path);
    };
    mem_assert_fs_drv_.close_cb = [](struct _lv_fs_drv_t *drv, void *file_p) -> lv_fs_res_t {
        auto *info = (tools::ResourceInfo *)file_p;
        if (info != nullptr) {
            tools::MemoryResManager::getInstance()->freeResource(info);
        }
        return LV_FS_RES_OK;
    };
    mem_assert_fs_drv_.read_cb = [](struct _lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr,
                                    uint32_t *br) -> lv_fs_res_t {
        auto *info = (tools::ResourceInfo *)file_p;
        if (info == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        memset(buf, 0, btr);
        int readLen = std::min((int)btr, (int)(info->data.length() - info->current_pos));
        memcpy(buf, info->data.data() + info->current_pos, readLen);
        (*br) = readLen;
        return LV_FS_RES_OK;
    };
    mem_assert_fs_drv_.write_cb = [](struct _lv_fs_drv_t *drv, void *file_p, const void *buf,
                                     uint32_t btw,
                                     uint32_t *bw) -> lv_fs_res_t { return LV_FS_RES_NOT_IMP; };
    mem_assert_fs_drv_.tell_cb = [](struct _lv_fs_drv_t *drv, void *file_p,
                                    uint32_t *pos_p) -> lv_fs_res_t {
        auto *info = (tools::ResourceInfo *)file_p;
        if (info == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        (*pos_p) = info->data.length();
        return LV_FS_RES_OK;
    };
    mem_assert_fs_drv_.seek_cb = [](struct _lv_fs_drv_t *drv, void *file_p, uint32_t pos,
                                    lv_fs_whence_t whence) -> lv_fs_res_t {
        auto *info = (tools::ResourceInfo *)file_p;
        if (file_p == nullptr) {
            return LV_FS_RES_FS_ERR;
        }
        if (whence == LV_FS_SEEK_SET) {
            info->current_pos = pos;
        } else if (whence == LV_FS_SEEK_CUR) {
            info->current_pos += pos;
        } else {
            info->current_pos = info->data.length() - pos;
        }
        return LV_FS_RES_OK;
    };
    lv_fs_drv_register(&mem_assert_fs_drv_);
    LogDebug << "register lvgl memory file system finished!!!";

    // use lvgl png file
    lv_lodepng_init();

    // init log printf
    lv_log_register_print_cb(
            [](lv_log_level_t level, const char *buf) -> void { LogWarn << "lvgl logs:" << buf; });
#if LV_TICK_CUSTOM == 0
    std::thread tick_thread([]() -> void {
        while (true) {
            std::chrono::milliseconds duration(5);
            std::this_thread::sleep_for(duration);

            lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
        }
    });
    tick_thread.detach();
#endif

    setInitStatus(true);
    return true;
}

int Application::exec()
{
    if (!isInit()) {
        initApp();
    }
    Navigators::getInstance()->start();
    if (Navigators::getInstance()->depth() == 0) {
        Navigators::getInstance()->pushView(std::shared_ptr<Activity>(new BaseActivity()));
    }

    while (true) {
        handleEvent();
        /* Periodically call the lv_task handler.
         * It could be done in a timer interrupt or an OS task too.*/
        lv_timer_handler();
        usleep(5 * 1000);
    }
    return 0;
}

void Application::postEvent(const Event &evt)
{
    std::lock_guard<std::mutex> lock(mutex_);

    event_queue_.push(evt);
}

void Application::handleEvent()
{
    std::lock_guard<std::mutex> lock(mutex_);

    Navigators::getInstance()->clearDeleteVec();

    // 需要注意
    if (event_queue_.empty()) {
        return;
    }

    Event event_ref = event_queue_.front();

    event_queue_.pop();
    Navigators::getInstance()->notifyAllUi(event_ref);
}
} // namespace heygears::sys
