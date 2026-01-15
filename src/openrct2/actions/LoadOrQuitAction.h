/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Window.h"
#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    enum class LoadOrQuitModes : uint8_t
    {
        OpenSavePrompt,
        CloseSavePrompt
    };

    class LoadOrQuitAction final : public GameActionBase<GameCommand::LoadOrQuit>
    {
    private:
        LoadOrQuitModes _mode{};
        PromptMode _savePromptMode{ PromptMode::saveBeforeLoad };

    public:
        LoadOrQuitAction() = default;
        LoadOrQuitAction(LoadOrQuitModes mode, PromptMode savePromptMode = PromptMode::saveBeforeLoad);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
