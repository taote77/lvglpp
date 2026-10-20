#include "TestFramework.h"
#include "core/tools/assets/AssetPack.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace lvglpp::test;
namespace pack = lvglpp::tools::pack;

namespace {

std::vector<std::uint8_t> makePack()
{
    pack::Writer writer;
    std::string  error;
    writer.addBytes("image/a.png", "abc", 3, &error);
    writer.addBytes("fonts/f.ttf", "hello", 5, &error);
    writer.addBytes("lottie/camera.json", "{}", 2, &error);

    std::vector<std::uint8_t> bytes;
    writer.writeToMemory(bytes, &error);
    return bytes;
}

std::string bytesToString(const std::vector<std::uint8_t> &bytes)
{
    return std::string(bytes.begin(), bytes.end());
}

} // namespace

TEST(AssetPack_roundtrip_is_sorted_and_verifiable)
{
    const auto bytes = makePack();
    CHECK_FALSE(bytes.empty());

    pack::Reader reader;
    std::string  error;
    CHECK_TRUE(reader.openMemory(bytes.data(), bytes.size(), &error));
    CHECK_EQ(reader.entries().size(), 3u);

    // Bytewise sorted paths.
    CHECK_EQ(reader.entries()[0].path, std::string("fonts/f.ttf"));
    CHECK_EQ(reader.entries()[1].path, std::string("image/a.png"));
    CHECK_EQ(reader.entries()[2].path, std::string("lottie/camera.json"));

    const pack::Entry *entry = reader.find("image/a.png");
    CHECK(entry != nullptr);
    CHECK_EQ(entry->size, 3u);

    std::vector<std::uint8_t> data;
    CHECK_TRUE(reader.read(*entry, data, &error));
    CHECK_EQ(bytesToString(data), std::string("abc"));
    CHECK_TRUE(reader.verifyAll(&error));

    const pack::Entry *missing = reader.find("image/missing.png");
    CHECK(missing == nullptr);
    return true;
}

TEST(AssetPack_output_is_deterministic)
{
    const auto first  = makePack();
    const auto second = makePack();
    CHECK_TRUE(first == second);
    return true;
}

TEST(AssetPack_empty_pack_is_valid)
{
    pack::Writer              writer;
    std::vector<std::uint8_t> bytes;
    std::string               error;
    CHECK_TRUE(writer.writeToMemory(bytes, &error));
    CHECK_EQ(bytes.size(), pack::kHeaderSize);

    pack::Reader reader;
    CHECK_TRUE(reader.openMemory(bytes.data(), bytes.size(), &error));
    CHECK_EQ(reader.entries().size(), 0u);
    CHECK_TRUE(reader.verifyAll(&error));
    return true;
}

TEST(AssetPack_rejects_invalid_paths)
{
    pack::Writer writer;
    std::string  error;
    CHECK_FALSE(writer.addBytes("", "x", 1, &error));
    CHECK_FALSE(writer.addBytes("/absolute.png", "x", 1, &error));
    CHECK_FALSE(writer.addBytes("../escape.png", "x", 1, &error));
    CHECK_FALSE(writer.addBytes("a/./b.png", "x", 1, &error));
    CHECK_FALSE(writer.addBytes("a//b.png", "x", 1, &error));
    CHECK_FALSE(writer.addBytes("a\\b.png", "x", 1, &error));
    CHECK_FALSE(writer.addBytes("dir/", "x", 1, &error));
    return true;
}

TEST(AssetPack_rejects_duplicate_paths)
{
    pack::Writer writer;
    std::string  error;
    CHECK_TRUE(writer.addBytes("image/a.png", "a", 1, &error));
    CHECK_FALSE(writer.addBytes("image/a.png", "b", 1, &error));
    return true;
}

TEST(AssetPack_rejects_bad_magic_and_truncation)
{
    auto        bytes = makePack();
    std::string error;

    pack::Reader reader;
    CHECK_FALSE(reader.openMemory(bytes.data(), pack::kHeaderSize - 1, &error));

    bytes[0] = 'X';
    CHECK_FALSE(reader.openMemory(bytes.data(), bytes.size(), &error));

    bytes = makePack();
    bytes.pop_back();
    CHECK_FALSE(reader.openMemory(bytes.data(), bytes.size(), &error));
    return true;
}

