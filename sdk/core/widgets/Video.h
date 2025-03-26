#ifndef LV_VIDEO_H
#define LV_VIDEO_H

#if LV_USE_FFMPEG == 1

#  include "BaseItem.h"
#  include <string>

namespace heygears {
namespace widgets {

class Video : public BaseItem
{
public:
    explicit Video(const std::string &path, BaseItem *parent = nullptr);

    void setVideoSrcPath(const std::string &path);

    void setVideoAutoRestart(bool b);

    void setVideoCmd(lv_ffmpeg_player_cmd_t cmd);

    const std::string &getVideoSrcPath() const { return src_path_; }

    void setVisible(bool visible) override;

    ~Video() override;

protected:
private:
    std::string src_path_;

    bool visible_ = true;

    lv_timer_t *lv_time_update_visible_ = nullptr;
};

} // namespace widgets
} // namespace heygears

#endif // LV_USE_FFMPEG
#endif // LV_VIDEO_H
