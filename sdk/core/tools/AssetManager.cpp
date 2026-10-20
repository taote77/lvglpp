#include "AssetManager.h"

#include "core/log/log.h"
#include "core/tools/assets/AssetPack.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace lvglpp::tools {

namespace {

namespace fs = std::filesystem;

enum class SourceResult {
    NotFound,
    Ok,
    Error,
};

class AssetSource
{
public:
    virtual ~AssetSource() = default;

    virtual SourceResult open(const std::string &path, bool verifyCrc, const std::uint8_t *&data, std::size_t &size, std::shared_ptr<const void> &owner,
                              std::string *error) const = 0;

    virtual bool                     exists(const std::string &path) const = 0;
    virtual std::vector<std::string> list(const std::string &prefix) const = 0;
    virtual std::string              name() const                          = 0;

    virtual bool realPath(const std::string &, std::filesystem::path &) const
    {
        return false;
    }
};

class PackSource final : public AssetSource
{
public:
    static std::shared_ptr<PackSource> fromFile(const std::string &path, std::string *error)
    {
        auto source   = std::shared_ptr<PackSource>(new PackSource());
        source->name_ = path;

        const int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC);
        if (fd < 0)
        {
            pack::setError(error, "cannot open asset pack: " + path);
            return nullptr;
        }

        struct stat status{};
        if (::fstat(fd, &status) != 0 || status.st_size <= 0)
        {
            ::close(fd);
            pack::setError(error, "cannot stat asset pack: " + path);
            return nullptr;
        }
        source->size_ = static_cast<std::size_t>(status.st_size);

        void *mapping = ::mmap(nullptr, source->size_, PROT_READ, MAP_PRIVATE, fd, 0);
        ::close(fd);

        if (mapping != MAP_FAILED)
        {
            source->data_   = static_cast<const std::uint8_t *>(mapping);
            source->mapped_ = true;
        } else
        {
            std::ifstream input(path, std::ios::binary | std::ios::ate);
            if (!input)
            {
                pack::setError(error, "cannot read asset pack: " + path);
                return nullptr;
            }
            const std::streamoff length = input.tellg();
            if (length <= 0)
            {
                pack::setError(error, "cannot read asset pack: " + path);
                return nullptr;
            }
            input.seekg(0, std::ios::beg);
            source->owned_.resize(static_cast<std::size_t>(length));
            if (!input.read(reinterpret_cast<char *>(source->owned_.data()), length))
            {
                pack::setError(error, "cannot read asset pack: " + path);
                return nullptr;
            }
            source->data_ = source->owned_.data();
            source->size_ = source->owned_.size();
        }

        if (!source->parse(error))
        {
            return nullptr;
        }
        return source;
    }

    static std::shared_ptr<PackSource> fromBytes(std::vector<std::uint8_t> bytes, std::string name, std::string *error)
    {
        auto source    = std::shared_ptr<PackSource>(new PackSource());
        source->name_  = std::move(name);
        source->owned_ = std::move(bytes);
        source->data_  = source->owned_.data();
        source->size_  = source->owned_.size();
        if (!source->parse(error))
        {
            return nullptr;
        }
        return source;
    }

    ~PackSource() override
    {
        if (mapped_ && data_ != nullptr)
        {
            ::munmap(const_cast<std::uint8_t *>(data_), size_);
        }
    }

    SourceResult open(const std::string &path, bool verifyCrc, const std::uint8_t *&data, std::size_t &size, std::shared_ptr<const void> &owner, std::string *error) const override
    {
        const auto it = index_.find(path);
        if (it == index_.end())
        {
            return SourceResult::NotFound;
        }

        const pack::Entry *entry = it->second;
        if (entry->offset > size_ || entry->size > size_ - entry->offset)
        {
            pack::setError(error, "asset entry is out of bounds: " + path);
            return SourceResult::Error;
        }
        if (verifyCrc && pack::crc32(data_ + entry->offset, static_cast<std::size_t>(entry->size)) != entry->crc32)
        {
            pack::setError(error, "asset entry CRC mismatch: " + path);
            return SourceResult::Error;
        }

        data = data_ + entry->offset;
        size = static_cast<std::size_t>(entry->size);
        return SourceResult::Ok;
    }

    bool exists(const std::string &path) const override
    {
        return index_.find(path) != index_.end();
    }

