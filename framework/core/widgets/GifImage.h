#ifndef LVGLPP_GIFIMAGE_H
#define LVGLPP_GIFIMAGE_H

#include "Widget.h"

#include <string>

namespace lvglpp::widgets {

class GifImage : public Widget
{
public:
    /// Load a GIF from the application assets (e.g. "gif/bird.gif").
    explicit GifImage(const std::string &assetPath, Widget *parent = nullptr);

    void setAsset(const std::string &assetPath);

    /// Compatibility name; now interpreted as an asset path.
    void setImageSrcPath(const std::string &assetPath)
    {
        setAsset(assetPath);
    }

    /// Set an explicit LVGL FS path, e.g. "A:/gif/bird.gif".
    void setFsPath(const std::string &lvglPath);

    const std::string &getImageSrcPath() const
    {
        return src_path_;
    }

private:
    void applyLvglPath(const std::string &lvglPath);

    std::string src_path_;
    std::string lvgl_path_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_GIFIMAGE_H
