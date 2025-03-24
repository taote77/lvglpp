
#include <utility>
#include "ConsumeText.h"
#include "core/sys/Application.h"
#include "core/tools/Utils.h"
#include "core/tools/fonts/LvFontManager.h"

namespace heygears
{
    namespace widgets
    {
#if 0
        static const lv_ft_info_t &getFontByStyle(ConsumeText::TextStyle style, ConsumeText::FontType type)
        {
            tools::TrueFontManager::FontName font_type;
            if (type == ConsumeText::FontType::Auto)
            {
                if (heygears::sys::Application::getLanguageType() == sys::Application::English)
                {
                    font_type = tools::TrueFontManager::DMSans;
                } else
                {
                    font_type = tools::TrueFontManager::SansSC;
                }
            } else if (type == ConsumeText::FontType::DMSans)
            {
                font_type = tools::TrueFontManager::DMSans;
            } else if (type == ConsumeText::FontType::SansSC)
            {
                font_type = tools::TrueFontManager::SansSC;
            } else
            {
                font_type = tools::TrueFontManager::Oswald;
            }

            switch (style)
            {
                case ConsumeText::TextStyle::HeadingBold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 36, FT_FONT_STYLE_BOLD);
                case ConsumeText::TextStyle::Heading:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 36, FT_FONT_STYLE_NORMAL);
                case ConsumeText::TextStyle::SubheadingBold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 24, FT_FONT_STYLE_BOLD);
                case ConsumeText::TextStyle::Subheading:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 24, FT_FONT_STYLE_NORMAL);
                case ConsumeText::TextStyle::Subheading2Bold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 20, FT_FONT_STYLE_BOLD);
                case ConsumeText::TextStyle::FontSize20:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 20, FT_FONT_STYLE_NORMAL);
                case ConsumeText::TextStyle::ParagraphBold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 16, FT_FONT_STYLE_BOLD);
                case ConsumeText::TextStyle::Custom18:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type,18,FT_FONT_STYLE_NORMAL);
                case ConsumeText::TextStyle::Custom48Bold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type,48,FT_FONT_STYLE_BOLD);
                case ConsumeText::TextStyle::Custom32:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type,32,FT_FONT_STYLE_NORMAL);
                case ConsumeText::TextStyle::Custom32Bold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type,32,FT_FONT_STYLE_BOLD);
                case ConsumeText::TextStyle::Custom36Bold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type,36,FT_FONT_STYLE_BOLD);
                case ConsumeText::TextStyle::Custom36:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type,36,FT_FONT_STYLE_NORMAL);
                case ConsumeText::TextStyle::Custom72Bold:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type,72,FT_FONT_STYLE_BOLD);
                default:
                    return tools::TrueFontManager::getInstance()->getFontInfo(font_type, 16, FT_FONT_STYLE_NORMAL);
            }
        }
#else

        static const lv_font_t *getFontByStyle(ConsumeText::TextStyle style, ConsumeText::FontType type)
        {
            tools::LvFontManager::FontName font_type;
            if (type == ConsumeText::FontType::Auto)
            {
                if (heygears::sys::Application::getLanguageType() == sys::Application::Chinese)
                {
                    font_type = tools::LvFontManager::SansSC;
                }
                else
                {
                    font_type = tools::LvFontManager::DMSans;
                }
            }
            else if (type == ConsumeText::FontType::DMSans)
            {
                font_type = tools::LvFontManager::DMSans;
            }
            else if (type == ConsumeText::FontType::SansSC)
            {
                font_type = tools::LvFontManager::SansSC;
            }
            else
            {
                font_type = tools::LvFontManager::Oswald;
            }

            switch (style)
            {
                case ConsumeText::TextStyle::FontSize20:
                    return tools::LvFontManager::getFontInfo(font_type, 20, tools::LvFontManager::Normal);
                case ConsumeText::TextStyle::FontSize24:
                    return tools::LvFontManager::getFontInfo(font_type, 24, tools::LvFontManager::Normal);
                case ConsumeText::TextStyle::FontSize28:
                    return tools::LvFontManager::getFontInfo(font_type, 28, tools::LvFontManager::Normal);
                case ConsumeText::TextStyle::FontSize32:
                    return tools::LvFontManager::getFontInfo(font_type, 32, tools::LvFontManager::Normal);
                case ConsumeText::TextStyle::FontSize36:
                    return tools::LvFontManager::getFontInfo(font_type, 36, tools::LvFontManager::Normal);
                case ConsumeText::TextStyle::FontSize20Bold:
                    return tools::LvFontManager::getFontInfo(font_type, 20, tools::LvFontManager::Bold);
                case ConsumeText::TextStyle::FontSize24Bold:
                    return tools::LvFontManager::getFontInfo(font_type, 24, tools::LvFontManager::Bold);
                case ConsumeText::TextStyle::FontSize28Bold:
                    return tools::LvFontManager::getFontInfo(font_type, 28, tools::LvFontManager::Bold);
                case ConsumeText::TextStyle::FontSize32Bold:
                    return tools::LvFontManager::getFontInfo(font_type, 32, tools::LvFontManager::Bold);
                case ConsumeText::TextStyle::FontSize36Bold:
                    return tools::LvFontManager::getFontInfo(font_type, 36, tools::LvFontManager::Bold);
                case ConsumeText::TextStyle::FontSize64Bold:
                    return tools::LvFontManager::getFontInfo(font_type, 64, tools::LvFontManager::Bold);
                default:
                    return tools::LvFontManager::getFontInfo(font_type, 20, tools::LvFontManager::Normal);
            }
        }

