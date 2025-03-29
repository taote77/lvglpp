#ifndef LVGLPP_TEXT_H
#define LVGLPP_TEXT_H

#include "BaseItem.h"
#include "lvgl.h"
#include <string>

namespace lvglpp::widgets {

class LvText : public BaseItem
{
public:
    enum TextStyle { FontSize20, FontSize24, FontSize28, FontSize32, FontSize36, FontSize20Bold, FontSize24Bold, FontSize28Bold, FontSize32Bold, FontSize36Bold, FontSize64Bold };

    enum FontType { Auto, DMSans, SansSC, Oswald };

    enum LongMode { None, AutoEllipsis, MiddleEllipsis };

    struct LongModeStruct {
        LongMode mode  = LongMode::None;
        int      width = 0;
    };

    LvText(const std::string &strMsg, uint32_t clr, BaseItem *parent, TextStyle style = TextStyle::FontSize20, FontType type = FontType::Auto);

    ~LvText() override;
    /**
     * 获取文本样式
     * @return
     */
    lv_style_t *getTextStyle()
    {
        return &font_style_;
    }
    /**
     * 设置文本内容
     * @param strMsg
     */
    void setText(const std::string &strMsg);

    const std::string &getText() const
    {
        return text_;
    }
    /**
     * 设置文本颜色
     * @param clr
     */
    void setTextClr(uint32_t clr);

    void setTextAlign(lv_text_align_t align);
    /**
     * 字体重渲染，用于局部文本显示不同的颜色
     * @param enable
     */
    void setReColorEnable(bool enable);
    /**
     * 设置文本显示的最大长度，过长时中间省略显示
     * @param n
     */
    void setMaxLen(int n);

    int getDisplayTextWidth() const;

    void setEnable(bool enable) override;

    //            void setPos(lv_coord_t x, lv_coord_t y) override;
    //
    //            void setAligment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
    //            override;
    //
    //            void setAligmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX,
    //            lv_coord_t offsetY) override;

    void setLongMode(int maxWidth, LongMode longMode);

private:
    std::string getLongModeStr(std::string src);

    lv_style_t     font_style_{};
    std::string    text_;
    int            max_len_;
    TextStyle      text_style_;
    FontType       font_type_;
    lv_font_t      font_;
    LongModeStruct long_mode_;
};
} // namespace lvglpp::widgets

#endif // LVGLPP_TEXT_H
