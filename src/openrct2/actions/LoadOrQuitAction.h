/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
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

class LoadOrQuitAction final : public GameActionBase<GameCommand::LoadOrQuit>
{
private:
    LoadOrQuitModes _mode{};
    PromptMode _savePromptMode{ PromptMode::SaveBeforeLoad };

public:
    LoadOrQuitAction() = default;
    LoadOrQuitAction(LoadOrQuitModes mode, PromptMode savePromptMode = PromptMode::SaveBeforeLoad);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;
};
