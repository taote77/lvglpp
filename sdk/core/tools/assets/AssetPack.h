#ifndef LVGLPP_ASSETPACK_H
#define LVGLPP_ASSETPACK_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_set>
#include <vector>

/**
 * LVPAK1 asset pack format.
 *
 * The format is a dependency-free, deterministic container for the files an
 * application ships under its asset root. It stores file bytes verbatim (no
 * base64, no compression in v1); images/fonts are already compressed.
 *
 * Header (40 bytes, little-endian):
 *   u8  magic[6]       "LVPAK1"
 *   u16 version        1
 *   u16 header_size    40
 *   u16 flags          0
 *   u32 entry_count
 *   u64 index_offset
 *   u64 index_size
 *   u32 index_crc32    CRC-32/IEEE over the index blob
 *   u32 reserved       0
 *
 * Index (at index_offset, entries strictly sorted by path):
 *   u16 path_len | u16 flags | u64 data_offset | u64 data_size |
 *   u32 data_crc32 | u8 path[path_len]
 *
 * Paths are relative, '/'-separated, UTF-8, without a leading slash and
 * without '.'/'..' segments. This header is shared by the runtime
 * AssetManager, the respack CLI and the unit tests; it has no LVGL dependency.
 */
namespace lvglpp::tools::pack {

inline constexpr std::size_t   kHeaderSize     = 40;
inline constexpr std::uint16_t kVersion        = 1;
inline constexpr std::size_t   kMaxPathLength  = 4096;
inline constexpr std::uint32_t kMaxEntryCount  = 1000000;
inline constexpr std::size_t   kEntryFixedSize = 24;
inline constexpr std::uint8_t  kMagic[6]       = {'L', 'V', 'P', 'A', 'K', '1'};

struct Header {
    std::uint16_t version      = kVersion;
    std::uint16_t header_size  = static_cast<std::uint16_t>(kHeaderSize);
    std::uint16_t flags        = 0;
    std::uint32_t entry_count  = 0;
    std::uint64_t index_offset = kHeaderSize;
    std::uint64_t index_size   = 0;
    std::uint32_t index_crc32  = 0;
    std::uint32_t reserved     = 0;
};

struct Entry {
    std::string   path;
    std::uint16_t flags  = 0;
    std::uint64_t offset = 0;
    std::uint64_t size   = 0;
    std::uint32_t crc32  = 0;
};

// ---------------------------------------------------------------- endian I/O

inline std::uint16_t readLE16(const std::uint8_t *p)
{
    return static_cast<std::uint16_t>(p[0]) | static_cast<std::uint16_t>(static_cast<std::uint16_t>(p[1]) << 8);
}

inline std::uint32_t readLE32(const std::uint8_t *p)
{
    return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) | (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
}

inline std::uint64_t readLE64(const std::uint8_t *p)
{
    return static_cast<std::uint64_t>(readLE32(p)) | (static_cast<std::uint64_t>(readLE32(p + 4)) << 32);
}

inline void writeLE16(std::uint8_t *p, std::uint16_t v)
{
    p[0] = static_cast<std::uint8_t>(v & 0xFFu);
    p[1] = static_cast<std::uint8_t>((v >> 8) & 0xFFu);
}

inline void writeLE32(std::uint8_t *p, std::uint32_t v)
{
    p[0] = static_cast<std::uint8_t>(v & 0xFFu);
    p[1] = static_cast<std::uint8_t>((v >> 8) & 0xFFu);
    p[2] = static_cast<std::uint8_t>((v >> 16) & 0xFFu);
    p[3] = static_cast<std::uint8_t>((v >> 24) & 0xFFu);
}

inline void writeLE64(std::uint8_t *p, std::uint64_t v)
{
    writeLE32(p, static_cast<std::uint32_t>(v & 0xFFFFFFFFull));
    writeLE32(p + 4, static_cast<std::uint32_t>((v >> 32) & 0xFFFFFFFFull));
}

inline void appendLE16(std::vector<std::uint8_t> &out, std::uint16_t v)
{
    out.push_back(static_cast<std::uint8_t>(v & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
}

inline void appendLE32(std::vector<std::uint8_t> &out, std::uint32_t v)
{
    out.push_back(static_cast<std::uint8_t>(v & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xFFu));
}

inline void appendLE64(std::vector<std::uint8_t> &out, std::uint64_t v)
{
    appendLE32(out, static_cast<std::uint32_t>(v & 0xFFFFFFFFull));
    appendLE32(out, static_cast<std::uint32_t>((v >> 32) & 0xFFFFFFFFull));
}

// ---------------------------------------------------------------- CRC-32/IEEE

constexpr std::array<std::uint32_t, 256> makeCrcTable()
{
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t i = 0; i < 256; ++i)
    {
        std::uint32_t value = i;
        for (int bit = 0; bit < 8; ++bit)
        {
            value = (value & 1u) ? (0xEDB88320u ^ (value >> 1)) : (value >> 1);
        }
        table[i] = value;
    }
    return table;
}

inline constexpr std::array<std::uint32_t, 256> kCrcTable = makeCrcTable();

inline std::uint32_t crc32Start()
{
    return 0xFFFFFFFFu;
}

inline std::uint32_t crc32Update(std::uint32_t crc, const void *data, std::size_t size)
{
    const auto *bytes = static_cast<const std::uint8_t *>(data);
    for (std::size_t i = 0; i < size; ++i)
    {
        crc = kCrcTable[(crc ^ bytes[i]) & 0xFFu] ^ (crc >> 8);
    }
    return crc;
}

inline std::uint32_t crc32Finish(std::uint32_t crc)
{
    return crc ^ 0xFFFFFFFFu;
}

inline std::uint32_t crc32(const void *data, std::size_t size)
{
    return crc32Finish(crc32Update(crc32Start(), data, size));
}

// ---------------------------------------------------------------- path rules

inline void setError(std::string *error, std::string message)
{
    if (error != nullptr)
    {
        *error = std::move(message);
    }
}

inline bool isCanonicalPath(std::string_view path, std::string *error = nullptr)
{
    if (path.empty())
    {
        setError(error, "empty asset path");
        return false;
    }
    if (path.size() > kMaxPathLength)
    {
        setError(error, "asset path too long");
        return false;
    }
    if (path.front() == '/')
    {
        setError(error, "asset path must be relative: " + std::string(path));
        return false;
    }
    if (path.back() == '/')
    {
        setError(error, "asset path must not end with '/': " + std::string(path));
        return false;
    }
    for (char c : path)
    {
        if (c == '\0' || c == '\\')
        {
            setError(error, "asset path contains an invalid character");
            return false;
        }
    }

    std::size_t start = 0;
    while (start <= path.size())
    {
        const std::size_t      end     = path.find('/', start);
        const std::size_t      stop    = (end == std::string_view::npos) ? path.size() : end;
        const std::string_view segment = path.substr(start, stop - start);
        if (segment.empty() || segment == "." || segment == "..")
        {
            setError(error, "asset path contains an invalid segment: " + std::string(path));
            return false;
        }
        if (end == std::string_view::npos)
        {
            break;
        }
        start = end + 1;
    }
    return true;
}

inline bool globMatch(std::string_view pattern, std::string_view text)
{
    std::size_t p    = 0;
    std::size_t t    = 0;
    std::size_t star = std::string_view::npos;
    std::size_t mark = 0;

    while (t < text.size())
    {
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == text[t]))
        {
            ++p;
            ++t;
        } else if (p < pattern.size() && pattern[p] == '*')
        {
            star = p++;
            mark = t;
        } else if (star != std::string_view::npos)
        {
            p = star + 1;
            t = ++mark;
        } else
        {
            return false;
        }
    }
    while (p < pattern.size() && pattern[p] == '*')
    {
        ++p;
    }
    return p == pattern.size();
}

