#ifndef LVGLPP_FILE_SYSTEM_MANAGER_H
#define LVGLPP_FILE_SYSTEM_MANAGER_H

#include <lvgl.h>

namespace lvglpp::sys {

class FileSystemManager
{
public:
    FileSystemManager(const FileSystemManager &) = delete;
    FileSystemManager &operator=(const FileSystemManager &) = delete;

    static FileSystemManager &getInstance()
    {
        static FileSystemManager instance;
        return instance;
    }

    /// Register both file and memory filesystem drivers with LVGL
    void registerDrivers();

private:
    FileSystemManager() = default;

    lv_fs_drv_t _fileFsDrv{};
    lv_fs_drv_t _memFsDrv{};
};

} // namespace lvglpp::sys

#endif // LVGLPP_FILE_SYSTEM_MANAGER_H
