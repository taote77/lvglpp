#ifndef ASSETRES_DATABASE_H
#define ASSETRES_DATABASE_H
#include <string>
#include <boost/optional.hpp>
#include <sqlite3.h>
namespace lvglpp {
namespace db {
/**
 * 图片资源结构体
 */
struct AssetImageData
{
    std::string url;
    std::string format;
    int width;
    int height;
    std::string imageData;
    std::string imageFileData;
};

//#define CREATE_ASSET_TABLE_FUNCTION make_storage(lvglpp::tools::Utils::getAssetDirRoot()+"/db/resource.db",\
//    make_table("maker_resource",\
//    make_column("url",&AssetImageData::url,primary_key()),\
//    make_column("format",&AssetImageData::format),\
//    make_column("width",&AssetImageData::width),\
//    make_column("height",&AssetImageData::height),\
//    make_column("imageData",&AssetImageData::imageData),                                                     \
//    make_column("imageFileData",&AssetImageData::imageFileData)))

class AssetResDataBase
{
public:
    AssetResDataBase();
    virtual ~AssetResDataBase();
    boost::optional<AssetImageData> getImageDataByUrl(const std::string &url);

private:
    sqlite3 *db_storage_;
};

} // namespace db
} // namespace lvglpp

#endif // ASSETRES_DATABASE_H