inline std::vector<std::string> defaultExcludes()
{
    return {"*.db", "*.pak", ".*"};
}

inline bool isExcluded(const std::string &relativePath, const std::vector<std::string> &patterns)
{
    const std::size_t slash = relativePath.find_last_of('/');
    const std::string name  = (slash == std::string::npos) ? relativePath : relativePath.substr(slash + 1);

    bool        hasHiddenComponent = false;
    std::size_t start              = 0;
    while (start <= relativePath.size())
    {
        const std::size_t      end     = relativePath.find('/', start);
        const std::size_t      stop    = (end == std::string::npos) ? relativePath.size() : end;
        const std::string_view segment = std::string_view(relativePath).substr(start, stop - start);
        if (!segment.empty() && segment.front() == '.')
        {
            hasHiddenComponent = true;
            break;
        }
        if (end == std::string::npos)
        {
            break;
        }
        start = end + 1;
    }

    for (const auto &pattern : patterns)
    {
        if (pattern == ".*")
        {
            if (hasHiddenComponent)
            {
                return true;
            }
            continue;
        }
        if (globMatch(pattern, relativePath) || globMatch(pattern, name))
        {
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------------------- parsing

inline bool parseHeader(const std::uint8_t *file, std::size_t size, Header &out, std::string *error = nullptr)
{
    if (file == nullptr || size < kHeaderSize)
    {
        setError(error, "asset pack is truncated (header)");
        return false;
    }
    if (std::memcmp(file, kMagic, sizeof(kMagic)) != 0)
    {
        setError(error, "asset pack has a bad magic");
        return false;
    }

    out.version      = readLE16(file + 6);
    out.header_size  = readLE16(file + 8);
    out.flags        = readLE16(file + 10);
    out.entry_count  = readLE32(file + 12);
    out.index_offset = readLE64(file + 16);
    out.index_size   = readLE64(file + 24);
    out.index_crc32  = readLE32(file + 32);
    out.reserved     = readLE32(file + 36);

    if (out.version != kVersion)
    {
        setError(error, "unsupported asset pack version");
        return false;
    }
    if (out.header_size != kHeaderSize)
    {
        setError(error, "unsupported asset pack header size");
        return false;
    }
    if (out.flags != 0 || out.reserved != 0)
    {
        setError(error, "unsupported asset pack flags");
        return false;
    }
    if (out.index_offset < kHeaderSize || out.index_offset > size)
    {
        setError(error, "asset pack index offset is invalid");
        return false;
    }
    if (out.index_size != size - out.index_offset)
    {
        setError(error, "asset pack index size does not match the file size");
        return false;
    }
    if (out.entry_count > kMaxEntryCount)
    {
        setError(error, "asset pack has too many entries");
        return false;
    }
    if (out.index_size / kEntryFixedSize < out.entry_count)
    {
        setError(error, "asset pack index is too small for its entry count");
        return false;
    }
    if (out.index_size == 0)
    {
        if (out.index_crc32 != 0)
        {
            setError(error, "empty asset pack has a non-zero index CRC");
            return false;
        }
    } else if (crc32(file + out.index_offset, static_cast<std::size_t>(out.index_size)) != out.index_crc32)
    {
        setError(error, "asset pack index CRC mismatch");
        return false;
    }
    return true;
}

inline bool parseIndex(const std::uint8_t *file, std::size_t size, const Header &header, std::vector<Entry> &out, std::string *error = nullptr)
{
    if (file == nullptr || header.index_offset > size || header.index_size > size - header.index_offset)
    {
        setError(error, "asset pack index is out of bounds");
        return false;
    }

    const std::uint8_t *index = file + header.index_offset;
    std::size_t         pos   = 0;
    out.clear();
    out.reserve(header.entry_count);

    std::string previous;
    for (std::uint32_t i = 0; i < header.entry_count; ++i)
    {
        if (header.index_size - pos < kEntryFixedSize)
        {
            setError(error, "asset pack index entry is truncated");
            return false;
        }

        const std::uint16_t path_len  = readLE16(index + pos);
        const std::uint16_t flags     = readLE16(index + pos + 2);
        const std::uint64_t offset    = readLE64(index + pos + 4);
        const std::uint64_t data_size = readLE64(index + pos + 12);
        const std::uint32_t data_crc  = readLE32(index + pos + 20);

        if (flags != 0)
        {
            setError(error, "asset pack entry has unsupported flags");
            return false;
        }
        if (path_len == 0 || path_len > kMaxPathLength)
        {
            setError(error, "asset pack entry has an invalid path length");
            return false;
        }
        if (header.index_size - pos - kEntryFixedSize < path_len)
        {
            setError(error, "asset pack index path is truncated");
            return false;
        }

        Entry entry;
        entry.path   = std::string(reinterpret_cast<const char *>(index + pos + kEntryFixedSize), path_len);
        entry.flags  = flags;
        entry.offset = offset;
        entry.size   = data_size;
        entry.crc32  = data_crc;

        if (!isCanonicalPath(entry.path, error))
        {
            return false;
        }
        if (i > 0 && entry.path <= previous)
        {
            setError(error, "asset pack index is not strictly sorted: " + entry.path);
            return false;
        }
        if (entry.offset < kHeaderSize || entry.offset > header.index_offset || entry.size > header.index_offset - entry.offset)
        {
            setError(error, "asset pack entry is out of bounds: " + entry.path);
            return false;
        }

        previous = entry.path;
        out.push_back(std::move(entry));
        pos += kEntryFixedSize + path_len;
    }

    if (pos != header.index_size)
    {
        setError(error, "asset pack index has trailing data");
        return false;
    }
    return true;
}

inline bool verifyEntryData(const std::uint8_t *file, std::size_t size, const Entry &entry, std::string *error = nullptr)
{
    if (file == nullptr || entry.offset > size || entry.size > size - entry.offset)
    {
        setError(error, "asset pack entry is out of bounds: " + entry.path);
        return false;
    }
    if (crc32(file + entry.offset, static_cast<std::size_t>(entry.size)) != entry.crc32)
    {
        setError(error, "asset pack entry CRC mismatch: " + entry.path);
        return false;
    }
    return true;
}

// ---------------------------------------------------------------- writer

class Writer
{
public:
    bool addBytes(std::string path, const void *data, std::size_t size, std::string *error = nullptr)
    {
        if (!isCanonicalPath(path, error))
        {
            return false;
        }
        if (data == nullptr && size != 0)
        {
            setError(error, "null asset data: " + path);
            return false;
        }
        if (!paths_.insert(path).second)
        {
            setError(error, "duplicate asset path: " + path);
            return false;
        }

        Item item;
        item.path = std::move(path);
        if (size != 0)
        {
            const auto *bytes = static_cast<const std::uint8_t *>(data);
            item.data.assign(bytes, bytes + size);
        }
        item.crc32 = crc32(item.data.data(), item.data.size());
        items_.push_back(std::move(item));
        return true;
    }

    bool addDirectory(const std::string &root, const std::vector<std::string> &excludes = defaultExcludes(), std::string *error = nullptr)
    {
        namespace fs = std::filesystem;

        std::error_code ec;
        if (!fs::is_directory(root, ec) || ec)
        {
            setError(error, "asset directory does not exist: " + root);
            return false;
        }

        std::vector<fs::path>            files;
        fs::recursive_directory_iterator it(root, fs::directory_options::skip_permission_denied, ec);
        fs::recursive_directory_iterator end;
        if (ec)
        {
            setError(error, "cannot scan asset directory: " + ec.message());
            return false;
        }

        for (; it != end; it.increment(ec))
        {
            if (ec)
            {
                setError(error, "cannot scan asset directory: " + ec.message());
                return false;
            }
            const fs::file_status status = it->symlink_status(ec);
            if (ec)
            {
                setError(error, "cannot stat asset: " + it->path().string());
                return false;
            }
            if (fs::is_symlink(status) || !fs::is_regular_file(status))
            {
                continue;
            }

            const fs::path relative = fs::relative(it->path(), root, ec);
            if (ec || relative.empty())
            {
                setError(error, "cannot resolve asset path: " + it->path().string());
                return false;
            }
            const std::string logical = relative.generic_string();
            if (isExcluded(logical, excludes))
            {
                continue;
            }
            files.push_back(it->path());
        }

        std::sort(files.begin(), files.end(),
                  [&root](const fs::path &a, const fs::path &b) { return fs::relative(a, root).generic_string() < fs::relative(b, root).generic_string(); });

        for (const auto &file : files)
        {
            std::ifstream input(file, std::ios::binary | std::ios::ate);
            if (!input)
            {
                setError(error, "cannot open asset: " + file.string());
                return false;
            }
            const std::streamoff length = input.tellg();
            if (length < 0)
            {
                setError(error, "cannot read asset size: " + file.string());
                return false;
            }
            input.seekg(0, std::ios::beg);

            std::vector<std::uint8_t> bytes(static_cast<std::size_t>(length));
            if (length > 0 && !input.read(reinterpret_cast<char *>(bytes.data()), length))
            {
                setError(error, "cannot read asset: " + file.string());
                return false;
            }

            const std::string logical = fs::relative(file, root).generic_string();
            if (!addBytes(logical, bytes.data(), bytes.size(), error))
            {
                return false;
            }
        }
        return true;
    }

    bool writeToMemory(std::vector<std::uint8_t> &out, std::string *error = nullptr) const
    {
        if (items_.size() > kMaxEntryCount)
        {
            setError(error, "too many asset entries");
            return false;
        }

        std::vector<const Item *> sorted;
        sorted.reserve(items_.size());
        for (const auto &item : items_)
        {
            sorted.push_back(&item);
        }
        std::sort(sorted.begin(), sorted.end(), [](const Item *a, const Item *b) { return a->path < b->path; });

        std::uint64_t data_offset = kHeaderSize;
        for (const Item *item : sorted)
        {
            data_offset = alignUp(data_offset, 8);
            data_offset += item->data.size();
        }
        const std::uint64_t index_offset = alignUp(data_offset, 8);

        std::vector<std::uint8_t> index;
        index.reserve(sorted.size() * kEntryFixedSize);
        std::uint64_t offset = kHeaderSize;
        for (const Item *item : sorted)
        {
            offset = alignUp(offset, 8);
            if (item->path.size() > std::numeric_limits<std::uint16_t>::max())
            {
                setError(error, "asset path is too long: " + item->path);
                return false;
            }
            appendLE16(index, static_cast<std::uint16_t>(item->path.size()));
            appendLE16(index, 0);
            appendLE64(index, offset);
            appendLE64(index, item->data.size());
            appendLE32(index, item->crc32);
            index.insert(index.end(), item->path.begin(), item->path.end());
            offset += item->data.size();
        }

        const std::uint32_t index_crc = crc32(index.data(), index.size());
        if (index_offset > std::numeric_limits<std::size_t>::max() - index.size())
        {
            setError(error, "asset pack is too large");
            return false;
        }

        out.assign(static_cast<std::size_t>(index_offset) + index.size(), 0);

        std::memcpy(out.data(), kMagic, sizeof(kMagic));
        writeLE16(out.data() + 6, kVersion);
        writeLE16(out.data() + 8, static_cast<std::uint16_t>(kHeaderSize));
        writeLE16(out.data() + 10, 0);
        writeLE32(out.data() + 12, static_cast<std::uint32_t>(sorted.size()));
        writeLE64(out.data() + 16, index_offset);
        writeLE64(out.data() + 24, index.size());
        writeLE32(out.data() + 32, index_crc);
        writeLE32(out.data() + 36, 0);

        offset = kHeaderSize;
        for (const Item *item : sorted)
        {
            offset = alignUp(offset, 8);
            if (!item->data.empty())
            {
                std::memcpy(out.data() + offset, item->data.data(), item->data.size());
            }
            offset += item->data.size();
        }
        if (!index.empty())
        {
            std::memcpy(out.data() + index_offset, index.data(), index.size());
        }
        return true;
    }

    bool writeFile(const std::string &path, std::string *error = nullptr) const
    {
        std::vector<std::uint8_t> bytes;
        if (!writeToMemory(bytes, error))
        {
            return false;
        }

        const std::string temporary = path + ".tmp";
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output)
            {
                setError(error, "cannot open output pack: " + temporary);
                return false;
            }
            if (!bytes.empty())
            {
                output.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
            }
            if (!output)
            {
                setError(error, "cannot write output pack: " + temporary);
                return false;
            }
        }

        std::error_code ec;
        std::filesystem::rename(temporary, path, ec);
        if (ec)
        {
            std::filesystem::remove(temporary, ec);
            setError(error, "cannot finalize output pack: " + path);
            return false;
        }
        return true;
    }

    std::size_t entryCount() const
    {
        return items_.size();
    }

private:
    struct Item {
        std::string               path;
        std::vector<std::uint8_t> data;
        std::uint32_t             crc32 = 0;
    };

    static std::uint64_t alignUp(std::uint64_t value, std::uint64_t alignment)
    {
        const std::uint64_t remainder = value % alignment;
        return remainder == 0 ? value : value + (alignment - remainder);
    }

    std::vector<Item>               items_;
    std::unordered_set<std::string> paths_;
};

// ---------------------------------------------------------------- reader

class Reader
{
public:
    bool openMemory(const void *data, std::size_t size, std::string *error = nullptr)
    {
        owned_.clear();
        data_ = static_cast<const std::uint8_t *>(data);
        size_ = size;
        return parse(error);
    }

    bool openFile(const std::string &path, std::string *error = nullptr)
    {
        std::ifstream input(path, std::ios::binary | std::ios::ate);
        if (!input)
        {
            setError(error, "cannot open asset pack: " + path);
            return false;
        }
        const std::streamoff length = input.tellg();
        if (length < 0)
        {
            setError(error, "cannot read asset pack size: " + path);
            return false;
        }
        input.seekg(0, std::ios::beg);

        owned_.resize(static_cast<std::size_t>(length));
        if (length > 0 && !input.read(reinterpret_cast<char *>(owned_.data()), length))
        {
            setError(error, "cannot read asset pack: " + path);
            return false;
        }
        data_ = owned_.data();
        size_ = owned_.size();
        return parse(error);
    }

    const Header &header() const
    {
        return header_;
    }

    const std::vector<Entry> &entries() const
    {
        return entries_;
    }

    const Entry *find(const std::string &path) const
    {
        const auto it = std::lower_bound(entries_.begin(), entries_.end(), path, [](const Entry &entry, const std::string &value) { return entry.path < value; });
        if (it == entries_.end() || it->path != path)
        {
            return nullptr;
        }
        return &*it;
    }

    bool read(const Entry &entry, std::vector<std::uint8_t> &out, std::string *error = nullptr) const
    {
        if (data_ == nullptr || entry.offset > size_ || entry.size > size_ - entry.offset)
        {
            setError(error, "asset pack entry is out of bounds: " + entry.path);
            return false;
        }
        if (crc32(data_ + entry.offset, static_cast<std::size_t>(entry.size)) != entry.crc32)
        {
            setError(error, "asset pack entry CRC mismatch: " + entry.path);
            return false;
        }
        out.assign(data_ + entry.offset, data_ + entry.offset + entry.size);
        return true;
    }

    bool verifyAll(std::string *error = nullptr) const
    {
        for (const auto &entry : entries_)
        {
            if (!verifyEntryData(data_, size_, entry, error))
            {
                return false;
            }
        }
        return true;
    }

    const std::uint8_t *data() const
    {
        return data_;
    }

    std::size_t size() const
    {
        return size_;
    }

private:
    bool parse(std::string *error)
    {
        entries_.clear();
        if (!parseHeader(data_, size_, header_, error))
        {
            return false;
        }
        return parseIndex(data_, size_, header_, entries_, error);
    }

    std::vector<std::uint8_t> owned_;
    const std::uint8_t       *data_ = nullptr;
    std::size_t               size_ = 0;
    Header                    header_{};
    std::vector<Entry>        entries_;
};

} // namespace lvglpp::tools::pack

#endif // LVGLPP_ASSETPACK_H
