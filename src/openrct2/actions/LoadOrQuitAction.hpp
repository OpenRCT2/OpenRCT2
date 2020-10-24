/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../OpenRCT2.h"
#include "GameAction.h"

enum class LoadOrQuitModes : uint8_t
{
    OpenSavePrompt,
    CloseSavePrompt
};

DEFINE_GAME_ACTION(LoadOrQuitAction, GAME_COMMAND_LOAD_OR_QUIT, GameActionResult)
{
private:
    uint8_t _mode{ 0 };
    uint8_t _savePromptMode{ PM_SAVE_BEFORE_LOAD };

public:
    LoadOrQuitAction()
    {
    }

    LoadOrQuitAction(LoadOrQuitModes mode, uint8_t savePromptMode = PM_SAVE_BEFORE_LOAD)
        : _mode(static_cast<uint8_t>(mode))
        , _savePromptMode(savePromptMode)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::CLIENT_ONLY | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_mode) << DS_TAG(_savePromptMode);
    }

    GameActionResult::Ptr Query() const override
    {
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        auto mode = static_cast<LoadOrQuitModes>(_mode);
        switch (mode)
        {
            case LoadOrQuitModes::OpenSavePrompt:
                gSavePromptMode = _savePromptMode;
                context_open_window(WC_SAVE_PROMPT);
                break;
            case LoadOrQuitModes::CloseSavePrompt:
                window_close_by_class(WC_SAVE_PROMPT);
                break;
            default:
                game_load_or_quit_no_save_prompt();
                break;
        }
        return std::make_unique<GameActionResult>();
    }
};
