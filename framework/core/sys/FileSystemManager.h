#ifndef LVGLPP_FILE_SYSTEM_MANAGER_H
#define LVGLPP_FILE_SYSTEM_MANAGER_H

namespace lvglpp::sys {

/**
 * Registers the SDK's read-only LVGL filesystem driver.
 *
 * Drive 'A' exposes the mounted AssetManager sources. It is used by widgets
 * that can only consume an LVGL file path (GIF, fonts via the LVGL FreeType
 * port, ...). Images normally go through ImageManager instead, which keeps
 * decoded descriptors cached.
 */
class FileSystemManager
{
public:
    FileSystemManager(const FileSystemManager &)            = delete;
    FileSystemManager &operator=(const FileSystemManager &) = delete;

    static FileSystemManager &getInstance()
    {
        static FileSystemManager instance;
        return instance;
    }

    /// Register the 'A:' asset driver. Idempotent.
    void registerAssetDriver();

private:
    FileSystemManager() = default;

    bool registered_ = false;
};

} // namespace lvglpp::sys

#endif // LVGLPP_FILE_SYSTEM_MANAGER_H
