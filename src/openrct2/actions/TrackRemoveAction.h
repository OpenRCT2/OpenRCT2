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
    class TrackRemoveAction final : public GameActionBase<GameCommand::RemoveTrack>
    {
    private:
        TrackElemType _trackType{};
        int32_t _sequence{};
        CoordsXYZD _origin;

    public:
        TrackRemoveAction() = default;
        TrackRemoveAction(TrackElemType trackType, int32_t sequence, const CoordsXYZD& origin);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const final;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
