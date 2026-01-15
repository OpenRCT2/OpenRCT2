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
    class TrackSetBrakeSpeedAction final : public GameActionBase<GameCommand::SetBrakesSpeed>
    {
    private:
        CoordsXYZ _loc;
        TrackElemType _trackType{};
        uint8_t _brakeSpeed{};

    public:
        TrackSetBrakeSpeedAction() = default;
        TrackSetBrakeSpeedAction(const CoordsXYZ& loc, TrackElemType trackType, uint8_t brakeSpeed);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const final;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        Result QueryExecute(bool isExecuting) const;
    };
} // namespace OpenRCT2::GameActions
