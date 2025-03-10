/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-07-05

Class:${CLASS}

Description:Asset资源数据库

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_ASSETRESDATABASE_H
#define ULTRACORE_FOR_MAKERS_ASSETRESDATABASE_H
#include <string>
#include <boost/optional.hpp>
#include <sqlite3.h>
namespace heygears
{
    namespace db
    {
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

//#define CREATE_ASSET_TABLE_FUNCTION make_storage(heygears::tools::Utils::getAssetDirRoot()+"/db/resource.db",\
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
            sqlite3* db_storage_;
        };

    } // heygears
} // db

#endif //ULTRACORE_FOR_MAKERS_ASSETRESDATABASE_H
