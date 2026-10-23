
#ifndef LVGLPP_TOASTIMPL_H
#define LVGLPP_TOASTIMPL_H

#include "Widget.h"
#include "Image.h"
#include "Label.h"
#include <memory>

namespace lvglpp::widgets {

class ToastImpl : public Widget
{
public:
    enum IconType { Succeed, Error, Warn };
    explicit ToastImpl();
    void setMessage(const std::string &str_msg, IconType type);

private:
    std::unique_ptr<widgets::Image> icon_;
    std::unique_ptr<Label>         desc_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TOASTIMPL_H
