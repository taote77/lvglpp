/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-06-20

Class:${CLASS}

Description:

**************************************************************************/

#ifndef ULTRACORE_FOR_MAKERS_GIFIMAGE_H
#define ULTRACORE_FOR_MAKERS_GIFIMAGE_H

#include "BaseItem.h"

#include <string>

namespace heygears
{
    namespace widgets
    {

        class GifImage : public BaseItem
        {
        public:
            explicit GifImage(const std::string &path, BaseItem *parent = nullptr);

            void setImageSrcPath(const std::string &path);

            const std::string &getImageSrcPath() const
            {
                return src_path_;
            }

        protected:

        private:
            std::string src_path_;
        };

    }
} // widgets

#endif //ULTRACORE_FOR_MAKERS_GIFIMAGE_H
