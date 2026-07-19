#include "AssetResDataBase.h"
#include "core/log/log.h"
#include "core/tools/Utils.h"
#include <iostream>

namespace lvglpp {
namespace db {
AssetResDataBase::AssetResDataBase() : db_storage_(nullptr)
{
    std::string db_path = lvglpp::tools::Utils::getAssetDirRoot() + "/db/resource.db";
    int         result  = sqlite3_open_v2(db_path.c_str(), &db_storage_, SQLITE_OPEN_READONLY, nullptr);
    if (result == SQLITE_OK)
    {
        LogDebug << "db path:" << db_path << " is opened succeed!";
    } else
    {
        LogWarn << "db path:" << db_path << " is opened failure!";
    }
}

std::optional<AssetImageData> AssetResDataBase::getImageDataByUrl(const std::string &url)
{
    if (db_storage_ == nullptr)
    {
        return std::nullopt;
    }
    std::string sql_fmt = "select format,width,height,imageData from images where url='" + url + "'";
    sqlite3_stmt *stmt   = nullptr;
    int           result = sqlite3_prepare_v2(db_storage_, sql_fmt.c_str(), -1, &stmt, nullptr);
    if (result == SQLITE_OK)
    {
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            AssetImageData reply_data;
            reply_data.format    = (char *)sqlite3_column_text(stmt, 0);
            reply_data.width     = sqlite3_column_int(stmt, 1);
            reply_data.height    = sqlite3_column_int(stmt, 2);
            reply_data.imageData = (char *)sqlite3_column_text(stmt, 3);
            // reply_data.metaData  = (char *)sqlite3_column_text(stmt, 4);
            sqlite3_finalize(stmt);

            std::cout << "getImageDataByUrl: " << reply_data.imageData.size() << std::endl;
            return reply_data;
        }
    } else
    {
        LogWarn << "getImageDataByUrl error: " << result << sqlite3_errmsg(db_storage_);
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

AssetResDataBase::~AssetResDataBase()
{
    sqlite3_close_v2(db_storage_);
}
} // namespace db
} // namespace lvglpp
