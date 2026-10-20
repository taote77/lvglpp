#include "TestFramework.h"
#include "core/sys/FileSystemManager.h"
#include "core/tools/AssetManager.h"
#include "core/tools/assets/AssetPack.h"
#include "lvgl.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using namespace lvglpp;
using namespace lvglpp::test;

namespace {

namespace fs = std::filesystem;

struct AssetFixture {
    AssetFixture()
    {
        tools::AssetManager::getInstance().reset();
    }

    ~AssetFixture()
    {
        tools::AssetManager::getInstance().reset();
    }
};

std::vector<std::uint8_t> makePack(const std::string &path, const std::string &contents)
{
    tools::pack::Writer writer;
    std::string         error;
    writer.addBytes(path, contents.data(), contents.size(), &error);

    std::vector<std::uint8_t> bytes;
    writer.writeToMemory(bytes, &error);
    return bytes;
}

std::string readAsset(const tools::AssetHandle &asset)
{
    if (!asset)
    {
        return {};
    }
    return std::string(reinterpret_cast<const char *>(asset->data()), asset->size());
}

std::string tempRoot()
{
    return (fs::temp_directory_path() / "lvglpp_assetmanager_test").string();
}

} // namespace

TEST(AssetManager_memory_pack)
{
    AssetFixture fixture;

    const auto bytes = makePack("image/icon.txt", "hello");
    CHECK_TRUE(tools::AssetManager::getInstance().mountMemory(bytes.data(), bytes.size()));
    CHECK_EQ(tools::AssetManager::getInstance().mountCount(), 1u);

    const auto asset = tools::AssetManager::getInstance().open("image/icon.txt");
    CHECK(asset != nullptr);
    CHECK_EQ(asset->path(), std::string("image/icon.txt"));
    CHECK_EQ(readAsset(asset), std::string("hello"));
    CHECK_TRUE(tools::AssetManager::getInstance().exists("image/icon.txt"));
    CHECK_FALSE(tools::AssetManager::getInstance().exists("image/missing.txt"));
    CHECK(tools::AssetManager::getInstance().open("image/missing.txt") == nullptr);
    return true;
}

TEST(AssetManager_directory_hot_reload_and_list)
{
    AssetFixture    fixture;
    std::error_code ec;

    const fs::path root = tempRoot();
    fs::remove_all(root, ec);
    fs::create_directories(root / "image", ec);
    {
        std::ofstream(root / "image" / "a.txt", std::ios::binary) << "first";
    }

    CHECK_TRUE(tools::AssetManager::getInstance().mountDirectory(root.string()));
    CHECK_EQ(readAsset(tools::AssetManager::getInstance().open("image/a.txt")), std::string("first"));

    // Directory sources re-read on every open, so edits are visible immediately.
    {
        std::ofstream(root / "image" / "a.txt", std::ios::binary | std::ios::trunc) << "second";
    }
    CHECK_EQ(readAsset(tools::AssetManager::getInstance().open("image/a.txt")), std::string("second"));

    const auto listed = tools::AssetManager::getInstance().list("image/");
    CHECK_EQ(listed.size(), 1u);
    CHECK_EQ(listed[0], std::string("image/a.txt"));

    // Traversal is rejected; an LVGL-style leading slash is normalized away.
    CHECK(tools::AssetManager::getInstance().open("../escape.txt") == nullptr);
    CHECK(tools::AssetManager::getInstance().exists("/image/a.txt"));

    fs::remove_all(root, ec);
    return true;
}

TEST(AssetManager_last_mount_wins)
{
    AssetFixture    fixture;
    std::error_code ec;

    const fs::path root = tempRoot() + "_priority";
    fs::remove_all(root, ec);
    fs::create_directories(root, ec);
    {
        std::ofstream(root / "shared.txt", std::ios::binary) << "common";
    }

    const auto appBytes    = makePack("shared.txt", "app");
    const auto commonBytes = makePack("shared.txt", "common");

    CHECK_TRUE(tools::AssetManager::getInstance().mountMemory(commonBytes.data(), commonBytes.size()));
    CHECK_TRUE(tools::AssetManager::getInstance().mountMemory(appBytes.data(), appBytes.size()));

    CHECK_EQ(readAsset(tools::AssetManager::getInstance().open("shared.txt")), std::string("app"));
    CHECK_TRUE(tools::AssetManager::getInstance().mountDirectory(root.string()));
    CHECK_EQ(readAsset(tools::AssetManager::getInstance().open("shared.txt")), std::string("common"));

    fs::remove_all(root, ec);
    return true;
}

