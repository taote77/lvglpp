
#include "trTranslateImpl.h"
#include "Utils.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace heygears {
namespace tools {
std::map<trTranslateImpl::LanguageType, trTranslateImpl *> trTranslateImpl::instance_;

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
    if (boost::filesystem::exists(asset_tr_file_path)) {
        auto all_content_str = Utils::readAllText(asset_tr_file_path);
        if (!all_content_str.empty()) {
            std::vector<std::string> translate_data_vec;
            boost::algorithm::split(translate_data_vec, all_content_str, boost::is_any_of("\n"),
                                    boost::token_compress_on);
            if (!translate_data_vec.empty()) {
                for (auto &it : translate_data_vec) {
                    if (it.empty()) {
                        continue;
                    }
                    auto index = it.find("->");
                    if (index != -1) {
                        std::string str_left = it.substr(0, index);
                        std::string str_right = it.substr(index + 2);
                        // boost::algorithm::trim(str_left);
                        boost::algorithm::replace_all(str_left, "\\n", "\n");
                        // boost::algorithm::trim(str_right);
                        boost::algorithm::replace_all(str_right, "\\n", "\n");
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
        auto p = new trTranslateImpl(type);
        instance_.insert(std::make_pair<>(type, p));
        return p;
    } else {
        return find_it->second;
    }
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
} // namespace heygears