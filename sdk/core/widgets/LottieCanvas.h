#ifndef LVGLPP_LOTTIECANVAS_H
#define LVGLPP_LOTTIECANVAS_H

#include "BaseItem.h"
#include <string>
#include <vector>

namespace lvglpp::widgets {

class LottieCanvas : public BaseItem
{
public:
    /// Load a Lottie JSON from the application assets.
    explicit LottieCanvas(const std::string &assetPath, BaseItem *parent = nullptr);

    void setAsset(const std::string &assetPath);

    /// Compatibility name; now interpreted as an asset path.
    void setImageSrcPath(const std::string &assetPath)
    {
        setAsset(assetPath);
    }

    /// Load a Lottie JSON from the real filesystem (ThorVG stdio path).
    void setFile(const std::string &fsPath);

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
