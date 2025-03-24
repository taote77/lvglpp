/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-06-20

Class:${CLASS}

Description:

**************************************************************************/

#include "GifImage.h"

namespace heygears
{
    namespace widgets
    {
        GifImage::GifImage(const std::string &path, BaseItem *parent) : BaseItem(ItemType::GIF, parent)
        {
            setImageSrcPath(path);
        }

        void GifImage::setImageSrcPath(const std::string &path)
        {
            src_path_ = path;
            if (getLvglItem() != nullptr && !src_path_.empty())
            {
                lv_gif_set_src(getLvglItem(), src_path_.c_str());
            }
        }
    }
} // widgets