/**************************************************************************

Copyright:Copyright  2023 HeyGears. All rights reserved

Author: Lhyang hli3@heygears.com

Date:2023-05-29

Class:Video

Description:

**************************************************************************/
#include <lvgl.h>

#if LV_USE_FFMPEG == 1

#include "Video.h"
#include "log/log.h"

namespace heygears
{
    namespace widgets
    {
        Video::Video(const std::string &path, BaseItem *parent) : BaseItem(parent)
        {
            setVideoSrcPath(path);
        }

        void Video::setVideoSrcPath(const std::string &path)
        {
            src_path_ = path;
            if (getLvglItem() != nullptr && !src_path_.empty())
            {
                player_ = lv_ffmpeg_player_create(getLvglItem());
                lv_ffmpeg_player_set_src(player_, path.c_str());
            }
        }

        void Video::setVideoAutoRestart(bool b)
        {
            lv_ffmpeg_player_set_auto_restart(player_, b);
        }

        void Video::setVideoCmd(lv_ffmpeg_player_cmd_t cmd)
        {
            if (getLvglItem() != nullptr && !src_path_.empty())
            {
                lv_ffmpeg_player_set_cmd(player_, cmd);
            }
            else
            {
                LOG_ERROR()<<"LvglItem == nullptr || src_path_ empty";
            }
        }

    } // heygears
} // widgets

#endif //LV_USE_FFMPEG