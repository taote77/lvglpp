#include "FileSystemManager.h"
#include "core/log/log.h"
#include "core/tools/MemoryResManager.h"
#include "core/tools/Utils.h"
#include <cstring>

namespace lvglpp::sys {

void FileSystemManager::registerDrivers()
{
    // ---- File asset filesystem ----
    lv_fs_drv_init(&_fileFsDrv);
    _fileFsDrv.letter     = tools::Utils::getAssetetter();
    _fileFsDrv.cache_size = 2 * 1024;

    _fileFsDrv.open_cb = [](struct _lv_fs_drv_t *, const char *path, lv_fs_mode_t) -> void * {
        std::string open_file_path = tools::Utils::getAssetDirRoot();
        open_file_path += ("/" + std::string(path));
        return fopen(open_file_path.c_str(), "rb");
    };
    _fileFsDrv.close_cb = [](struct _lv_fs_drv_t *, void *file_p) -> lv_fs_res_t {
        if (file_p == nullptr) return LV_FS_RES_FS_ERR;
        fclose(static_cast<FILE *>(file_p));
        return LV_FS_RES_OK;
    };
    _fileFsDrv.read_cb = [](struct _lv_fs_drv_t *, void *file_p, void *buf, uint32_t btr, uint32_t *br) -> lv_fs_res_t {
        if (file_p == nullptr) return LV_FS_RES_FS_ERR;
        memset(buf, 0, btr);
        *br = fread(buf, 1, btr, static_cast<FILE *>(file_p));
        return LV_FS_RES_OK;
    };
    _fileFsDrv.write_cb = [](struct _lv_fs_drv_t *, void *file_p, const void *buf, uint32_t btw, uint32_t *bw) -> lv_fs_res_t {
        if (file_p == nullptr) return LV_FS_RES_FS_ERR;
        *bw = fwrite(buf, 1, btw, static_cast<FILE *>(file_p));
        return LV_FS_RES_OK;
    };
    _fileFsDrv.tell_cb = [](struct _lv_fs_drv_t *, void *file_p, uint32_t *pos_p) -> lv_fs_res_t {
        if (file_p == nullptr) return LV_FS_RES_FS_ERR;
        *pos_p = ftell(static_cast<FILE *>(file_p));
        return LV_FS_RES_OK;
    };
    _fileFsDrv.seek_cb = [](struct _lv_fs_drv_t *, void *file_p, uint32_t pos, lv_fs_whence_t whence) -> lv_fs_res_t {
        if (file_p == nullptr) return LV_FS_RES_FS_ERR;
        fseek(static_cast<FILE *>(file_p), pos, whence);
        return LV_FS_RES_OK;
    };
    lv_fs_drv_register(&_fileFsDrv);
    LogDebug << "register lvgl file system finished!!!";

    // ---- Memory asset filesystem ----
    lv_fs_drv_init(&_memFsDrv);
    _memFsDrv.letter     = tools::Utils::getMemAssetLetter();
    _memFsDrv.cache_size = 32 * 1024;

    _memFsDrv.open_cb = [](struct _lv_fs_drv_t *, const char *path, lv_fs_mode_t) -> void * {
        return static_cast<void *>(tools::MemoryResManager::getInstance().getResourceByUrl(path));
    };
    _memFsDrv.close_cb = [](struct _lv_fs_drv_t *, void *file_p) -> lv_fs_res_t {
        if (auto *info = static_cast<tools::ResourceInfo *>(file_p)) {
            tools::MemoryResManager::getInstance().freeResource(info);
        }
        return LV_FS_RES_OK;
    };
    _memFsDrv.read_cb = [](struct _lv_fs_drv_t *, void *file_p, void *buf, uint32_t btr, uint32_t *br) -> lv_fs_res_t {
        auto *info = static_cast<tools::ResourceInfo *>(file_p);
        if (info == nullptr) return LV_FS_RES_FS_ERR;
        memset(buf, 0, btr);
        int readLen = std::min(static_cast<int>(btr), static_cast<int>(info->data.length() - info->current_pos));
        memcpy(buf, info->data.data() + info->current_pos, readLen);
        *br = readLen;
        return LV_FS_RES_OK;
    };
    _memFsDrv.write_cb = [](struct _lv_fs_drv_t *, void *, const void *, uint32_t, uint32_t *) -> lv_fs_res_t {
        return LV_FS_RES_NOT_IMP;
    };
    _memFsDrv.tell_cb = [](struct _lv_fs_drv_t *, void *file_p, uint32_t *pos_p) -> lv_fs_res_t {
        auto *info = static_cast<tools::ResourceInfo *>(file_p);
        if (info == nullptr) return LV_FS_RES_FS_ERR;
        *pos_p = info->data.length();
        return LV_FS_RES_OK;
    };
    _memFsDrv.seek_cb = [](struct _lv_fs_drv_t *, void *file_p, uint32_t pos, lv_fs_whence_t whence) -> lv_fs_res_t {
        auto *info = static_cast<tools::ResourceInfo *>(file_p);
        if (info == nullptr) return LV_FS_RES_FS_ERR;
        if (whence == LV_FS_SEEK_SET)
            info->current_pos = pos;
        else if (whence == LV_FS_SEEK_CUR)
            info->current_pos += pos;
        else
            info->current_pos = info->data.length() - pos;
        return LV_FS_RES_OK;
    };
    lv_fs_drv_register(&_memFsDrv);
    LogDebug << "register lvgl memory file system finished!!!";
}

} // namespace lvglpp::sys
