#ifndef NOTICE_DELEGATE_H
#define NOTICE_DELEGATE_H

#include "core/widgets/Widget.h"
#include "core/widgets/Label.h"
#include "core/widgets/Image.h"
#include "core/widgets/ListViewDelegate.hpp"
#include <memory>
#include <utility>

namespace lvglpp::ui::lDelegate {

using namespace lvglpp;

using namespace lvglpp::widgets;

struct NoticeData {
    enum Type {
        Type_Info,
        Type_Warning,
        Type_Error,
    };
    Type        type;
    std::string message;

    NoticeData(Type type, std::string message) : type(type), message(std::move(message))
    {}
};

class NoticeDelegate : public ListViewDelegate<NoticeData>
{
public:
    explicit NoticeDelegate(int index, Widget *parent = nullptr);

protected:
    void draw(const NoticeData &data) override;

private:
    std::shared_ptr<Widget> _bg_item;

    std::shared_ptr<widgets::Image> _head_icon;
    std::shared_ptr<Label>    _head_date_label;
    std::shared_ptr<Label>    _msg_label;
};

} // namespace lvglpp::ui::lDelegate

#endif // NOTICE_DELEGATE_H