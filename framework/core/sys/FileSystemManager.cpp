#include "FileSystemManager.h"

#include "core/log/log.h"
#include "core/tools/AssetManager.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

#include <lvgl.h>

namespace lvglpp::sys {

namespace {

struct AssetFile {
    tools::AssetHandle asset;
    std::size_t        position = 0;
};

// lv_fs_drv_register() stores the pointer; the struct must outlive LVGL.
lv_fs_drv_t g_asset_driver;

std::string normalizePath(const char *path)
{
    if (path == nullptr)
    {
        return {};
    }
    std::string normalized(path);
    while (!normalized.empty() && normalized.front() == '/')
    {
        normalized.erase(normalized.begin());
    }
    return normalized;
}

void *assetOpen(lv_fs_drv_t *, const char *path, lv_fs_mode_t mode)
{
    if ((mode & LV_FS_MODE_WR) != 0)
    {
        return nullptr;
    }

    const std::string normalized = normalizePath(path);
    if (normalized.empty())
    {
        return nullptr;
    }

    tools::AssetHandle asset = tools::AssetManager::getInstance().openUnchecked(normalized);
    if (!asset)
    {
        return nullptr;
    }
    if (asset->size() > std::numeric_limits<std::uint32_t>::max())
    {
        LOG_WARN() << "asset is too large for the LVGL FS API: " << normalized;
        return nullptr;
    }

    auto *file  = new AssetFile();
    file->asset = std::move(asset);
    return file;
}

lv_fs_res_t assetClose(lv_fs_drv_t *, void *file)
{
    delete static_cast<AssetFile *>(file);
    return LV_FS_RES_OK;
}

lv_fs_res_t assetRead(lv_fs_drv_t *, void *file, void *buffer, std::uint32_t bytesToRead, std::uint32_t *bytesRead)
{
    if (file == nullptr || bytesRead == nullptr)
    {
        return LV_FS_RES_INV_PARAM;
    }

    auto *handle = static_cast<AssetFile *>(file);
    *bytesRead   = 0;

    if (handle->position >= handle->asset->size())
    {
        return LV_FS_RES_OK;
    }

    const std::size_t remaining = handle->asset->size() - handle->position;
    const std::size_t readSize  = std::min<std::size_t>(bytesToRead, remaining);
    std::memcpy(buffer, handle->asset->data() + handle->position, readSize);
    handle->position += readSize;
    *bytesRead = static_cast<std::uint32_t>(readSize);
    return LV_FS_RES_OK;
}

lv_fs_res_t assetSeek(lv_fs_drv_t *, void *file, std::uint32_t position, lv_fs_whence_t whence)
{
    if (file == nullptr)
    {
        return LV_FS_RES_INV_PARAM;
    }

    auto *handle = static_cast<AssetFile *>(file);

    std::int64_t target = 0;
    if (whence == LV_FS_SEEK_SET)
    {
        target = static_cast<std::int64_t>(position);
    } else if (whence == LV_FS_SEEK_CUR)
    {
        target = static_cast<std::int64_t>(handle->position) + static_cast<std::int32_t>(position);
    } else
    {
        target = static_cast<std::int64_t>(handle->asset->size()) + static_cast<std::int32_t>(position);
    }

    target           = std::clamp<std::int64_t>(target, 0, static_cast<std::int64_t>(handle->asset->size()));
    handle->position = static_cast<std::size_t>(target);
    return LV_FS_RES_OK;
}

lv_fs_res_t assetTell(lv_fs_drv_t *, void *file, std::uint32_t *position)
{
    if (file == nullptr || position == nullptr)
    {
        return LV_FS_RES_INV_PARAM;
    }

    const auto *handle = static_cast<const AssetFile *>(file);
    if (handle->position > std::numeric_limits<std::uint32_t>::max())
    {
        return LV_FS_RES_UNKNOWN;
    }
    *position = static_cast<std::uint32_t>(handle->position);
    return LV_FS_RES_OK;
}

} // namespace

void FileSystemManager::registerAssetDriver()
{
    if (registered_)
    {
        return;
    }

    lv_fs_drv_init(&g_asset_driver);
    g_asset_driver.letter       = 'A';
    g_asset_driver.cache_size   = 0;
    g_asset_driver.open_cb      = assetOpen;
    g_asset_driver.close_cb     = assetClose;
    g_asset_driver.read_cb      = assetRead;
    g_asset_driver.seek_cb      = assetSeek;
    g_asset_driver.tell_cb      = assetTell;
    g_asset_driver.write_cb     = nullptr;
    g_asset_driver.dir_open_cb  = nullptr;
    g_asset_driver.dir_read_cb  = nullptr;
    g_asset_driver.dir_close_cb = nullptr;

    lv_fs_drv_register(&g_asset_driver);
    registered_ = true;

    LOG_DEBUG() << "registered asset filesystem driver 'A:'";
}

} // namespace lvglpp::sys
