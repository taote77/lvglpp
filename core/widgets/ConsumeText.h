/**************************************************************************

Copyright:Copyright © 2022 HeyGears. All rights reserved

Author: LnJan

Date:2022-04-18

Class:ConsumeText

Description:消费级上位机统一Text控件

**************************************************************************/

#ifndef LVGL_XFJ_DEMO_CONSUMETEXT_H
#define LVGL_XFJ_DEMO_CONSUMETEXT_H

#include "lvgl.h"
#include "BaseItem.h"
#include <string>

namespace heygears
{
    namespace widgets
    {
        class ConsumeText : public BaseItem
        {
        public:
            enum TextStyle
            {
                Heading,
                HeadingBold,
                Subheading,
                SubheadingBold,
                Subheading2,
                Subheading2Bold,
                Paragraph,
                ParagraphBold,
                Custom18,
                Custom48Bold,
                Custom32,
                Custom32Bold,
                Custom64Bold,
                Custom72Bold,
                Custom120Bold,
            };

            enum FontType
            {
                Auto,
                DMSans,
                SansSC,
                Oswald
            };

            enum LongMode
            {
                None,
                AutoEllipsis,
                MiddleEllipsis
            };

            struct LongModeStruct
            {
                LongMode mode = LongMode::None;
                int width = 0;
            };

            ConsumeText(const std::string &strMsg, uint32_t clr, BaseItem *parent,
                        TextStyle style = TextStyle::Paragraph, FontType type = FontType::Auto);

            ~ConsumeText() override;
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

            const std::string& getText() const
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

            void setPos(lv_coord_t x, lv_coord_t y) override;

            void setAligment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY) override;

            void setAligmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY) override;

            void setLongMode(int maxWidth, LongMode longMode);

        private:
            std::string getLongModeStr(std::string src);


            lv_style_t font_style_{};
            std::string text_;
            int max_len_;
            TextStyle text_style_;
            FontType font_type_;
            lv_font_t font_;
            LongModeStruct long_mode_;
        };
    }
}


#endif //LVGL_XFJ_DEMO_CONSUMETEXT_H
