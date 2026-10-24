#ifndef GAME_STATE_DIALOG_H
#define GAME_STATE_DIALOG_H

#include "core/widgets/Dialog.h"
#include "core/widgets/Widget.h"
#include "core/widgets/Label.h"
#include "core/widgets/PushButton.h"

namespace lvglpp {

class GameStateDialog : public widgets::Dialog
{
public:
    explicit GameStateDialog();

    // ~GameStateDialog() override;

    void open() override;

private:
    std::shared_ptr<widgets::Widget> _bg_item;

    std::shared_ptr<lvglpp::widgets::Label> _score_lbl;

    std::shared_ptr<widgets::PushButton> _btn;
};

}; // namespace lvglpp

#endif // GAME_STATE_DIALOG_H