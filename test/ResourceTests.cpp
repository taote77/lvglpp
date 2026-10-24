#include "TestFramework.h"
#include "core/tools/AssetManager.h"
#include "core/tools/ImageManager.h"
#include "core/tools/Utils.h"
#include "core/tools/stb/StbImageHelpers.h"
#include "core/widgets/Base64Image.h"
#include "core/widgets/Image.h"
#include "core/widgets/Widget.h"
#include "lvgl.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

using namespace lvglpp;
using namespace lvglpp::test;

namespace {

namespace fs = std::filesystem;

// 1x1 red RGBA PNG, base64 encoded.
constexpr const char *kPng1x1Base64 = "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGP4z8DwHwAFAAH/"
                                      "iZk9HQAAAABJRU5ErkJggg==";

std::vector<std::uint8_t> makePng(int width, int height, std::uint8_t red, std::uint8_t green, std::uint8_t blue)
{
    std::vector<std::uint8_t> rgba(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4);
    for (std::size_t i = 0; i + 3 < rgba.size(); i += 4)
    {
        rgba[i]     = red;
        rgba[i + 1] = green;
        rgba[i + 2] = blue;
        rgba[i + 3] = 255;
    }

    return tools::encodePng(rgba.data(), width, height, 4);
}

struct ImageFixture {
    fs::path root;

    ImageFixture()
    {
        tools::AssetManager::getInstance().reset();
        tools::ImageManager::shutdown();

        std::error_code ec;
        root = fs::temp_directory_path() / "lvglpp_resource_test";
        fs::remove_all(root, ec);
        fs::create_directories(root / "image", ec);
    }

    ~ImageFixture()
    {
        tools::ImageManager::shutdown();
        tools::AssetManager::getInstance().reset();
        std::error_code ec;
        fs::remove_all(root, ec);
    }

    bool write(const std::string &logicalPath, const std::vector<std::uint8_t> &bytes)
    {
        const fs::path  path = root / logicalPath;
        std::error_code ec;
        fs::create_directories(path.parent_path(), ec);
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        if (!output)
        {
            return false;
        }
        if (!bytes.empty())
        {
            output.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        }
        return static_cast<bool>(output);
    }

    bool mount()
    {
        return tools::AssetManager::getInstance().mountDirectory(root.string());
    }
};

} // namespace

TEST(Base64Image_pixels_survive_wrapper_destruction)
{
    auto parent = std::make_unique<widgets::Widget>();

    lv_obj_t *image_obj = nullptr;
    {
        auto image = std::make_unique<widgets::Base64Image>(kPng1x1Base64, 4, 4, parent.get());
        CHECK_TRUE(image->isAttached());
        image_obj = image->lvObj();
    }

    CHECK(image_obj != nullptr);

    const auto *dsc = static_cast<const lv_image_dsc_t *>(lv_image_get_src(image_obj));
    CHECK(dsc != nullptr);
    CHECK(dsc->data != nullptr);
    CHECK_EQ(dsc->header.w, 4u);
    CHECK_EQ(dsc->header.h, 4u);
    CHECK_EQ(dsc->data_size, 4u * 4u * 4u);

    // Source is a red PNG; LVGL ARGB8888 stores BGRA in memory.
    const auto *pixel = static_cast<const std::uint8_t *>(dsc->data);
    CHECK_EQ(pixel[0], 0u);
    CHECK_EQ(pixel[1], 0u);
    CHECK_EQ(pixel[2], 255u);
    CHECK_EQ(pixel[3], 255u);

    lv_obj_invalidate(image_obj);
    lv_refr_now(nullptr);
    return true;
}

TEST(Base64Image_update_replaces_descriptor_safely)
{
    auto parent = std::make_unique<widgets::Widget>();
    auto image  = std::make_unique<widgets::Base64Image>(kPng1x1Base64, 4, 4, parent.get());

    const auto *before = static_cast<const lv_image_dsc_t *>(lv_image_get_src(image->lvObj()));
    CHECK(before != nullptr);

    image->setBase64(kPng1x1Base64);

    const auto *after = static_cast<const lv_image_dsc_t *>(lv_image_get_src(image->lvObj()));
    CHECK(after != nullptr);
    CHECK(after != before);
    CHECK(after->data != nullptr);

    lv_obj_invalidate(image->lvObj());
    lv_refr_now(nullptr);
    return true;
}

