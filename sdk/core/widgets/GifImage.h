#ifndef LV_GIF_IMAGE_H
#define LV_GIF_IMAGE_H

#include "BaseItem.h"

#include <string>

namespace lvglpp::widgets {

class GifImage : public BaseItem
{
public:
    explicit GifImage(const std::string &path, BaseItem *parent = nullptr);

    void setImageSrcPath(const std::string &path);

    const std::string &getImageSrcPath() const
    {
        return src_path_;
    }

private:
    std::string src_path_;
};

} // namespace lvglpp::widgets

#endif // LV_GIF_IMAGE_H