    std::vector<std::string> list(const std::string &prefix) const override
    {
        std::vector<std::string> result;
        result.reserve(entries_.size());
        for (const auto &entry : entries_)
        {
            if (prefix.empty() || entry.path.compare(0, prefix.size(), prefix) == 0)
            {
                result.push_back(entry.path);
            }
        }
        return result;
    }

    std::string name() const override
    {
        return name_;
    }

private:
    bool parse(std::string *error)
    {
        if (!pack::parseHeader(data_, size_, header_, error))
        {
            return false;
        }
        if (!pack::parseIndex(data_, size_, header_, entries_, error))
        {
            return false;
        }
        index_.reserve(entries_.size());
        for (const auto &entry : entries_)
        {
            index_.emplace(entry.path, &entry);
        }
        return true;
    }

    std::string                                          name_;
    const std::uint8_t                                  *data_   = nullptr;
    std::size_t                                          size_   = 0;
    bool                                                 mapped_ = false;
    std::vector<std::uint8_t>                            owned_;
    pack::Header                                         header_{};
    std::vector<pack::Entry>                             entries_;
    std::unordered_map<std::string, const pack::Entry *> index_;
};

class DirectorySource final : public AssetSource
{
public:
    static std::shared_ptr<DirectorySource> create(const std::string &rootPath, std::string *error)
    {
        std::error_code ec;
        const fs::path  root = fs::weakly_canonical(fs::absolute(rootPath), ec);
        if (ec || !fs::is_directory(root, ec))
        {
            pack::setError(error, "asset directory does not exist: " + rootPath);
            return nullptr;
        }
        return std::shared_ptr<DirectorySource>(new DirectorySource(root));
    }

    SourceResult open(const std::string &path, bool, const std::uint8_t *&data, std::size_t &size, std::shared_ptr<const void> &owner, std::string *error) const override
    {
        fs::path full;
        if (!resolve(path, full))
        {
            pack::setError(error, "asset path escapes the asset directory: " + path);
            return SourceResult::Error;
        }

        std::error_code ec;
        if (!fs::exists(full, ec))
        {
            return SourceResult::NotFound;
        }
        if (!fs::is_regular_file(full, ec))
        {
            pack::setError(error, "asset is not a regular file: " + path);
            return SourceResult::Error;
        }

        std::ifstream input(full, std::ios::binary | std::ios::ate);
        if (!input)
        {
            pack::setError(error, "cannot open asset: " + path);
            return SourceResult::Error;
        }
        const std::streamoff length = input.tellg();
        if (length < 0)
        {
            pack::setError(error, "cannot read asset size: " + path);
            return SourceResult::Error;
        }
        input.seekg(0, std::ios::beg);

        auto buffer = std::make_shared<std::vector<std::uint8_t>>(static_cast<std::size_t>(length));
        if (length > 0 && !input.read(reinterpret_cast<char *>(buffer->data()), length))
        {
            pack::setError(error, "cannot read asset: " + path);
            return SourceResult::Error;
        }

        data  = buffer->data();
        size  = buffer->size();
        owner = std::move(buffer);
        return SourceResult::Ok;
    }

    bool exists(const std::string &path) const override
    {
        fs::path full;
        if (!resolve(path, full))
        {
            return false;
        }
        std::error_code ec;
        return fs::is_regular_file(full, ec);
    }

    std::vector<std::string> list(const std::string &prefix) const override
    {
        std::vector<std::string>               result;
        std::error_code                        ec;
        fs::recursive_directory_iterator       it(root_, fs::directory_options::skip_permission_denied, ec);
        const fs::recursive_directory_iterator end;
        for (; it != end; it.increment(ec))
        {
            if (ec)
            {
                break;
            }
            const fs::file_status status = it->symlink_status(ec);
            if (ec || !fs::is_regular_file(status))
            {
                continue;
            }
            const fs::path relative = fs::relative(it->path(), root_, ec);
            if (ec)
            {
                continue;
            }
            const std::string logical = relative.generic_string();
            if (prefix.empty() || logical.compare(0, prefix.size(), prefix) == 0)
            {
                result.push_back(logical);
            }
        }
        std::sort(result.begin(), result.end());
        return result;
    }

    std::string name() const override
    {
        return root_.string();
    }

