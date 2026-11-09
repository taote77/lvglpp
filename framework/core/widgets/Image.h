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

/// Marks base64-encoded image bytes held in memory. The decoded image is
/// aspect-fitted and centered on a width x height transparent canvas.
struct Base64Source {
    std::string data;
    int         width  = 0;
    int         height = 0;
};

class Image : public Widget
{
public:
    /// Load an image from the application assets (AssetManager logical path).
    explicit Image(const std::string &assetPath, Widget *parent = nullptr);

    /// Load an image from a real filesystem path.
    explicit Image(FileSource source, Widget *parent = nullptr);

    /// Load an image from base64-encoded bytes.
    explicit Image(Base64Source source, Widget *parent = nullptr);

    /// Load from the application assets, e.g. "image/40x40/icon.png".
    void setAsset(const std::string &assetPath);

    /// Load from the real filesystem.
    void setFile(const std::string &fsPath);

    /// Load base64-encoded bytes into a width x height canvas.
    void setBase64(const std::string &data, int width, int height);

    /// Reload base64 data using the current target canvas size.
    void setBase64(const std::string &data);

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

    /// Asset or filesystem path; empty for base64-sourced images.
    const std::string &source() const
    {
        return src_path_;
    }

    bool isFileSource() const
    {
        return source_kind_ == SourceKind::File;
    }

    bool isBase64Source() const
    {
        return source_kind_ == SourceKind::Base64;
    }

    void setEnabled(bool enable) override;

private:
    enum class SourceKind { None, Asset, File, Base64 };

    struct EncodedState;

    void applySource(const std::string &path, bool fileSource);
    std::shared_ptr<EncodedState> applyBase64(const std::string &data, int width, int height);
    void clearBase64State();

    std::string        src_path_;
    std::string        rtl_asset_path_;
    tools::ImageHandle image_resource_;
    SourceKind         source_kind_ = SourceKind::None;

    std::string                 base64_data_;
    int                         base64_width_     = 0;
    int                         base64_height_    = 0;
    std::shared_ptr<EncodedState> encoded_state_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_IMAGE_H
