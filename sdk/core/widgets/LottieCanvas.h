
#ifndef LVGLPP_LOTTIECANVAS_H
#define LVGLPP_LOTTIECANVAS_H

#include "BaseItem.h"
#include <string>

namespace lvglpp::widgets {

class LottieCanvas : public BaseItem
{
public:
    explicit LottieCanvas(const std::string &path, BaseItem *parent = nullptr);

    void setImageSrcPath(const std::string &path);

    void setCacheSize(int32_t width, int32_t height);

    const std::string &getImageSrcPath() const
    {
        return src_path_;
    }

private:
    std::string          src_path_;
    std::vector<uint8_t> cache_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_LOTTIECANVAS_H
