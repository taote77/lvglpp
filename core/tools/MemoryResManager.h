/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan eliang@heygears.com

Date:2022-09-02

Class:MemoryResManager

Description:

**************************************************************************/

#ifndef HGLV_MEMORYRESMANAGER_H
#define HGLV_MEMORYRESMANAGER_H
#include <map>
#include <vector>
#include <string>
namespace heygears
{
    namespace tools
    {
        struct ResourceInfo
        {
            std::string url;
            std::string data;
            int current_pos;

            ResourceInfo(const std::string& u,const std::string& d)
            {
                url=u;
                data=d;
                current_pos=0;
            }
        };
        class MemoryResManager
        {
        public:
            MemoryResManager(const MemoryResManager&)=delete;
            MemoryResManager operator=(const MemoryResManager&) = delete;
            static MemoryResManager* getInstance();
            void initResource(const std::map<std::string,std::string>& map);
            ResourceInfo* getResourceByUrl(const std::string& url);
            void freeResource(const ResourceInfo* info);
        private:
            MemoryResManager()=default;
            static MemoryResManager* instance_;
            std::map<std::string,std::string> res_map_;
            std::vector<ResourceInfo> open_res_vec_;
        };

    } // heygears
} // tools

#endif //HGLV_MEMORYRESMANAGER_H
