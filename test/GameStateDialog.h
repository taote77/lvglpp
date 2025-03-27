#ifndef GAME_STATE_DIALOG_H
#define GAME_STATE_DIALOG_H

#include "core/widgets/BaseDialog.h"
#include "core/widgets/BaseItem.h"
#include "core/widgets/LvText.h"
#include "core/widgets/RoundedButton.h"

namespace lvglpp {

class GameStateDialog : public widgets::BaseDialog
{
public:
    explicit GameStateDialog();

    // ~GameStateDialog() override;

    void open() override;

private:
    std::shared_ptr<widgets::BaseItem> _bg_item;

    std::shared_ptr<lvglpp::widgets::LvText> _score_lbl;

    std::shared_ptr<widgets::RoundedButton> _btn;
};

}; // namespace lvglpp

#endif // GAME_STATE_DIALOG_H