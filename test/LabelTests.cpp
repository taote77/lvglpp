#include "TestFramework.h"
#include "core/widgets/Image.h"
#include "core/widgets/Label.h"
#include "core/widgets/Widget.h"
#include <lvgl.h>
#include <memory>
#include <string>

using namespace lvglpp::test;
using namespace lvglpp::widgets;

namespace {

std::string displayed(const Label &text)
{
    return text.lvObj() != nullptr ? lv_label_get_text(text.lvObj()) : std::string();
}

std::size_t codepointCount(const std::string &value)
{
    std::size_t count = 0;
    for (const char byte : value)
    {
        if ((static_cast<unsigned char>(byte) & 0xC0) != 0x80)
        {
            ++count;
        }
    }
    return count;
}

bool isValidUtf8(const std::string &value)
{
    std::size_t i = 0;
    while (i < value.size())
    {
        const unsigned char lead   = static_cast<unsigned char>(value[i]);
        int                 length = 0;
        if (lead < 0x80)
        {
            length = 1;
        } else if ((lead >> 5) == 0x6)
        {
            length = 2;
        } else if ((lead >> 4) == 0xE)
        {
            length = 3;
        } else if ((lead >> 3) == 0x1E)
        {
            length = 4;
        } else
        {
            return false;
        }

        if (i + static_cast<std::size_t>(length) > value.size())
        {
            return false;
        }
        for (int offset = 1; offset < length; ++offset)
        {
            if ((static_cast<unsigned char>(value[i + static_cast<std::size_t>(offset)]) & 0xC0) != 0x80)
            {
                return false;
            }
        }
        i += static_cast<std::size_t>(length);
    }
    return true;
}

} // namespace

TEST(Label_max_len_middle_ellipsis_ascii)
{
    Widget parent;
    Label  text("abcdefghij", 0, &parent);

    text.setMaxLength(8);
    CHECK_EQ(displayed(text), std::string("abc...ij"));
    CHECK_EQ(text.getText(), std::string("abcdefghij"));

    // Idempotent: rebuilding from the source text must not chain ellipses.
    text.setMaxLength(8);
    CHECK_EQ(displayed(text), std::string("abc...ij"));
    return true;
}

TEST(Label_max_len_small_budgets)
{
    Widget parent;
    Label  text("abcdefghij", 0, &parent);

    text.setMaxLength(3);
    CHECK_EQ(displayed(text), std::string("abc"));

    text.setMaxLength(1);
    CHECK_EQ(displayed(text), std::string("a"));

    text.setMaxLength(0);
    CHECK_EQ(displayed(text), std::string("abcdefghij"));
    return true;
}

TEST(Label_max_len_keeps_utf8_intact)
{
    Widget parent;
    Label  text("你好世界你好世界", 0, &parent);

    text.setMaxLength(5);
    const std::string shown = displayed(text);
    CHECK_TRUE(isValidUtf8(shown));
    CHECK_EQ(shown, std::string("你...界"));

    text.setMaxLength(3);
    CHECK_TRUE(isValidUtf8(displayed(text)));
    return true;
}

TEST(Label_max_len_keeps_combining_marks_attached)
{
    Widget parent;
    Label  text(u8"éx", 0, &parent); // e + combining acute, then x

    text.setMaxLength(1);
    CHECK_EQ(displayed(text), std::string(u8"é"));

    text.setMaxLength(2);
    CHECK_EQ(displayed(text), std::string(u8"éx"));
    return true;
}

TEST(Label_max_len_keeps_arabic_marks_attached)
{
    Widget parent;
    Label  text(u8"مُح", 0, &parent); // م + damma, then ح

    text.setMaxLength(1);
    const std::string shown = displayed(text);
    // LVGL's bidi/Arabic shaping stores presentation forms in the label, so
    // compare the cluster shape (base + damma) rather than the input bytes.
    CHECK_TRUE(isValidUtf8(shown));
    CHECK_EQ(codepointCount(shown), std::size_t{2});
    CHECK_EQ(shown.substr(shown.size() - 2), std::string(u8"ُ"));
    return true;
}

TEST(Label_pixel_auto_ellipsis)
{
    Widget parent;
    Label  text("The quick brown fox jumps over the lazy dog", 0, &parent);

    text.setLongMode(80, Label::AutoEllipsis);
    const std::string shown = displayed(text);
    CHECK_TRUE(shown.size() >= 3);
    CHECK_EQ(shown.substr(shown.size() - 3), std::string("..."));
    CHECK_TRUE(text.getDisplayTextWidth() <= 80);

    // Repeated calls must produce the same result.
    text.setLongMode(80, Label::AutoEllipsis);
    CHECK_EQ(displayed(text), shown);
    return true;
}

TEST(Label_pixel_middle_ellipsis)
{
    Widget parent;
    Label  text("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", 0, &parent);

    text.setLongMode(100, Label::MiddleEllipsis);
    const std::string shown = displayed(text);
    CHECK_TRUE(shown.find("...") != std::string::npos);
    CHECK_EQ(shown.front(), 'A');
    CHECK_EQ(shown.back(), '9');
    CHECK_TRUE(text.getDisplayTextWidth() <= 100);
    return true;
}

TEST(Label_pixel_limit_smaller_than_dots)
{
    Widget parent;
    Label  text("abcdefghij", 0, &parent);

    text.setLongMode(1, Label::AutoEllipsis);
    CHECK_TRUE(displayed(text).empty());

    text.setLongMode(1, Label::MiddleEllipsis);
    CHECK_TRUE(displayed(text).empty());
    return true;
}

