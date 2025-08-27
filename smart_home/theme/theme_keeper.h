#ifndef THEME_KEEPER_H
#define THEME_KEEPER_H


#include "theme/form_design.h"
#include <src/misc/lv_types.h>
#include <src/themes/lv_theme_private.h>
#include <string>

class ThemeKeeper
{
public:
    static ThemeKeeper &Instance();

    void SetTheme(const std::string &theme_name);

    lv_style_t &StyleTaskCard();

    lv_style_t &StyleCardGradu();

    lv_style_t &StyleButton();


protected:
    explicit ThemeKeeper();
    ~ThemeKeeper() = default;

    void init();

private:
    lv_theme_t _dark;

    lv_theme_t _shallow;


    lv_style_t _style_btn;

    lv_style_t _style_card_gradu;

    lv_style_t _style_task_card;
};


#endif// THEME_KEEPER_H