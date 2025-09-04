#ifndef LV_QR_CODE_H
#define LV_QR_CODE_H

#include "BaseItem.h"
#include "Image.h"
#include <memory>

namespace lvglpp::widgets {

class QrCode : public BaseItem
{
public:
    explicit QrCode(uint32_t length, lv_color_t back_color, lv_color_t front_color, BaseItem *parent = nullptr);

    ~QrCode();

    void setData(const std::string &data);

    void setImgSrc(const std::string &src);

private:
    lv_obj_t *_qr;

    std::shared_ptr<widgets::Image> _img_icon;
    uint32_t                        _length = 0;
};

} // namespace lvglpp::widgets

#endif // LV_QR_CODE_H