TEST(Label_display_width_uses_widest_line)
{
    Widget parent;
    Label  text("short\nlongerline", 0, &parent);

    const int        width    = text.getDisplayTextWidth();
    const lv_font_t *font     = lv_obj_get_style_text_font(text.lvObj(), LV_PART_MAIN);
    const int        longLine = lv_text_get_width("longerline", 10, font, 0);
    CHECK_EQ(width, longLine);
    CHECK_TRUE(width > lv_text_get_width("short", 5, font, 0));
    return true;
}

TEST(Label_native_long_mode)
{
    Widget parent;
    Label  text("abcdefghij", 0, &parent);

    text.setLongMode(LV_LABEL_LONG_MODE_DOTS);
    CHECK_EQ(lv_label_get_long_mode(text.lvObj()), LV_LABEL_LONG_MODE_DOTS);

    text.setLongMode(LV_LABEL_LONG_MODE_WRAP);
    CHECK_EQ(lv_label_get_long_mode(text.lvObj()), LV_LABEL_LONG_MODE_WRAP);
    return true;
}

TEST(Label_set_text_reapplies_limits)
{
    Widget parent;
    Label  text("abcdefghij", 0, &parent);
    text.setMaxLength(8);

    text.setText("0123456789");
    CHECK_EQ(text.getText(), std::string("0123456789"));
    CHECK_EQ(displayed(text), std::string("012...89"));
    return true;
}

TEST(Label_recolor_disables_manual_truncation)
{
    Widget parent;
    Label  text("#FF0000 red# and blue", 0, &parent);

    text.setRecolorEnabled(true);
    text.setMaxLength(4);
    CHECK_EQ(displayed(text), std::string("#FF0000 red# and blue"));

    text.setRecolorEnabled(false);
    CHECK_EQ(displayed(text), std::string("#..."));
    return true;
}

TEST(Label_empty_text_is_safe)
{
    Widget parent;
    Label  text("", 0, &parent);

    text.setMaxLength(5);
    text.setLongMode(10, Label::AutoEllipsis);
    CHECK_TRUE(displayed(text).empty());
    CHECK_EQ(text.getDisplayTextWidth(), 0);
    return true;
}

TEST(Widget_enabled_state_does_not_touch_clickable)
{
    Widget widget;
    CHECK_TRUE(widget.isEnabled());

    const bool clickable = lv_obj_has_flag(widget.lvObj(), LV_OBJ_FLAG_CLICKABLE);
    widget.setEnabled(false);
    CHECK_FALSE(widget.isEnabled());
    CHECK_TRUE(lv_obj_has_state(widget.lvObj(), LV_STATE_DISABLED));
    CHECK_EQ(lv_obj_has_flag(widget.lvObj(), LV_OBJ_FLAG_CLICKABLE), clickable);

    widget.setEnabled(true);
    CHECK_TRUE(widget.isEnabled());
    CHECK_FALSE(lv_obj_has_state(widget.lvObj(), LV_STATE_DISABLED));
    return true;
}

TEST(Image_enable_makes_it_clickable)
{
    Widget parent;
    Image  image("", &parent);

    image.setEnabled(true);
    CHECK_TRUE(image.isEnabled());
    CHECK_TRUE(lv_obj_has_flag(image.lvObj(), LV_OBJ_FLAG_CLICKABLE));

    image.setEnabled(false);
    CHECK_FALSE(image.isEnabled());
    CHECK_FALSE(lv_obj_has_flag(image.lvObj(), LV_OBJ_FLAG_CLICKABLE));
    return true;
}

TEST(Widget_duplicate_adopt_tracks_all_wrappers)
{
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    CHECK_TRUE(obj != nullptr);

    {
        Widget first(Widget::AdoptExistingTag::Adopt, obj);
        Widget second(Widget::AdoptExistingTag::Adopt, obj);
        CHECK_TRUE(Widget::fromLvglObject(obj) == &second);
    }

    CHECK_TRUE(Widget::fromLvglObject(obj) == nullptr);
    lv_obj_delete(obj);
    return true;
}

TEST(Widget_external_delete_invalidates_all_wrappers)
{
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    CHECK_TRUE(obj != nullptr);

    Widget first(Widget::AdoptExistingTag::Adopt, obj);
    Widget second(Widget::AdoptExistingTag::Adopt, obj);
    lv_obj_delete(obj);

    CHECK_FALSE(first.isAttached());
    CHECK_FALSE(second.isAttached());
    return true;
}

TEST(Widget_delete_notification_tolerates_wrapper_destruction)
{
    lv_obj_t *obj = lv_obj_create(lv_scr_act());
    CHECK_TRUE(obj != nullptr);

    Widget                  first(Widget::AdoptExistingTag::Adopt, obj);
    std::unique_ptr<Widget> second = std::make_unique<Widget>(Widget::AdoptExistingTag::Adopt, obj);

    first.onDestroyed([&second]() { second.reset(); });
    lv_obj_delete(obj);

    CHECK_TRUE(second == nullptr);
    CHECK_FALSE(first.isAttached());
    return true;
}

TEST(Widget_describe_state_escapes_strings)
{
    Widget widget;
    widget.setName("a\"b\\c\td");

    const std::string json = widget.describeState();
    CHECK_TRUE(json.find("a\\\"b\\\\c\\td") != std::string::npos);
    return true;
}
