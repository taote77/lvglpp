/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-08-26

Class:${CLASS}

Description:

**************************************************************************/

#include "ImageManagerImpl.h"
#include "Utils.h"
#include <boost/filesystem.hpp>

namespace heygears {
namespace tools {
ImageManagerImpl *ImageManagerImpl::instance_ = nullptr;
ImageManagerImpl *ImageManagerImpl::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new ImageManagerImpl();
    }
    return instance_;
}

lv_img_dsc_t *ImageManagerImpl::getImageDscByUrl(const std::string &url)
{
    if (asset_db_ == nullptr) {
        return nullptr;
    }
    auto find_it = app_image_map_.find(url);
    if (find_it != app_image_map_.end()) {
        return &find_it->second;
    } else {
        auto data = asset_db_->getImageDataByUrl(url);
        if (data) {
            lv_img_dsc_t img_struct;
            img_struct.header.w = data->width;
            img_struct.header.h = data->height;
            img_struct.header.cf = LV_COLOR_FORMAT_ARGB8888;
            // img_struct.header.always_zero = 0;
            img_struct.header.reserved_2 = 0;
            img_struct.data_size = data->width * data->height * 4;
            std::string data_str;
            Utils::base64Decode(data->imageData, data_str);
            int len = std::max((uint32_t)data_str.length(), img_struct.data_size);
            auto *buffer = new unsigned char[len];
            memset(buffer, 0, len);
            memcpy(buffer, data_str.c_str(), data_str.length());
            img_struct.data = buffer;
            auto res = app_image_map_.insert(std::make_pair(url, img_struct));
            if (res.second) {
                return &res.first->second;
            }
        }
    }
    return nullptr;
}

std::string ImageManagerImpl::getImageFileDataByUrl(const std::string &url)
{
    if (asset_db_ == nullptr) {
        return {};
    }
    auto data = asset_db_->getImageDataByUrl(url);
    if (data) {
        return data->imageFileData;
    }
    return {};
}

ImageManagerImpl::ImageManagerImpl()
{
    if (boost::filesystem::exists(heygears::tools::Utils::getAssetDirRoot() + "/db/resource.db")) {
        asset_db_ = new db::AssetResDataBase();
    }
}

ImageManagerImpl::~ImageManagerImpl()
{
    delete asset_db_;
}
} // namespace tools
} // namespace heygears