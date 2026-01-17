
#ifndef LV_TOAST_IMPL_H
#define LV_TOAST_IMPL_H

#include "BaseItem.h"
#include "Image.h"
#include "LvText.h"
#include <memory>

namespace lvglpp::widgets {

class ToastImpl : public BaseItem
{
public:
    enum IconType { Succeed, Error, Warn };
    explicit ToastImpl();
    void setMessage(const std::string &str_msg, IconType type);

private:
    std::unique_ptr<widgets::Image> icon_;
    std::unique_ptr<LvText>         desc_;
};

} // namespace lvglpp::widgets

#endif // LV_TOAST_IMPL_H
