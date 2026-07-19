#ifndef LVGLPP_GIFIMAGE_H
#define LVGLPP_GIFIMAGE_H

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

#endif // LVGLPP_GIFIMAGE_H