    bool realPath(const std::string &path, std::filesystem::path &out) const override
    {
        if (!resolve(path, out))
        {
            return false;
        }
        std::error_code ec;
        return std::filesystem::is_regular_file(out, ec);
    }

private:
    explicit DirectorySource(fs::path root) : root_(std::move(root))
    {}

    bool resolve(const std::string &path, fs::path &out) const
    {
        std::error_code ec;
        const fs::path  full = fs::weakly_canonical(root_ / fs::path(path), ec);
        if (ec)
        {
            return false;
        }
        const fs::path relative = full.lexically_relative(root_);
        if (relative.empty() || relative == ".")
        {
            return false;
        }
        const auto first = relative.begin();
        if (first == relative.end() || *first == "..")
        {
            return false;
        }
        out = full;
        return true;
    }

    fs::path root_;
};

std::string normalizePath(std::string_view path)
{
    if (path.size() >= 2 && path[0] == 'A' && path[1] == ':')
    {
        path.remove_prefix(2);
    }
    while (!path.empty() && path.front() == '/')
    {
        path.remove_prefix(1);
    }
    return std::string(path);
}

} // namespace

struct AssetManager::Impl {
    ~Impl()
    {
        if (!temp_dir_.empty())
        {
            std::error_code ec;
            std::filesystem::remove_all(temp_dir_, ec);
        }
    }

    mutable std::shared_mutex                       mutex;
    std::vector<std::shared_ptr<const AssetSource>> mounts;
    mutable std::mutex                              warn_mutex;
    mutable std::unordered_set<std::string>         warned;
    std::mutex                                      temp_mutex;
    std::filesystem::path                           temp_dir_;
    std::unordered_set<std::string>                 materialized;

    void warnMissing(const std::string &path) const
    {
        std::lock_guard<std::mutex> lock(warn_mutex);
        if (warned.insert(path).second)
        {
            LOG_WARN() << "asset not found: " << path;
        }
    }

    AssetHandle openImpl(const std::string &logicalPath, bool verifyCrc) const
    {
        const std::string path = normalizePath(logicalPath);
        std::string       error;
        if (!pack::isCanonicalPath(path, &error))
        {
            LOG_WARN() << "invalid asset path: " << logicalPath;
            return nullptr;
        }

        std::vector<std::shared_ptr<const AssetSource>> snapshot;
        {
            std::shared_lock<std::shared_mutex> lock(mutex);
            snapshot = mounts;
        }

        for (auto it = snapshot.rbegin(); it != snapshot.rend(); ++it)
        {
            const std::uint8_t         *data = nullptr;
            std::size_t                 size = 0;
            std::shared_ptr<const void> owner;
            std::string                 sourceError;

            const SourceResult result = (*it)->open(path, verifyCrc, data, size, owner, &sourceError);
            if (result == SourceResult::Ok)
            {
                if (!owner)
                {
                    owner = *it;
                }
                return std::make_shared<Asset>(path, data, size, std::move(owner));
            }
            if (result == SourceResult::Error)
            {
                LOG_ERROR() << "asset load failed: " << path << ": " << sourceError;
                return nullptr;
            }
        }

        warnMissing(path);
        return nullptr;
    }
};

AssetManager::AssetManager() : impl_(std::make_unique<Impl>())
{}

AssetManager::~AssetManager() = default;

AssetManager &AssetManager::getInstance()
{
    static AssetManager instance;
    return instance;
}

bool AssetManager::mountPack(const std::string &packPath, std::string *error)
{
    auto source = PackSource::fromFile(packPath, error);
    if (!source)
    {
        LOG_ERROR() << "failed to mount asset pack: " << packPath;
        return false;
    }

    {
        std::unique_lock<std::shared_mutex> lock(impl_->mutex);
        impl_->mounts.push_back(std::move(source));
    }
    LOG_DEBUG() << "mounted asset pack: " << packPath;
    return true;
}

bool AssetManager::mountDirectory(const std::string &root, std::string *error)
{
    auto source = DirectorySource::create(root, error);
    if (!source)
    {
        LOG_ERROR() << "failed to mount asset directory: " << root;
        return false;
    }

    {
        std::unique_lock<std::shared_mutex> lock(impl_->mutex);
        impl_->mounts.push_back(std::move(source));
    }
    LOG_DEBUG() << "mounted asset directory: " << root;
    return true;
}

