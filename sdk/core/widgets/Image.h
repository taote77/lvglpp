
#ifndef LVPP_IMAGE_H
#define LVPP_IMAGE_H

#include "BaseItem.h"
#include <string>

namespace heygears {
namespace widgets {

class Image : public BaseItem
{
public:
    explicit Image(const std::string &path, BaseItem *parent = nullptr);

    void setImageSrcPath(const std::string &path);

    const std::string &getImageSrcPath() const { return src_path_; }

    void setEnable(bool enable) override;

protected:
private:
    std::string src_path_;
};

} // namespace widgets
} // namespace heygears

#endif // LVPP_IMAGE_H
