/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LoadOrQuitAction.h"

#include "../Context.h"
#include "../OpenRCT2.h"

LoadOrQuitAction::LoadOrQuitAction(LoadOrQuitModes mode, PromptMode savePromptMode)
    : _mode(mode)
    , _savePromptMode(savePromptMode)
{
}

uint16_t LoadOrQuitAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::ClientOnly | GameActions::Flags::AllowWhilePaused;
}

void LoadOrQuitAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_mode) << DS_TAG(_savePromptMode);
}

GameActions::Result LoadOrQuitAction::Query() const
{
    return GameActions::Result();
}

GameActions::Result LoadOrQuitAction::Execute() const
{
    auto mode = static_cast<LoadOrQuitModes>(_mode);
    switch (mode)
    {
        case LoadOrQuitModes::OpenSavePrompt:
            gSavePromptMode = _savePromptMode;
            context_open_window(WindowClass::SavePrompt);
            break;
        case LoadOrQuitModes::CloseSavePrompt:
            window_close_by_class(WindowClass::SavePrompt);
            break;
        default:
            game_load_or_quit_no_save_prompt();
            break;
    }
    return GameActions::Result();
}
