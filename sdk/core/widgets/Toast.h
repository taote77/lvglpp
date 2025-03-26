
#ifndef LV_TOAST_H
#define LV_TOAST_H
#include "BaseItem.h"
#include "Image.h"
#include "LvText.h"
#include <memory>

namespace heygears {
namespace widgets {
class Toast : public BaseItem
{
public:
    enum IconType { Succeed, Error, Warn };
    explicit Toast();
    void setMessage(const std::string &str_msg, IconType type);

private:
    std::shared_ptr<widgets::Image> icon_;
    std::shared_ptr<LvText> desc_;
};

} // namespace widgets
} // namespace heygears

#endif // LV_TOAST_H
