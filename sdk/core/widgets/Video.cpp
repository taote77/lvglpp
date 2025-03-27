#include <lvgl.h>

#if LV_USE_FFMPEG == 1

#include "Video.h"
#include "core/log/log.h"

namespace lvglpp {
namespace widgets {
Video::Video(const std::string &path, BaseItem *parent) : BaseItem(ItemType::Video, parent)
{
    setVideoSrcPath(path);
}

void Video::setVideoSrcPath(const std::string &path)
{
#ifdef USED_FRAMEBUFF
    src_path_ = "/heygears/app/bin/" + path;
#else
    src_path_ = "." + path;
#endif
    if (getLvglItem() != nullptr && !src_path_.empty())
    {
        lv_ffmpeg_player_set_src(getLvglItem(), src_path_.c_str());
    }
}

void Video::setVideoAutoRestart(bool b)
{
    lv_ffmpeg_player_set_auto_restart(getLvglItem(), b);
}

void Video::setVisible(bool visible)
{
    visible_ = visible;
    if (lv_time_update_visible_ == nullptr)
    {
        BaseItem::setVisible(visible);
    }
}

void Video::setVideoCmd(lv_ffmpeg_player_cmd_t cmd)
{
    if (getLvglItem() != nullptr && !src_path_.empty())
    {
        if (cmd == LV_FFMPEG_PLAYER_CMD_START)
        {
            BaseItem::setVisible(false);
            lv_ffmpeg_player_set_cmd(getLvglItem(), cmd);
            lv_time_update_visible_ = lv_timer_create(
                [](struct _lv_timer_t *t) -> void {
                    auto  user_data = lv_timer_get_user_data(t);
                    auto *video     = reinterpret_cast<Video *>(user_data);
                    if (video->visible_)
                    {
                        lv_obj_clear_flag(video->getLvglItem(), LV_OBJ_FLAG_HIDDEN);
                    } else
                    {
                        lv_obj_add_flag(video->getLvglItem(), LV_OBJ_FLAG_HIDDEN);
                    }
                    lv_timer_del(video->lv_time_update_visible_);
                    video->lv_time_update_visible_ = nullptr;
                },
                66, this);
        } else
        {
            lv_ffmpeg_player_set_cmd(getLvglItem(), cmd);
        }
    } else
    {
        LOG_ERROR() << "LvglItem == nullptr || src_path_ empty";
    }
}

Video::~Video()
{
    if (lv_time_update_visible_ != nullptr)
    {
        lv_timer_del(lv_time_update_visible_);
        lv_time_update_visible_ = nullptr;
    }
}

} // namespace widgets
} // namespace lvglpp

#endif // LV_USE_FFMPEG