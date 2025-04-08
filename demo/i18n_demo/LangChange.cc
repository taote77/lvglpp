#include "LangChange.h"
#include "LvTranslator.h"
#include "core/sys/Navigators.h"
#include "core/sys/SdlSimulateApplication.h"
#include "core/widgets/GlobalVar.h"
#include "session.h"
#include <core/log/log.h>
#include <fstream>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "core/tools/stb/stb_image.h"
#include "core/tools/stb/stb_image_resize2.h"
#include "core/tools/stb/stb_image_write.h"

using namespace lvglpp::core;

uint8_t *extract_argb8888_to_rgba(const lv_draw_buf_t *draw_buf)
{
    uint32_t width      = draw_buf->header.w;
    uint32_t height     = draw_buf->header.h;
    uint32_t src_stride = draw_buf->header.stride; // 包含填充的每行字节数
    uint8_t *src        = (uint8_t *)draw_buf->data;

    // 目标缓冲区（RGBA8888，无填充）
    uint8_t *dst = (uint8_t *)malloc(width * height * 4);
    if (!dst)
        return NULL;

    for (uint32_t y = 0; y < height; y++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            // 从 ARGB8888 读取像素（4字节/像素）
            uint8_t a = src[y * src_stride + x * 4 + 0]; // Alpha
            uint8_t r = src[y * src_stride + x * 4 + 1]; // Red
            uint8_t g = src[y * src_stride + x * 4 + 2]; // Green
            uint8_t b = src[y * src_stride + x * 4 + 3]; // Blue

            // 转换为 RGBA8888 布局（Alpha 在最后）
            dst[(y * width + x) * 4 + 0] = r;
            dst[(y * width + x) * 4 + 1] = g;
            dst[(y * width + x) * 4 + 2] = b;
            dst[(y * width + x) * 4 + 3] = a;
        }
    }
    return dst;
}

void save_rgb888_to_png(const char *filename, const uint8_t *data, int width, int height)
{
    // 参数说明：
    // filename: 输出文件名
    // width: 图像宽度
    // height: 图像高度
    // 3: 通道数（RGB）
    // data: 去除了 stride 的 RGB888 数据
    // width * 3: 每行字节数（无填充）
    int success = stbi_write_png(filename, width, height, 4, data, width * 4);
    if (!success)
    {
        LV_LOG_ERROR("Failed to write PNG: %s", filename);

        LOG_DEBUG() << "Failed to write PNG " << filename;
    } else
    {
        LOG_DEBUG() << "Saved PNG: " << filename;
    }
}

bool takeScreenshot(lv_obj_t *screen, const std::string &filename)
{
    // 获取当前活动屏幕对象
    // lv_obj_t *screen = lv_scr_act();
    if (!screen)
    {
        std::cerr << "Failed to get the active screen." << std::endl;
        return false;
    }

    // 获取屏幕的宽度和高度
    lv_coord_t width  = lv_obj_get_width(screen);
    lv_coord_t height = lv_obj_get_height(screen);

    LOG_DEBUG() << "Screenshot taken successfully." << width << "x" << height;

    // 调用 lv_snapshot_take 函数进行截图
    lv_draw_buf_t *draw_buf = lv_snapshot_take(screen, LV_COLOR_FORMAT_ARGB8888);
    if (!draw_buf)
    {
        std::cerr << "Failed to take the snapshot." << std::endl;
        return false;
    }

    LOG_DEBUG() << "Screenshot taken successfully." << width << "x" << height << draw_buf->header.cf;

    // 去除 stride 的填充字节
    uint8_t *rgb888_data = extract_argb8888_to_rgba(draw_buf);
    if (!rgb888_data)
    {
        lv_draw_buf_destroy(draw_buf);
        return false;
    }

    save_rgb888_to_png("screenshot.png", rgb888_data, draw_buf->header.w, draw_buf->header.h);

    if (draw_buf)
    {
        if (draw_buf->data)
        {
            lv_free(draw_buf->data);
        }

        lv_free(draw_buf);
        draw_buf = nullptr;
    }

    free(rgb888_data);
    return true;
}

