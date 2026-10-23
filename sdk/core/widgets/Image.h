#ifndef LVGLPP_IMAGE_H
#define LVGLPP_IMAGE_H

#include "Widget.h"
#include "core/tools/ImageResource.h"
#include <memory>
#include <string>

namespace lvglpp::widgets {

/// Marks a real filesystem path (as opposed to an application asset path).
struct FileSource {
    std::string path;
};

class Image : public Widget
{
public:
    /// Load an image from the application assets (AssetManager logical path).
    explicit Image(const std::string &assetPath, Widget *parent = nullptr);

    /// Load an image from a real filesystem path.
    explicit Image(FileSource source, Widget *parent = nullptr);

    /// Load from the application assets, e.g. "image/40x40/icon.png".
    void setAsset(const std::string &assetPath);

    /// Load from the real filesystem.
    void setFile(const std::string &fsPath);

    /// Optional RTL alternative asset. When the UI direction is RTL this
    /// replaces the current source (e.g. a mirrored arrow).
    void setRtlAsset(const std::string &assetPath);

    /// Drop the cached entry and load the current source again.
    void reload();

    /// Compatibility name; now always interpreted as an asset path.
    void setImageSrcPath(const std::string &assetPath)
    {
        setAsset(assetPath);
    }

    const std::string &getImageSrcPath() const
    {
        return src_path_;
    }

    const std::string &source() const
    {
        return src_path_;
    }

    bool isFileSource() const
    {
        return file_source_;
    }

    void setEnabled(bool enable) override;

private:
    void applySource(const std::string &path, bool fileSource);

    std::string        src_path_;
    std::string        rtl_asset_path_;
    tools::ImageHandle image_resource_;
    bool               file_source_ = false;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_IMAGE_H
