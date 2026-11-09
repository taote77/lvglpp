#ifndef LVGLPP_TOASTTYPE_H
#define LVGLPP_TOASTTYPE_H

namespace lvglpp::widgets {

/// Semantic variant of a toast message; selects the icon and is a natural
/// extension point for per-type colours.
enum class ToastType {
    Success,
    Warning,
    Error,
};

} // namespace lvglpp::widgets

#endif // LVGLPP_TOASTTYPE_H