bool AssetManager::mountMemory(const void *packBytes, std::size_t size, std::string *error)
{
    if (packBytes == nullptr || size == 0)
    {
        pack::setError(error, "empty asset pack");
        return false;
    }

    const auto               *bytes = static_cast<const std::uint8_t *>(packBytes);
    std::vector<std::uint8_t> copy(bytes, bytes + size);

    auto source = PackSource::fromBytes(std::move(copy), "<memory>", error);
    if (!source)
    {
        LOG_ERROR() << "failed to mount in-memory asset pack";
        return false;
    }

    {
        std::unique_lock<std::shared_mutex> lock(impl_->mutex);
        impl_->mounts.push_back(std::move(source));
    }
    LOG_DEBUG() << "mounted in-memory asset pack (" << size << " bytes)";
    return true;
}

void AssetManager::reset()
{
    {
        std::unique_lock<std::shared_mutex> lock(impl_->mutex);
        impl_->mounts.clear();
    }
    {
        std::lock_guard<std::mutex> lock(impl_->warn_mutex);
        impl_->warned.clear();
    }
    {
        std::lock_guard<std::mutex> lock(impl_->temp_mutex);
        impl_->materialized.clear();
    }
    LOG_DEBUG() << "asset mounts cleared";
}

AssetHandle AssetManager::open(const std::string &logicalPath) const
{
    return impl_->openImpl(logicalPath, true);
}

AssetHandle AssetManager::openUnchecked(const std::string &logicalPath) const
{
    return impl_->openImpl(logicalPath, false);
}

bool AssetManager::exists(const std::string &logicalPath) const
{
    const std::string path = normalizePath(logicalPath);
    if (!pack::isCanonicalPath(path))
    {
        return false;
    }

    std::vector<std::shared_ptr<const AssetSource>> snapshot;
    {
        std::shared_lock<std::shared_mutex> lock(impl_->mutex);
        snapshot = impl_->mounts;
    }
    for (auto it = snapshot.rbegin(); it != snapshot.rend(); ++it)
    {
        if ((*it)->exists(path))
        {
            return true;
        }
    }
    return false;
}

