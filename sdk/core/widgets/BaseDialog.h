#ifndef LV_BASE_DIALOG_H
#define LV_BASE_DIALOG_H

#include "BaseItem.h"
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
        background_item_ptr_->setEnable(true);
        background_item_ptr_->setOnClickedListener(func);
    }

    void setOpacity(double opacity)
    {
        background_item_ptr_->setOpacity(opacity);
    }

protected:
    BaseItem *getRoot() const;
    virtual ~BaseDialog() = default;

private:
    std::unique_ptr<BaseItem> background_item_ptr_;
};

} // namespace lvglpp::widgets

#endif // LV_BASE_DIALOG_H
