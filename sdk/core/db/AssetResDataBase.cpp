#include "AssetResDataBase.h"
#include "core/log/log.h"
#include "core/tools/Utils.h"
#include <boost/format.hpp>

namespace heygears {
namespace db {
AssetResDataBase::AssetResDataBase() : db_storage_(nullptr)
{
    std::string db_path = heygears::tools::Utils::getAssetDirRoot() + "/db/resource.db";
    int result = sqlite3_open_v2(db_path.c_str(), &db_storage_, SQLITE_OPEN_READONLY, nullptr);
    if (result == SQLITE_OK) {
        LogDebug << "db path:" << db_path << " is opened succeed!";
    } else {
        LogWarn << "db path:" << db_path << " is opened failure!";
    }
}

boost::optional<AssetImageData> AssetResDataBase::getImageDataByUrl(const std::string &url)
{
    if (db_storage_ == nullptr) {
        return boost::none;
    }
    boost::format sql_fmt("select format,width,height,imageData,imageFileData from maker_resource "
                          "where url='%1%'");
    sql_fmt % url;
    sqlite3_stmt *stmt = nullptr;
    int result = sqlite3_prepare_v2(db_storage_, sql_fmt.str().c_str(), -1, &stmt, nullptr);
    if (result == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            AssetImageData reply_data;
            reply_data.format = (char *)sqlite3_column_text(stmt, 0);
            reply_data.width = sqlite3_column_int(stmt, 1);
            reply_data.height = sqlite3_column_int(stmt, 2);
            reply_data.imageData = (char *)sqlite3_column_text(stmt, 3);
            reply_data.imageFileData = (char *)sqlite3_column_text(stmt, 4);
            sqlite3_finalize(stmt);
            return reply_data;
        }
    }
    sqlite3_finalize(stmt);
    return boost::none;
}

AssetResDataBase::~AssetResDataBase()
{
    sqlite3_close_v2(db_storage_);
}
} // namespace db
} // namespace heygears
