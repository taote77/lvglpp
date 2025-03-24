
#ifndef ULTRACORE_FOR_MAKERS_BASE64IMAGE_H
#define ULTRACORE_FOR_MAKERS_BASE64IMAGE_H
#include "BaseItem.h"
#include <string>
namespace heygears
{
    namespace widgets
    {
        class Base64Image:public BaseItem
        {
        public:
            explicit Base64Image(std::string  base64,int w,int h,BaseItem* parent);
            ~Base64Image() override;
            void updateImage(const std::string& data);

        private:
            lv_image_dsc_t img_struct_{};
            int w_;
            int h_;
            std::string base64Data_;
        };

    } // heygears
} // widgets

#endif //ULTRACORE_FOR_MAKERS_BASE64IMAGE_H