//        void ConsumeText::setAligment(lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
//        {
//            if (heygears::sys::Application::getLanguageType() != sys::Application::Chinese2English ||
//                font_type_ == FontType::SansSC)
//            {
//                //中文时文字效果会偏下
//                offsetY += 5;
//            }
//            BaseItem::setAligment(align, offsetX, offsetY);
//        }
//
//        void ConsumeText::setAligmentTo(const BaseItem &item, lv_align_t align, lv_coord_t offsetX, lv_coord_t offsetY)
//        {
//            if (heygears::sys::Application::getLanguageType() != sys::Application::Chinese2English ||
//                font_type_ == FontType::SansSC)
//            {
//                //中文时文字效果会偏下
//                offsetY += 5;
//            }
//            BaseItem::setAligmentTo(item, align, offsetX, offsetY);
//        }

#endif

        static void setFontLineSpacing(lv_style_t *font_style_t, ConsumeText::TextStyle style)
        {
            LV_UNUSED(style);
            lv_style_set_text_line_space(font_style_t, 2);
            /*switch (style)
            {
                case ConsumeText::TextStyle::SubHeading1:
                    lv_style_set_text_line_space(font_style_t,36);
                    break;
                case ConsumeText::TextStyle::FontSize20:
                case ConsumeText::TextStyle::SubHeading2Bold:
                    lv_style_set_text_line_space(font_style_t,2);
                    break;
                case ConsumeText::TextStyle::Paragraph1:
                case ConsumeText::TextStyle::Paragraph1Bold:
                    lv_style_set_text_line_space(font_style_t,24);
                    break;
                default:
                    lv_style_set_text_line_space(font_style_t,36);
                    break;
            }*/
        }

        ConsumeText::ConsumeText(const std::string &strMsg, uint32_t clr, BaseItem *parent,
                                 ConsumeText::TextStyle style, FontType type) : BaseItem(BaseItem::Text, parent),
                                                                                text_(strMsg), max_len_(-1),
                                                                                text_style_(style),
                                                                                font_type_(type)
        {
            lv_style_init(&font_style_);
            //lv_style_set_text_font(&font_style_, getFontByStyle(style, type).font);
            lv_style_set_text_font(&font_style_, getFontByStyle(style, type));
            font_ = *getFontByStyle(style, type);
            lv_style_set_text_align(&font_style_, LV_TEXT_ALIGN_AUTO);
            setFontLineSpacing(&font_style_, style);
            setTextClr(clr);
            lv_obj_add_style(getLvglItem(), &font_style_, LV_STATE_DEFAULT);
            //lv_label_set_long_mode(label_lvgl_obj_, LV_LABEL_LONG_DOT);
            setText(strMsg);
        }

        void ConsumeText::setText(const std::string &strMsg)
        {
            text_ = strMsg;
            std::string str_show_text = strMsg;
            if (max_len_ > 8)
            {
                std::wstring tmp = heygears::tools::Utils::s2ws(str_show_text);
                auto n = tmp.length();
                if (n > max_len_)
                {
                    int split_left=max_len_/2;
                    int split_right=max_len_-split_left;
                    std::wstring str_left = tmp.substr(0, split_left);
                    std::wstring str_right = tmp.substr(tmp.length() - split_right, split_right);
                    tmp = str_left + heygears::tools::Utils::s2ws("...") + str_right;
                    str_show_text = heygears::tools::Utils::ws2s(tmp);
                }
            }
            lv_label_set_text(getLvglItem(), getLongModeStr(str_show_text).c_str());
        }

        void ConsumeText::setTextClr(uint32_t clr)
        {
            lv_style_set_text_color(&font_style_, lv_color_hex(clr));
        }

        void ConsumeText::setReColorEnable(bool enable)
        {
            lv_label_set_recolor(getLvglItem(), enable);
        }

        void ConsumeText::setMaxLen(int n)
        {
            max_len_ = n;
            setText(text_);
        }

