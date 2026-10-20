#ifndef LVGLPP_ASSETMANAGER_H
#define LVGLPP_ASSETMANAGER_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace lvglpp::tools {

/**
 * Immutable view of one asset.
 *
 * The bytes stay valid as long as the handle is alive: pack assets point into
 * a read-only mmap (or an owned buffer), directory assets own their file
 * contents. Widgets must keep the handle alive for as long as LVGL references
 * the underlying buffer.
 */
class Asset
{
public:
    Asset(std::string path, const std::uint8_t *data, std::size_t size, std::shared_ptr<const void> owner) :
        path_(std::move(path)), data_(data), size_(size), owner_(std::move(owner))
    {}

    const std::string &path() const
    {
        return path_;
    }

    const std::uint8_t *data() const
    {
        return data_;
    }

    std::size_t size() const
    {
        return size_;
    }

    std::string_view text() const
    {
        return std::string_view(reinterpret_cast<const char *>(data_), size_);
    }

private:
    std::string                 path_;
    const std::uint8_t         *data_ = nullptr;
    std::size_t                 size_ = 0;
    std::shared_ptr<const void> owner_;
};

using AssetHandle = std::shared_ptr<const Asset>;

/**
 * Read-only asset sources (LVPAK1 packs and plain directories).
 *
 * Logical paths are relative, '/'-separated and must be canonical
 * (no leading '/', no '.'/'..' segments).
 *
 * Mount order defines priority: the last mounted source wins. The intended
 * layout is common assets first, then the application-specific pack.
 */
class AssetManager
{
public:
    static AssetManager &getInstance();

    AssetManager(const AssetManager &)            = delete;
    AssetManager &operator=(const AssetManager &) = delete;

    /// Mount a LVPAK1 file. Returns false and sets error on malformed packs.
    bool mountPack(const std::string &packPath, std::string *error = nullptr);

    /// Mount a plain directory. Files are re-read on every open (hot reload).
    bool mountDirectory(const std::string &root, std::string *error = nullptr);

    /// Mount pack bytes; the data is copied, so the caller may free it.
    bool mountMemory(const void *packBytes, std::size_t size, std::string *error = nullptr);

    void reset();

    /// Open an asset and verify its data CRC (pack sources only).
    AssetHandle open(const std::string &logicalPath) const;

    /// Internal fast path for the read-only LVGL FS driver (skips data CRC).
    AssetHandle openUnchecked(const std::string &logicalPath) const;

    bool exists(const std::string &logicalPath) const;

    std::vector<std::string> list(const std::string &prefix = {}) const;

    /**
     * Return a real filesystem path for an asset.
     *
     * Directory sources return their path directly; pack entries are extracted
     * to a per-process temporary directory. Needed by libraries that bypass
     * LVGL's FS API (FreeType without the LVGL port, ffmpeg).
     */
    bool materialize(const std::string &logicalPath, std::string &outPath);

    /**
     * Auto-discover runtime sources:
     *   1. LVGLPP_ASSET_DIR (directory, exclusive)
     *   2. LVGLPP_ASSET_PACK (colon-separated pack list)
     *   3. <baseDir>/common.pak or common.asset/
     *   4. <baseDir>/<exeName>.pak or <exeName>.asset/
     *   5. <baseDir>/asset/ (legacy fallback)
     */
    bool mountDefaults(const std::string &baseDir, const std::string &exeName, std::string *error = nullptr);

    std::size_t mountCount() const;

    /// Human-readable source descriptions, low to high priority (diagnostics).
    std::vector<std::string> mountNames() const;

private:
    AssetManager();
    ~AssetManager();

    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace lvglpp::tools

#endif // LVGLPP_ASSETMANAGER_H