TEST(Base64Image_letterboxes_and_centers)
{
    const auto  png = makePng(2, 1, 255, 0, 0);
    std::string base64;
    CHECK_TRUE(tools::Utils::base64Encode(std::string(png.begin(), png.end()), base64));

    auto                 parent = std::make_unique<widgets::Widget>();
    widgets::Base64Image image(base64, 4, 4, parent.get());
    const auto          *dsc = static_cast<const lv_image_dsc_t *>(lv_image_get_src(image.lvObj()));
    CHECK(dsc != nullptr);
    CHECK_EQ(dsc->header.w, 4);
    CHECK_EQ(dsc->header.h, 4);

    const auto *pixels = static_cast<const std::uint8_t *>(dsc->data);
    const auto  stride = dsc->header.stride;
    for (std::uint32_t x = 0; x < 4; ++x)
    {
        // Top and bottom rows are transparent letterbox padding.
        CHECK_EQ(pixels[x * 4 + 3], 0u);
        CHECK_EQ(pixels[3 * stride + x * 4 + 3], 0u);

        // The 2x1 red source is scaled to 4x2 and centered on row 1.
        const auto *pixel = pixels + stride + x * 4;
        CHECK_EQ(pixel[0], 0u);
        CHECK_EQ(pixel[1], 0u);
        CHECK_EQ(pixel[2], 255u);
        CHECK_EQ(pixel[3], 255u);
    }
    return true;
}

TEST(ImageManager_loads_asset_from_directory)
{
    ImageFixture fixture;
    CHECK_TRUE(fixture.write("image/red.png", makePng(4, 4, 255, 0, 0)));
    CHECK_TRUE(fixture.mount());

    const auto handle = tools::ImageManager::load("image/red.png");
    CHECK(handle != nullptr);
    CHECK_EQ(handle->width(), 4);
    CHECK_EQ(handle->height(), 4);
    CHECK_TRUE(tools::ImageManager::isCached("image/red.png"));

    const std::uint8_t *pixel = static_cast<const std::uint8_t *>(handle->descriptor()->data);
    CHECK_EQ(pixel[0], 0u);   // blue
    CHECK_EQ(pixel[1], 0u);   // green
    CHECK_EQ(pixel[2], 255u); // red
    CHECK_EQ(pixel[3], 255u); // alpha

    const auto again = tools::ImageManager::load("image/red.png");
    CHECK(again == handle);
    return true;
}

TEST(ImageManager_load_file_source)
{
    ImageFixture fixture;
    CHECK_TRUE(fixture.write("image/file.png", makePng(3, 2, 0, 255, 0)));

    const fs::path filePath = fixture.root / "image" / "file.png";
    const auto     handle   = tools::ImageManager::loadFile(filePath.string());
    CHECK(handle != nullptr);
    CHECK_EQ(handle->width(), 3);
    CHECK_EQ(handle->height(), 2);

    const std::string key = "file:" + fs::absolute(filePath).lexically_normal().string();
    CHECK_TRUE(tools::ImageManager::isCached(key));
    return true;
}

TEST(ImageManager_missing_asset_returns_null)
{
    ImageFixture fixture;
    CHECK_TRUE(fixture.mount());
    CHECK(tools::ImageManager::load("image/missing.png") == nullptr);
    return true;
}

