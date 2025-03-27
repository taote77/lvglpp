#ifndef LV_GIF_IMAGE_H
#define LV_GIF_IMAGE_H

#include "BaseItem.h"

#include <string>

namespace lvglpp {
namespace widgets {

class GifImage : public BaseItem
{
public:
    explicit GifImage(const std::string &path, BaseItem *parent = nullptr);

    void setImageSrcPath(const std::string &path);

    const std::string &getImageSrcPath() const
    {
        return src_path_;
    }

protected:
private:
    std::string src_path_;
};

} // namespace widgets
} // namespace lvglpp

#endif // LV_GIF_IMAGE_H
