#ifndef LV_QR_CODE_H
#define LV_QR_CODE_H

#include "BaseItem.h"
#include <memory>

namespace lvglpp {
namespace widgets {

class QrCode : public BaseItem
{
public:
    explicit QrCode(uint32_t length, lv_color_t back_color, lv_color_t front_color, BaseItem *parent = nullptr);

    ~QrCode();

    void setData(const std::string &data);

private:
    lv_obj_t *_qr;
};

} // namespace widgets
} // namespace lvglpp

#endif // LV_QR_CODE_H