TEST(AssetManager_handles_survive_reset)
{
    AssetFixture fixture;

    const auto bytes = makePack("a.txt", "payload");
    CHECK_TRUE(tools::AssetManager::getInstance().mountMemory(bytes.data(), bytes.size()));
    const auto asset = tools::AssetManager::getInstance().open("a.txt");
    CHECK(asset != nullptr);

    tools::AssetManager::getInstance().reset();
    CHECK_EQ(tools::AssetManager::getInstance().mountCount(), 0u);
    CHECK_EQ(readAsset(asset), std::string("payload"));
    return true;
}

TEST(AssetManager_rejects_corrupt_pack)
{
    AssetFixture fixture;

    auto bytes = makePack("a.txt", "payload");
    bytes[0]   = 'X';
    CHECK_FALSE(tools::AssetManager::getInstance().mountMemory(bytes.data(), bytes.size()));

    bytes = makePack("a.txt", "payload");
    bytes.back() ^= 0xFF;
    CHECK_FALSE(tools::AssetManager::getInstance().mountMemory(bytes.data(), bytes.size()));
    return true;
}

TEST(AssetManager_materializes_pack_and_directory_entries)
{
    AssetFixture    fixture;
    std::error_code ec;

    const auto bytes = makePack("fonts/test.ttf", "font-bytes");
    CHECK_TRUE(tools::AssetManager::getInstance().mountMemory(bytes.data(), bytes.size()));

    std::string materialized;
    CHECK_TRUE(tools::AssetManager::getInstance().materialize("fonts/test.ttf", materialized));
    CHECK_FALSE(materialized.empty());

    std::ifstream input(materialized, std::ios::binary);
    CHECK_TRUE(static_cast<bool>(input));
    const std::string contents((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    CHECK_EQ(contents, std::string("font-bytes"));

    // Repeated calls reuse the extracted file.
    std::string again;
    CHECK_TRUE(tools::AssetManager::getInstance().materialize("fonts/test.ttf", again));
    CHECK_EQ(again, materialized);

    // Directory sources return their real path directly.
    const fs::path root = tempRoot() + "_materialize";
    fs::remove_all(root, ec);
    fs::create_directories(root / "image", ec);
    {
        std::ofstream(root / "image" / "a.txt", std::ios::binary) << "dir-bytes";
    }
    CHECK_TRUE(tools::AssetManager::getInstance().mountDirectory(root.string()));
    std::string directoryPath;
    CHECK_TRUE(tools::AssetManager::getInstance().materialize("image/a.txt", directoryPath));
    CHECK_EQ(fs::weakly_canonical(directoryPath, ec).string(), fs::weakly_canonical(root / "image" / "a.txt", ec).string());

    fs::remove_all(root, ec);
    return true;
}

TEST(AssetManager_fs_driver_reads_pack)
{
    AssetFixture fixture;
    sys::FileSystemManager::getInstance().registerAssetDriver();

    const std::string contents = "0123456789";
    const auto        bytes    = makePack("image/data.bin", contents);
    CHECK_TRUE(tools::AssetManager::getInstance().mountMemory(bytes.data(), bytes.size()));

    lv_fs_file_t file;
    CHECK_EQ(lv_fs_open(&file, "A:/image/data.bin", LV_FS_MODE_RD), LV_FS_RES_OK);

    char          buffer[4] = {};
    std::uint32_t read      = 0;
    CHECK_EQ(lv_fs_read(&file, buffer, 4, &read), LV_FS_RES_OK);
    CHECK_EQ(read, 4u);
    CHECK_EQ(std::string(buffer, 4), std::string("0123"));

    std::uint32_t position = 0;
    CHECK_EQ(lv_fs_tell(&file, &position), LV_FS_RES_OK);
    CHECK_EQ(position, 4u);

    // Negative relative seek.
    CHECK_EQ(lv_fs_seek(&file, static_cast<std::uint32_t>(-2), LV_FS_SEEK_CUR), LV_FS_RES_OK);
    CHECK_EQ(lv_fs_tell(&file, &position), LV_FS_RES_OK);
    CHECK_EQ(position, 2u);

    CHECK_EQ(lv_fs_seek(&file, 0, LV_FS_SEEK_END), LV_FS_RES_OK);
    CHECK_EQ(lv_fs_tell(&file, &position), LV_FS_RES_OK);
    CHECK_EQ(position, 10u);

    CHECK_EQ(lv_fs_read(&file, buffer, 4, &read), LV_FS_RES_OK);
    CHECK_EQ(read, 0u);
    CHECK_EQ(lv_fs_close(&file), LV_FS_RES_OK);

    lv_fs_file_t missing;
    CHECK_EQ(lv_fs_open(&missing, "A:/image/missing.bin", LV_FS_MODE_RD), LV_FS_RES_UNKNOWN);
    return true;
}
