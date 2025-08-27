#ifndef NOTICE_DELEGATE_H
#define NOTICE_DELEGATE_H

#include "core/widgets/BaseItem.h"
#include "core/widgets/ConsumeText.h"
#include "core/widgets/Image.h"
#include "core/widgets/ListViewDelegate.hpp"
#include <boost/signals2/signal.hpp>
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
    explicit NoticeDelegate(int index, BaseItem *parent = nullptr);

protected:
    void draw(const NoticeData &data) override;

private:
    std::shared_ptr<BaseItem> _bg_item;

    std::shared_ptr<widgets::Image> _head_icon;
    std::shared_ptr<ConsumeText>    _head_date_label;
    std::shared_ptr<ConsumeText>    _msg_label;
};

} // namespace lvglpp::ui::lDelegate

#endif // NOTICE_DELEGATE_H