LangChangePage::LangChangePage() : sys::BaseActivity()
{}

LangChangePage::~LangChangePage()
{}

void LangChangePage::onCreate(void *arg)
{
    _label = std::make_shared<lvglpp::widgets::I18nText>(lvglpp::LvTranslator::tr("ChangeLangWarnText"), CLR_SUCCESS_CONTAINER, getRootItem());

    _label->setAligment(LV_ALIGN_CENTER, 0, -120);

    static auto createLabel = [this]() {
        _label1 = std::make_shared<lvglpp::widgets::LvText>(lvglpp::LvTranslator::tr("ChangeLangWarnText"), CLR_SUCCESS_CONTAINER, getRootItem());

        _label->setAligment(LV_ALIGN_CENTER, 0, -140);
    };

    auto btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "中文", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, -50);
    btn->setOnClickedListener([this]() {
        //
        LOG_DEBUG() << "Chinese lang";

        _txt = (_txt == "Start") ? "Stop" : "Start";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::CHINESE);
        createLabel();

        auto msg = lvglpp::LvTranslator::getI18nText("ChangeLangWarnText");

        _label->setText(msg);
    });
    _btn_chinese = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "英文", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 0);
    btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";
        //
        LOG_DEBUG() << "English lang";
        std::cout << "dsdsdsds";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::ENGLISH);

        createLabel();

        _label->langTrigerred();
    });
    _btn_english = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "日文", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 50);
    btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";

        LOG_DEBUG() << "Japanese lang";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::JAPANESE);
        createLabel();

        _label->langTrigerred();
    });

    _btn_japanese = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "俄语", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 100);
    btn->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";

        LOG_DEBUG() << "Russian lang";
        lvglpp::LvTranslator::load(lvglpp::LANGUAGE::RUSSIAN);

        createLabel();
        _label->langTrigerred();
    });
    _btn_russian = btn;

    btn = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "测试", getRootItem());
    btn->setPos(480, 300);
    btn->setAligment(LV_ALIGN_CENTER, 0, 150);
    btn->setOnClickedListener([this, btn]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";

        LOG_DEBUG() << "Test lang";

        createLabel();
        _label->langTrigerred();

        btn->emitSignal("hello");

        sys::SdlSimulateApplication::simulate_click_at(780, 4);
    });

    Object::connect(btn.get(), "hello", [this]() {
        LOG_DEBUG() << "onHello";
        //

        lv_obj_t *screen = lv_scr_act();
        // getRootItem()->getLvglItem();
        if (takeScreenshot(screen, "screenshot.png"))
        {
            std::cout << "Screenshot saved successfully." << std::endl;

            std::ifstream file("screenshot.png", std::ios::binary);

            // 读取文件内容到缓冲区
            std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            file.close();

            boost::json::object data;
            data["topic"] = "image/png";
            data["data"]  = buffer;

            std::cout << "JSON data length: " << buffer.length() << std::endl;

            WebsocketSession::Instance().Post(data);
        } else
        {
            std::cerr << "Failed to save screenshot." << std::endl;
        }
    });

    _btn_test = btn;

    _btn_simu = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "模拟", getRootItem());
    _btn_simu->setPos(480, 300);

    _btn_simu->setAligment(LV_ALIGN_TOP_RIGHT, 0, 0);
    _btn_simu->setOnClickedListener([this]() {
        _txt = (_txt == "Start") ? "Stop" : "Start";

        LOG_DEBUG() << "Simu lang";

        boost::json::object data;
        data["topic"] = "test";
        data["data"]  = "hello";

        WebsocketSession::Instance().Post(data);
    });
}

void LangChangePage::onNotifyUI(const sys::Event &evt)
{
    (void)evt;
}

void LangChangePage::onDestroy()
{}
