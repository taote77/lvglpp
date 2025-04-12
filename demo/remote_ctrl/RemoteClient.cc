#include "RemoteClient.h"
#include "core/sys/Application.h"
#include "core/sys/Navigators.h"
#include "core/sys/SdlSimulateApplication.h"
#include "core/widgets/GlobalVar.h"
#include "session.h"
#include <boost/beast/core/detail/base64.hpp>
#include <core/log/log.h>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "core/tools/stb/stb_image.h"
#include "core/tools/stb/stb_image_resize2.h"
#include "core/tools/stb/stb_image_write.h"

enum ComTopic {
    MOUSE_EVENT,
    CLOUS_IMAGE_STREAM_PUSH,
};

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
        //
    }

    success = stbi_write_jpg("screenshot.jpg", width, height, 4, data, 90);
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

    // LOG_DEBUG() << "Screenshot taken successfully." << width << "x" << height;

    // 调用 lv_snapshot_take 函数进行截图
    lv_draw_buf_t *draw_buf = lv_snapshot_take(screen, LV_COLOR_FORMAT_ARGB8888);
    if (!draw_buf)
    {
        std::cerr << "Failed to take the snapshot." << std::endl;
        return false;
    }

    // LOG_DEBUG() << "Screenshot taken successfully." << width << "x" << height << draw_buf->header.cf;

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

std::string base64_encode(const std::string &input)
{
    std::string output;
    output.resize(boost::beast::detail::base64::encoded_size(input.size()));
    output.resize(boost::beast::detail::base64::encode(&output[0], input.data(), input.size()));
    return output;
}

RemoteClient::RemoteClient() : sys::BaseActivity()
{}

RemoteClient::~RemoteClient()
{}

void RemoteClient::onCreate(void *arg)
{
    WebsocketSession::Instance().OnMsgHandler([this](boost::json::object &req, boost::json::object &res) {
        auto topic = req["topic"].as_string();

        auto data_json = req["data"].as_object();

        LOG_DEBUG() << "===========================================" << topic;

        if (topic == "MOUSE_EVENT")
        {
            static long long last_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            auto now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            if (now - last_time < 150)
            {
                return;
            }

            int x = data_json["x"].as_int64();
            int y = data_json["y"].as_int64();
            LOG_DEBUG() << "MOUSE_EVENT pos: " << x << y;

            last_time = now;
            lvglpp::sys::Navigators::getInstance()->getApplication()->postEvent(lvglpp::sys::Event(ComTopic::MOUSE_EVENT, 0, std::pair<int, int>(x, y)));
        }
    });

    _label = std::make_shared<lvglpp::widgets::LvText>(("Demo"), CLR_SUCCESS_CONTAINER, getRootItem());

    _label->setAligment(LV_ALIGN_CENTER, 0, -120);

    _btn_test = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "截图", getRootItem());
    _btn_test->setPos(480, 300);
    _btn_test->setAligment(LV_ALIGN_CENTER, 0, 150);
    _btn_test->setOnClickedListener([this]() {
        LOG_DEBUG() << "Test lang";
        _btn_test->emitSignal("snapshoot");
    });

    Object::connect(_btn_test.get(), "snapshoot", [this]() {
        lv_obj_t *screen = lv_scr_act();
        // getRootItem()->getLvglItem();
        if (takeScreenshot(screen, "screenshot.png"))
        {
            // std::cout << "Screenshot saved successfully." << std::endl;

            std::ifstream file("./screenshot.jpg", std::ios::binary);

            // 读取文件内容到缓冲区
            std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            file.close();

            std::string base64Data = base64_encode(buffer);

            boost::json::object data;
            data["topic"] = "IMAGE_DATA";
            data["data"]  = base64Data;

            // std::cout << "JSON data length: " << base64Data.length() << std::endl;

            WebsocketSession::Instance().Post(data);
        } else
        {
            std::cerr << "Failed to save screenshot." << std::endl;
        }
    });

    _btn_simu = std::make_shared<widgets::RoundedButton>(200, 40, widgets::RoundedButton::ColorStyle::Gray, "重置", getRootItem());

    _btn_simu->setAligment(LV_ALIGN_TOP_RIGHT, 0, 0);
    _btn_simu->setOnClickedListener([this]() {
        LOG_DEBUG() << "Simu lang";
        _count = 0;
    });

    _timer = lv_timer_create(
        [](lv_timer_t *data) -> void {
            auto user_data = lv_timer_get_user_data(data);
            auto page      = (RemoteClient *)user_data;

            page->_count++;
            page->_label->setText(std::to_string(page->_count));
            page->_btn_test->emitSignal("snapshoot");
        },
        1 * 1000, this);
}

void RemoteClient::onNotifyUI(const sys::Event &evt)
{
    BaseActivity::onNotifyUI(evt);

    if (evt.getType() == ComTopic::MOUSE_EVENT)
    {
        LOG_DEBUG() << "MOUSE_EVENT";
        auto data = evt.convertData<std::pair<int, int>>();
        if (data)
        {
            auto pos = *data;
            LOG_DEBUG() << "MOUSE_EVENT pos: " << pos.first << pos.second;
            sys::SdlSimulateApplication::simulate_click_at(pos.first, pos.second);
        }
    }
}

void RemoteClient::onDestroy()
{}
