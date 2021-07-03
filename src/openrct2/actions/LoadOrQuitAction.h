/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class LoadOrQuitModes : uint8_t
{
    OpenSavePrompt,
    CloseSavePrompt
};

DEFINE_GAME_ACTION(LoadOrQuitAction, GameCommand::LoadOrQuit, GameActions::Result)
{
private:
    LoadOrQuitModes _mode{};
    PromptMode _savePromptMode{ PromptMode::SaveBeforeLoad };

public:
    LoadOrQuitAction() = default;
    LoadOrQuitAction(LoadOrQuitModes mode, PromptMode savePromptMode = PromptMode::SaveBeforeLoad);

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
