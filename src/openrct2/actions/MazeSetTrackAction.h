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
    enum class MazeBuildMode : uint8_t
    {
        build,
        move,
        fill,
    };

    class MazeSetTrackAction final : public GameActionBase<GameCommand::SetMazeTrack>
    {
    private:
        CoordsXYZD _loc;
        bool _initialPlacement{};
        RideId _rideIndex{ RideId::GetNull() };
        MazeBuildMode _mode{};

    public:
        MazeSetTrackAction() = default;
        MazeSetTrackAction(const CoordsXYZD& location, bool initialPlacement, RideId rideIndex, MazeBuildMode mode);

        void AcceptParameters(GameActionParameterVisitor&) final;
        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        uint8_t MazeGetSegmentBit(const CoordsXY&) const;
    };
} // namespace OpenRCT2::GameActions
