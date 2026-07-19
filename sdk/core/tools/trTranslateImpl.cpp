
#include "trTranslateImpl.h"
#include "Utils.h"
#include <filesystem>
#include <vector>
#include <iostream>
#include <sstream>

namespace lvglpp {
namespace tools {
std::map<trTranslateImpl::LanguageType, std::unique_ptr<trTranslateImpl>> trTranslateImpl::instance_;

trTranslateImpl::trTranslateImpl(LanguageType type)
{
    init(type);
}

void trTranslateImpl::init(LanguageType type)
{
    std::string asset_tr_file_path;
    if (type == LanguageType::English) {
        asset_tr_file_path = Utils::convertAssetResToGlobal("G:tr/english.trf");
    } else if (type == LanguageType::Chinese) {
        asset_tr_file_path = Utils::convertAssetResToGlobal("G:tr/chinese.trf");
    } else {
        return;
    }
    if (std::filesystem::exists(asset_tr_file_path)) {
        auto all_content_str = Utils::readAllText(asset_tr_file_path);
        if (!all_content_str.empty()) {
            // Split by newlines
            std::vector<std::string> translate_data_vec;
            std::istringstream iss(all_content_str);
            std::string line;
            while (std::getline(iss, line)) {
                if (!line.empty()) translate_data_vec.push_back(line);
            }
            if (!translate_data_vec.empty()) {
                for (auto &it : translate_data_vec) {
                    if (it.empty()) {
                        continue;
                    }
                    auto index = it.find("->");
                    if (index != std::string::npos) {
                        std::string str_left = it.substr(0, index);
                        std::string str_right = it.substr(index + 2);
                        // Replace escaped newlines
                        auto replaceNewlines = [](std::string &s) {
                            size_t pos = 0;
                            while ((pos = s.find("\\n", pos)) != std::string::npos) {
                                s.replace(pos, 2, "\n");
                                pos += 1;
                            }
                        };
                        replaceNewlines(str_left);
                        replaceNewlines(str_right);
                        translate_map_.insert(std::make_pair<>(str_left, str_right));
                    } else {
                        std::cerr << "-> error:" << it << std::endl;
                    }
                }
            }

            //                    for(const auto &it:translate_map_)
            //                    {
            //                        std::cout<<"key:"<<it.first<<",val:"<<it.second<<std::endl;
            //                    }
            std::cout << "map size:" << translate_map_.size() << std::endl;
        }
    }
}

trTranslateImpl *trTranslateImpl::getInstance(LanguageType type)
{
    auto find_it = instance_.find(type);
    if (find_it == instance_.end()) {
        auto [it, _] = instance_.emplace(type, std::make_unique<trTranslateImpl>(type));
        return it->second.get();
    }
    return find_it->second.get();
}

std::string trTranslateImpl::translate(const std::string &src_text)
{
    auto find_it = translate_map_.find(src_text);
    if (find_it == translate_map_.end() || find_it->second.empty()) {
        return src_text;
    }
    return find_it->second;
}
} // namespace tools
} // namespace lvglpp