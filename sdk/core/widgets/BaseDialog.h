#ifndef LVGLPP_BASEDIALOG_H
#define LVGLPP_BASEDIALOG_H

#include "Widget.h"
#include "core/base/Connection.h"
#include <cstdint>
#include <memory>

namespace lvglpp::widgets {

class BaseDialog
{
public:
    explicit BaseDialog(bool is_sys_dialog = false);

    virtual void open();

    virtual void close();

    bool isOpen();

    void setOnClicked(const std::function<void()> &func)
    {
        background_item_ptr_->setEnabled(true);
        background_item_ptr_->setOnClickedListener(func);
    }

    void setOpacity(double opacity)
    {
        background_item_ptr_->setOpacity(opacity);
    }

protected:
    Widget *getRoot() const;
    virtual ~BaseDialog();

private:
    std::unique_ptr<Widget> background_item_ptr_;
    std::uint64_t             focus_scope_ = 0;
    core::ScopedConnection    config_connection_;
};

} // namespace lvglpp::widgets

#endif // LVGLPP_BASEDIALOG_H
