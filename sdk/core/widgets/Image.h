
#ifndef LVGLPP_IMAGE_H
#define LVGLPP_IMAGE_H

#include "BaseItem.h"
#include <string>

namespace lvglpp::widgets {

class Image : public BaseItem
{
public:
    explicit Image(const std::string &path, BaseItem *parent = nullptr);

    void setImageSrcPath(const std::string &path);

    const std::string &getImageSrcPath() const
    {
        return src_path_;
    }

    void setEnable(bool enable) override;

private:
    std::string src_path_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_IMAGE_H