//        void ConsumeText::setPos(lv_coord_t x, lv_coord_t y)
//        {
//            if (heygears::sys::Application::getLanguageType() != sys::Application::Chinese2English ||
//                font_type_ == FontType::SansSC)
//            {
//                //中文时文字效果会偏下
//                y += 6;
//            }
//            BaseItem::setPos(x, y);
//        }

        void ConsumeText::setTextAlign(lv_text_align_t align)
        {
            lv_style_set_text_align(&font_style_, align);
        }

        void ConsumeText::setEnable(bool enable)
        {
            BaseItem::setEnable(enable);
            if (enable)
            {
                lv_obj_set_ext_click_area(getLvglItem(), 15);
            }
        }

        int ConsumeText::getDisplayTextWidth() const
        {
            int res = 0;
            int res_tmp = 0;
            if(!text_.empty())
            {
                std::wstring wid_string=tools::Utils::s2ws(text_);
                for (int i = 0; i < wid_string.length(); i++)
                {
                    if(wid_string[i] == '\n')
                    {
                        res_tmp = res > res_tmp ? res : res_tmp;
                        res = 0;
                    }
                    if (i < wid_string.length() - 1)
                    {
                        res += lv_font_get_glyph_width(lv_obj_get_style_text_font(getLvglItem(), LV_STATE_DEFAULT),
                                                       wid_string[i], wid_string[i + 1]);
                    }
                    else
                    {
                        res += lv_font_get_glyph_width(lv_obj_get_style_text_font(getLvglItem(), LV_STATE_DEFAULT),
                                                       wid_string[i], 0);
                    }
                }
            }
            return res>res_tmp?res:res_tmp;
        }

        ConsumeText::~ConsumeText()
        {
            lv_style_reset(&font_style_);
        }

        void ConsumeText::setLongMode(int maxWidth, ConsumeText::LongMode longMode)
        {
            long_mode_.mode = longMode;
            long_mode_.width = maxWidth;
            setText(getText());
        }

        std::string ConsumeText::getLongModeStr(std::string src)
        {
            std::string out_str = std::move(src);

            if(out_str.empty() || long_mode_.mode == None || getDisplayTextWidth() <= long_mode_.width)
            {
                return out_str;
            }

            std::wstring src_str = heygears::tools::Utils::s2ws(getText());

            lv_coord_t letter_space = lv_obj_get_style_text_letter_space(getLvglItem(), LV_PART_MAIN);
            int real_txt_width = long_mode_.width - (lv_font_get_glyph_width(&font_, '.', '.') + letter_space) * 3;

            int src_text_width = 0;
            int last_char_index = -1;
            if(long_mode_.mode == LongMode::AutoEllipsis)
            {
                for (int i = 0; i < src_str.length(); i++)
                {
                    src_text_width += lv_font_get_glyph_width(lv_obj_get_style_text_font(getLvglItem(), LV_STATE_DEFAULT),
                                                              src_str[i], i < src_str.length() - 1 ? src_str[i + 1] : 0);
                    if (src_text_width > real_txt_width && i > 0)
                    {
                        last_char_index = i;
                        out_str = heygears::tools::Utils::ws2s(src_str.substr(0, last_char_index) + heygears::tools::Utils::s2ws("..."));
                        break;
                    }
                }
            }
            else if(long_mode_.mode == LongMode::MiddleEllipsis)
            {
                int src_text_width_font = 0;
                int src_text_width_end = 0;

                for (int i = 0; i < src_str.length(); i++)
                {
                    src_text_width_font += lv_font_get_glyph_width(lv_obj_get_style_text_font(getLvglItem(), LV_STATE_DEFAULT),
                                                                   src_str[i], i < src_str.length() - 1 ? src_str[i + 1] : 0);
                    if (src_text_width_font >= real_txt_width/2 && i > 0)
                    {
                        last_char_index = i - 1;
                        out_str = heygears::tools::Utils::ws2s(src_str.substr(0, last_char_index+1) + heygears::tools::Utils::s2ws("..."));
                        break;
                    }
                }

                for(int i = src_str.length() - 1; i > 0; i--)
                {
                    src_text_width_end += lv_font_get_glyph_width(lv_obj_get_style_text_font(getLvglItem(), LV_STATE_DEFAULT),
                                                                  src_str[i], i < src_str.length() - 1 ? src_str[i + 1] : 0);
                    if(src_text_width_end >= real_txt_width/2 )
                    {
                        last_char_index = i;
                        break;
                    }
                }

                if (src_text_width_end + src_text_width_font >= real_txt_width)
                {
                    last_char_index++;
                }
                out_str += heygears::tools::Utils::ws2s(src_str.substr(last_char_index, src_str.length() - last_char_index));
            }
            return out_str;
        }
    }
}