TEST(AssetPack_detects_index_crc_corruption)
{
    auto bytes = makePack();
    // The last byte belongs to the index; flipping it must fail the index CRC.
    bytes.back() ^= 0xFF;

    pack::Reader reader;
    std::string  error;
    CHECK_FALSE(reader.openMemory(bytes.data(), bytes.size(), &error));
    CHECK_TRUE(error.find("index CRC") != std::string::npos);
    return true;
}

TEST(AssetPack_rejects_unsorted_index_even_with_valid_crc)
{
    pack::Writer writer;
    std::string  error;
    writer.addBytes("a.txt", "a", 1, &error);
    writer.addBytes("b.txt", "b", 1, &error);

    std::vector<std::uint8_t> bytes;
    CHECK_TRUE(writer.writeToMemory(bytes, &error));

    const std::uint64_t indexOffset = pack::readLE64(bytes.data() + 16);
    const std::uint64_t indexSize   = pack::readLE64(bytes.data() + 24);

    // The first index entry has a 5-byte path ("a.txt"); overwrite the second
    // path ("b.txt") with "a.txt" to create a duplicate.
    const std::size_t secondPath = static_cast<std::size_t>(indexOffset) + pack::kEntryFixedSize + 5 + pack::kEntryFixedSize;
    CHECK_TRUE(secondPath + 5 <= bytes.size());
    std::memcpy(bytes.data() + secondPath, "a.txt", 5);

    // Recompute the index CRC so only the ordering check can reject the pack.
    const std::uint32_t crc = pack::crc32(bytes.data() + indexOffset, static_cast<std::size_t>(indexSize));
    pack::writeLE32(bytes.data() + 32, crc);

    pack::Reader reader;
    CHECK_FALSE(reader.openMemory(bytes.data(), bytes.size(), &error));
    return true;
}

TEST(AssetPack_detects_data_crc_corruption)
{
    auto bytes = makePack();

    pack::Reader reader;
    std::string  error;
    CHECK_TRUE(reader.openMemory(bytes.data(), bytes.size(), &error));

    const pack::Entry *entry = reader.find("image/a.png");
    CHECK(entry != nullptr);

    bytes[static_cast<std::size_t>(entry->offset)] ^= 0xFF;

    pack::Reader damaged;
    CHECK_TRUE(damaged.openMemory(bytes.data(), bytes.size(), &error));
    const pack::Entry *damagedEntry = damaged.find("image/a.png");
    CHECK(damagedEntry != nullptr);

    std::vector<std::uint8_t> data;
    CHECK_FALSE(damaged.read(*damagedEntry, data, &error));
    CHECK_FALSE(damaged.verifyAll(&error));
    return true;
}

TEST(AssetPack_directory_and_file_roundtrip)
{
    namespace fs = std::filesystem;

    const fs::path  root = fs::temp_directory_path() / "lvglpp_assetpack_test";
    std::error_code ec;
    fs::remove_all(root, ec);
    fs::create_directories(root / "image" / "40x40", ec);
    fs::create_directories(root / ".hidden", ec);

    {
        std::ofstream(root / "image" / "40x40" / "icon.png", std::ios::binary) << "png-bytes";
        std::ofstream(root / "notes.txt", std::ios::binary) << "notes";
        std::ofstream(root / "ignored.db", std::ios::binary) << "db";
        std::ofstream(root / ".hidden" / "secret", std::ios::binary) << "hidden";
    }

    pack::Writer writer;
    std::string  error;
    CHECK_TRUE(writer.addDirectory(root.string(), pack::defaultExcludes(), &error));
    CHECK_EQ(writer.entryCount(), 2u);

    const fs::path packPath = root / "assets.pak";
    CHECK_TRUE(writer.writeFile(packPath.string(), &error));

    pack::Reader reader;
    CHECK_TRUE(reader.openFile(packPath.string(), &error));
    CHECK(reader.find("image/40x40/icon.png") != nullptr);
    CHECK(reader.find("notes.txt") != nullptr);
    CHECK(reader.find("ignored.db") == nullptr);
    CHECK(reader.find(".hidden/secret") == nullptr);

    std::vector<std::uint8_t> data;
    CHECK_TRUE(reader.read(*reader.find("notes.txt"), data, &error));
    CHECK_EQ(bytesToString(data), std::string("notes"));

    fs::remove_all(root, ec);
    return true;
}
