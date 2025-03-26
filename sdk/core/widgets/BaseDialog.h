#ifndef LV_BASE_DIALOG_H
#define LV_BASE_DIALOG_H

#include "BaseItem.h"
#include <memory>

namespace lvglpp {
namespace widgets {

class BaseDialog
{
public:
    explicit BaseDialog(bool is_sys_dialog = false);
    virtual void open();
    virtual void close();
    bool isOpen();
    void setAutoDel(bool b) { auto_del_ = b; }
    bool getAutoDel() const { return auto_del_; }

    void setOnClicked(const std::function<void()> &func)
    {
        background_item_ptr_->setEnable(true);
        background_item_ptr_->setOnClickedListener(func);
    }

    void setOpacity(double opacity) { background_item_ptr_->setOpacity(opacity); }

protected:
    BaseItem *getRootItem() const;
    virtual ~BaseDialog() = default;

private:
    std::shared_ptr<BaseItem> background_item_ptr_;
    bool auto_del_ = true;
};

} // namespace widgets
} // namespace lvglpp

#endif // LV_BASE_DIALOG_H
