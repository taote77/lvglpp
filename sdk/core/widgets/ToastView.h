#ifndef LVGLPP_TOASTVIEW_H
#define LVGLPP_TOASTVIEW_H

#include "Image.h"
#include "Label.h"
#include "ToastType.h"
#include "Widget.h"

#include <memory>
#include <string>

namespace lvglpp::widgets {

/// Internal presentational widget used by Toast. Not part of the public API:
/// include Toast.h instead and let the facade manage lifetime and animation.
class ToastView : public Widget
{
public:
    ToastView();

    void setContent(ToastType type, const std::string &message);

private:
    void applyTheme();

    std::unique_ptr<Image> icon_;
    std::unique_ptr<Label> desc_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TOASTVIEW_H