TEST(ImageManager_lru_entry_limit_and_handle_survival)
{
    ImageFixture fixture;
    CHECK_TRUE(fixture.write("image/a.png", makePng(1, 1, 255, 0, 0)));
    CHECK_TRUE(fixture.write("image/b.png", makePng(1, 1, 0, 255, 0)));
    CHECK_TRUE(fixture.write("image/c.png", makePng(1, 1, 0, 0, 255)));
    CHECK_TRUE(fixture.mount());

    tools::ImageManager::setCacheLimits(2, 64ull * 1024 * 1024);

    const auto a = tools::ImageManager::load("image/a.png");
    const auto b = tools::ImageManager::load("image/b.png");
    CHECK(a != nullptr);
    CHECK(b != nullptr);
    CHECK_EQ(tools::ImageManager::cacheSize(), 2u);

    // Touch A so B becomes the least recently used entry.
    tools::ImageManager::load("image/a.png");
    const auto c = tools::ImageManager::load("image/c.png");
    CHECK(c != nullptr);
    CHECK_TRUE(tools::ImageManager::isCached("image/a.png"));
    CHECK_TRUE(tools::ImageManager::isCached("image/c.png"));
    CHECK_FALSE(tools::ImageManager::isCached("image/b.png"));

    // Eviction only drops the cache reference; the caller's handle stays valid.
    CHECK(b != nullptr);
    CHECK(b->descriptor()->data != nullptr);

    tools::ImageManager::setCacheLimits(32, 64ull * 1024 * 1024);
    return true;
}

TEST(ImageManager_lru_byte_budget)
{
    ImageFixture fixture;
    CHECK_TRUE(fixture.write("image/small.png", makePng(12, 12, 255, 0, 0)));
    CHECK_TRUE(fixture.write("image/large.png", makePng(20, 20, 0, 255, 0)));
    CHECK_TRUE(fixture.mount());

    tools::ImageManager::setCacheLimits(16, 2000);

    const auto small = tools::ImageManager::load("image/small.png");
    CHECK(small != nullptr);
    CHECK_EQ(small->byteSize(), 12u * 12u * 4u);

    const auto large = tools::ImageManager::load("image/large.png");
    CHECK(large != nullptr);
    CHECK_EQ(large->byteSize(), 20u * 20u * 4u);
    CHECK_TRUE(tools::ImageManager::cacheBytes() <= 2000u);
    CHECK_FALSE(tools::ImageManager::isCached("image/small.png"));
    CHECK_TRUE(tools::ImageManager::isCached("image/large.png"));

    tools::ImageManager::setCacheLimits(32, 64ull * 1024 * 1024);
    return true;
}

TEST(ImageManager_prefetch_warms_cache)
{
    ImageFixture fixture;
    CHECK_TRUE(fixture.write("image/p1.png", makePng(2, 2, 255, 0, 0)));
    CHECK_TRUE(fixture.write("image/p2.png", makePng(2, 2, 0, 255, 0)));
    CHECK_TRUE(fixture.mount());

    tools::ImageManager::prefetch(std::vector<std::string>{"image/p1.png", "image/p2.png"});
    tools::ImageManager::waitForPrefetch();

    CHECK_TRUE(tools::ImageManager::isCached("image/p1.png"));
    CHECK_TRUE(tools::ImageManager::isCached("image/p2.png"));

    // Shutdown is idempotent and leaves the manager restartable.
    tools::ImageManager::shutdown();
    tools::ImageManager::shutdown();
    CHECK_EQ(tools::ImageManager::cacheSize(), 0u);
    return true;
}

TEST(Image_widget_uses_asset_path)
{
    ImageFixture fixture;
    CHECK_TRUE(fixture.write("image/red.png", makePng(2, 2, 255, 0, 0)));
    CHECK_TRUE(fixture.mount());

    auto parent = std::make_unique<widgets::Widget>();

    lv_obj_t *image_obj = nullptr;
    {
        auto image = std::make_unique<widgets::Image>("image/red.png", parent.get());
        CHECK_TRUE(image->isAttached());
        image_obj = image->lvObj();
    }

    // The wrapper is gone; the registry keepalive must keep the descriptor alive.
    const auto *dsc = static_cast<const lv_image_dsc_t *>(lv_image_get_src(image_obj));
    CHECK(dsc != nullptr);
    CHECK(dsc->data != nullptr);
    CHECK_EQ(dsc->header.w, 2u);
    CHECK_EQ(dsc->header.h, 2u);

    lv_obj_invalidate(image_obj);
    lv_refr_now(nullptr);
    return true;
}
