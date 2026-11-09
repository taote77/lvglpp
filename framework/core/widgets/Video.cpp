#include "core/base/LvObjFactory.h"
#include <lvgl.h>

#if LV_USE_FFMPEG == 1

#include "Video.h"
#include "core/log/log.h"
#include <memory>

namespace lvglpp::widgets {

Video::Video(const std::string &path, Widget *parent) : Widget(tools::LvObjFactory::createLvglVideo, parent)
{
    setVideoSrcPath(path);
}

void Video::setVideoSrcPath(const std::string &path)
{
#if defined(LVGLPP_PLATFORM_FRAMEBUF)
    src_path_ = "/home/app/bin/" + path;
#else
    src_path_ = "." + path;
#endif
    if (lvObj() != nullptr && !src_path_.empty())
    {
        auto persistent = std::make_shared<std::string>(src_path_);
        setObjectKeepalive("video.src", persistent);
        lv_ffmpeg_player_set_src(lvObj(), persistent->c_str());
    }
}

void Video::setVideoAutoRestart(bool b)
{
    lv_ffmpeg_player_set_auto_restart(lvObj(), b);
}

void Video::setVisible(bool visible)
{
    visible_ = visible;
    if (lv_time_update_visible_ == nullptr)
    {
        Widget::setVisible(visible);
    }
}

void Video::setVideoCmd(lv_ffmpeg_player_cmd_t cmd)
{
    if (lvObj() != nullptr && !src_path_.empty())
    {
        if (cmd == LV_FFMPEG_PLAYER_CMD_START)
        {
            Widget::setVisible(false);
            lv_ffmpeg_player_set_cmd(lvObj(), cmd);
            lv_time_update_visible_ = lv_timer_create(
                [](struct _lv_timer_t *t) -> void {
                    auto  user_data = lv_timer_get_user_data(t);
                    auto *video     = reinterpret_cast<Video *>(user_data);
                    if (video == nullptr)
                    {
                        return;
                    }
                    if (video->lvObj() != nullptr)
                    {
                        if (video->visible_)
                        {
                            lv_obj_clear_flag(video->lvObj(), LV_OBJ_FLAG_HIDDEN);
                        } else
                        {
                            lv_obj_add_flag(video->lvObj(), LV_OBJ_FLAG_HIDDEN);
                        }
                    }
                    video->cleanupTimer();
                },
                66, this);
        } else
        {
            lv_ffmpeg_player_set_cmd(lvObj(), cmd);
        }
    } else
    {
        LOG_ERROR() << "LvglItem == nullptr || src_path_ empty";
    }
}

void Video::cleanupTimer()
{
    if (lv_time_update_visible_ != nullptr)
    {
        lv_timer_delete(lv_time_update_visible_);
        lv_time_update_visible_ = nullptr;
    }
}

void Video::onLvglObjectDeleted()
{
    cleanupTimer();
}

Video::~Video()
{
    cleanupTimer();
}

} // namespace lvglpp::widgets

#endif // LV_USE_FFMPEG