std::vector<std::string> AssetManager::list(const std::string &prefix) const
{
    std::vector<std::shared_ptr<const AssetSource>> sources;
    {
        std::shared_lock<std::shared_mutex> lock(impl_->mutex);
        sources = impl_->mounts;
    }

    std::vector<std::string>        result;
    std::unordered_set<std::string> seen;
    for (auto it = sources.rbegin(); it != sources.rend(); ++it)
    {
        for (auto &path : (*it)->list(prefix))
        {
            if (seen.insert(path).second)
            {
                result.push_back(std::move(path));
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool AssetManager::materialize(const std::string &logicalPath, std::string &outPath)
{
    const std::string path = normalizePath(logicalPath);
    if (!pack::isCanonicalPath(path))
    {
        return false;
    }

    std::vector<std::shared_ptr<const AssetSource>> snapshot;
    {
        std::shared_lock<std::shared_mutex> lock(impl_->mutex);
        snapshot = impl_->mounts;
    }

    for (auto it = snapshot.rbegin(); it != snapshot.rend(); ++it)
    {
        std::filesystem::path real;
        if ((*it)->realPath(path, real))
        {
            outPath = real.string();
            return true;
        }
    }

    AssetHandle asset = open(path);
    if (!asset)
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(impl_->temp_mutex);
    std::error_code             ec;
    if (impl_->temp_dir_.empty())
    {
        impl_->temp_dir_ = std::filesystem::temp_directory_path(ec) / ("lvglpp-assets-" + std::to_string(::getpid()));
        std::filesystem::create_directories(impl_->temp_dir_, ec);
        if (ec)
        {
            impl_->temp_dir_.clear();
            LOG_ERROR() << "cannot create asset temp directory: " << ec.message();
            return false;
        }
    }

    const std::size_t slash = path.find_last_of('/');
    const std::size_t dot   = path.find_last_of('.');
    std::string       extension;
    if (dot != std::string::npos && (slash == std::string::npos || dot > slash))
    {
        for (std::size_t i = dot; i < path.size() && i - dot <= 8; ++i)
        {
            const char c = path[i];
            if (std::isalnum(static_cast<unsigned char>(c)) || c == '.')
            {
                extension.push_back(c);
            }
        }
    }

    const std::filesystem::path target = impl_->temp_dir_ / (std::to_string(std::hash<std::string>{}(path)) + extension);

    if (impl_->materialized.find(path) == impl_->materialized.end())
    {
        std::ofstream output(target, std::ios::binary | std::ios::trunc);
        if (!output || !output.write(reinterpret_cast<const char *>(asset->data()), static_cast<std::streamsize>(asset->size())))
        {
            LOG_ERROR() << "cannot materialize asset: " << path;
            return false;
        }
        impl_->materialized.insert(path);
    }

    outPath = target.string();
    return true;
}

bool AssetManager::mountDefaults(const std::string &baseDir, const std::string &exeName, std::string *error)
{
    std::error_code ec;
    const fs::path  base = baseDir.empty() ? fs::current_path(ec) : fs::path(baseDir);
    if (ec)
    {
        pack::setError(error, "cannot resolve the asset base directory");
        return false;
    }

    if (const char *directory = std::getenv("LVGLPP_ASSET_DIR"); directory != nullptr && *directory != '\0')
    {
        return mountDirectory(directory, error);
    }

    const std::size_t before = mountCount();

    if (const char *packList = std::getenv("LVGLPP_ASSET_PACK"); packList != nullptr && *packList != '\0')
    {
        const std::string value(packList);
        std::size_t       start = 0;
        while (start <= value.size())
        {
            const std::size_t end   = value.find(':', start);
            const std::size_t stop  = (end == std::string::npos) ? value.size() : end;
            const std::string entry = value.substr(start, stop - start);
            if (!entry.empty())
            {
                std::string mountError;
                if (!mountPack(entry, &mountError))
                {
                    LOG_ERROR() << "asset pack mount failed: " << mountError;
                }
            }
            if (end == std::string::npos)
            {
                break;
            }
            start = end + 1;
        }
        return mountCount() > before;
    }

    auto mountCommon = [&]() {
        const fs::path pack = base / "common.pak";
        if (fs::is_regular_file(pack, ec))
        {
            std::string mountError;
            if (!mountPack(pack.string(), &mountError))
            {
                LOG_ERROR() << "common asset pack mount failed: " << mountError;
            }
            return;
        }
        const fs::path directory = base / "common.asset";
        if (fs::is_directory(directory, ec))
        {
            std::string mountError;
            if (!mountDirectory(directory.string(), &mountError))
            {
                LOG_ERROR() << "common asset directory mount failed: " << mountError;
            }
        }
    };

    auto mountApp = [&]() {
        if (exeName.empty())
        {
            return;
        }
        const fs::path pack = base / (exeName + ".pak");
        if (fs::is_regular_file(pack, ec))
        {
            std::string mountError;
            if (!mountPack(pack.string(), &mountError))
            {
                LOG_ERROR() << "application asset pack mount failed: " << mountError;
            }
            return;
        }
        const fs::path directory = base / (exeName + ".asset");
        if (fs::is_directory(directory, ec))
        {
            std::string mountError;
            if (!mountDirectory(directory.string(), &mountError))
            {
                LOG_ERROR() << "application asset directory mount failed: " << mountError;
            }
        }
    };

    mountCommon();
    mountApp();

    if (mountCount() == before)
    {
        const fs::path legacy = base / "asset";
        if (fs::is_directory(legacy, ec))
        {
            LOG_WARN() << "using legacy asset directory (no pack found): " << legacy.string();
            std::string mountError;
            if (!mountDirectory(legacy.string(), &mountError))
            {
                LOG_ERROR() << "legacy asset directory mount failed: " << mountError;
            }
        }
    }

    return mountCount() > before;
}

std::size_t AssetManager::mountCount() const
{
    std::shared_lock<std::shared_mutex> lock(impl_->mutex);
    return impl_->mounts.size();
}

std::vector<std::string> AssetManager::mountNames() const
{
    std::shared_lock<std::shared_mutex> lock(impl_->mutex);
    std::vector<std::string>            names;
    names.reserve(impl_->mounts.size());
    for (const auto &source : impl_->mounts)
    {
        names.push_back(source->name());
    }
    return names;
}

} // namespace lvglpp::tools
