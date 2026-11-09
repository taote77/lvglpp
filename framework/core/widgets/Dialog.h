#ifndef LVGLPP_DIALOG_H
#define LVGLPP_DIALOG_H

#include "OverlayManager.h"
#include "Widget.h"
#include "core/base/Connection.h"
#include <functional>
#include <memory>

namespace lvglpp::widgets {

/// Frame for modal dialogs. It owns a full-screen background widget and
/// manages the modal focus scope; subclasses populate the background through
/// getRoot().
class Dialog
{
public:
    /// Selects which LVGL layer hosts the background widget.
    enum class Layer { Application, System };

    explicit Dialog(Layer layer = Layer::Application);

    Dialog(const Dialog &)            = delete;
    Dialog &operator=(const Dialog &) = delete;
    Dialog(Dialog &&)                 = delete;
    Dialog &operator=(Dialog &&)      = delete;

    virtual ~Dialog();

    virtual void open();

    virtual void close();

    bool isOpen() const;

    /// Override the default priority (Dialog for Application layer,
    /// SystemDialog for System layer). Same-layer ordering only.
    void setPriority(OverlayManager::Priority priority);

    OverlayManager::Priority priority() const
    {
        return priority_;
    }

    /// Invoked when the dialog background (outside the content) is clicked.
    void setOnBackgroundClicked(const std::function<void()> &func);

    [[deprecated("use setOnBackgroundClicked()")]]
    void setOnClicked(const std::function<void()> &func)
    {
        setOnBackgroundClicked(func);
    }

    void setBackgroundOpacity(double opacity);

    [[deprecated("use setBackgroundOpacity()")]]
    void setOpacity(double opacity)
    {
        setBackgroundOpacity(opacity);
    }

protected:
    /// Root of the dialog content (the full-screen background widget).
    Widget *getRoot() const;

private:
    std::unique_ptr<Widget>  background_;
    OverlayManager::Token    overlay_;
    OverlayManager::Priority priority_ = OverlayManager::Priority::Dialog;
    core::ScopedConnection   config_connection_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_DIALOG_H
