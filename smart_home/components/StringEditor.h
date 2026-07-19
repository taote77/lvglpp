#ifndef STRING_VIEW_EDITOR_H
#define STRING_VIEW_EDITOR_H

#include "core/widgets/BaseItem.h"
#include <string>

namespace wrapper::widgets {
using namespace lvglpp::widgets;
class StringEditor : public BaseItem
{
public:
    explicit StringEditor(BaseItem *parent = nullptr);
    void addLetter(const std::string &letter)
    {
        if (max_len_ != -1 && text_.length() >= max_len_)
        {
            return;
        }
        text_ += letter;
        draw(text_);
    }
    void backSpace()
    {
        text_ = text_.substr(0, text_.length() - 1);
        draw(text_);
    }
    const std::string &getContent() const
    {
        return text_;
    }
    void setContent(const std::string &str)
    {
        if (max_len_ != -1 && str.length() > max_len_)
        {
            return;
        }
        text_ = str;
        draw(text_);
    }
    void setMaxLen(int n)
    {
        max_len_ = n;
    }

protected:
    virtual void draw(const std::string &display) = 0;

private:
    std::string text_;
    int         max_len_;
};

} // namespace wrapper::widgets

#endif // STRING_VIEW_EDITOR_H
