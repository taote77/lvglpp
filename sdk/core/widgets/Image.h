/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-20

Class:Image

Description:png图片显示控件。大小位图片大小

**************************************************************************/

#ifndef LVGL_XFJ_DEMO_IMAGE_H
#define LVGL_XFJ_DEMO_IMAGE_H

#include "BaseItem.h"
#include <string>

namespace heygears
{
    namespace widgets
    {

        class Image : public BaseItem
        {
        public:
            explicit Image(const std::string& path, BaseItem *parent = nullptr);

            void setImageSrcPath(const std::string &path);

            const std::string &getImageSrcPath() const
            {
                return src_path_;
            }

            void setEnable(bool enable) override;

        protected:

        private:
            std::string src_path_;
        };

    }
} // widgets

#endif //LVGL_XFJ_DEMO_IMAGE_H
