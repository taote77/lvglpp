#ifndef ASSETRES_DATABASE_H
#define ASSETRES_DATABASE_H
#include <sqlite3.h>
#include <optional>
#include <string>
namespace lvglpp {
namespace db {
/**
 * 图片资源结构体
 */
struct AssetImageData {
    std::string url;
    std::string format;
    int         width;
    int         height;
    std::string imageData;
    std::string metaData;
};

class AssetResDataBase
{
public:
    AssetResDataBase();
    virtual ~AssetResDataBase();
    std::optional<AssetImageData> getImageDataByUrl(const std::string &url);

private:
    sqlite3 *db_storage_;
};

} // namespace db
} // namespace lvglpp

#endif // ASSETRES_DATABASE_H
