/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan eliang@heygears.com

Date:2022-09-02

Class:MemoryResManager

Description:

**************************************************************************/

#include "MemoryResManager.h"
#include "Utils.h"
namespace heygears
{
    namespace tools
    {
        MemoryResManager* MemoryResManager::instance_ = nullptr;
        MemoryResManager *MemoryResManager::getInstance()
        {
            if(instance_==nullptr)
            {
                instance_=new MemoryResManager();
            }
            return instance_;
        }

        void MemoryResManager::initResource(const std::map<std::string, std::string> &map)
        {
            res_map_=map;
        }

        ResourceInfo *MemoryResManager::getResourceByUrl(const std::string &url)
        {
            auto it=res_map_.find(url);
            if(it!=res_map_.end())
            {
                std::string image_data;
                if(Utils::base64Decode(it->second,image_data))
                {
                    open_res_vec_.emplace_back(url,image_data);
                    return &open_res_vec_.back();
                }
            }
            return nullptr;
        }

        void MemoryResManager::freeResource(const ResourceInfo *info)
        {
            for(auto it=open_res_vec_.begin();it!=open_res_vec_.end();it++)
            {
                if(it->url==info->url)
                {
                    open_res_vec_.erase(it);
                    break;
                }
            }
        }
    } // heygears
} // tools