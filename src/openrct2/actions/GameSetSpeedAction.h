/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

namespace OpenRCT2::GameActions
{
    class GameSetSpeedAction final : public GameActionBase<GameCommand::SetGameSpeed>
    {
    private:
        uint8_t _speed{ 1 };

    public:
        GameSetSpeedAction() = default;
        GameSetSpeedAction(uint8_t speed);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        bool IsValidSpeed(uint8_t speed) const;
    };
} // namespace OpenRCT2::GameActions
