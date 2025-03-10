/**************************************************************************

Copyright:Copyright  2023 HeyGears. All rights reserved

Author: Lhyang hli3@heygears.com

Date:2023-05-29

Class:Video

Description:

**************************************************************************/

#ifndef ULTRACORE_REFLEX_VIDEO_H
#define ULTRACORE_REFLEX_VIDEO_H

#if LV_USE_FFMPEG == 1

#include "BaseItem.h"
#include <string>

namespace heygears
{
    namespace widgets
    {

        class Video : public BaseItem
        {
        public:
            explicit Video(const std::string& path, BaseItem *parent = nullptr);

            void setVideoSrcPath(const std::string &path);

            void setVideoAutoRestart(bool b);

            void setVideoCmd(lv_ffmpeg_player_cmd_t cmd);

            const std::string &getVideoSrcPath() const
            {
                return src_path_;
            }

        protected:

        private:
            std::string src_path_;

            lv_obj_t * player_;
        };

    } // heygears
} // widgets

#endif //LV_USE_FFMPEG
#endif //ULTRACORE_REFLEX_VIDEO_H
