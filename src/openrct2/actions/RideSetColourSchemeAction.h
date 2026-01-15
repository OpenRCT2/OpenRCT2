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
    class RideSetColourSchemeAction final : public GameActionBase<GameCommand::SetColourScheme>
    {
    private:
        CoordsXYZD _loc;
        TrackElemType _trackType{};
        uint16_t _newColourScheme{};

    public:
        RideSetColourSchemeAction() = default;
        RideSetColourSchemeAction(const CoordsXYZD& location, TrackElemType trackType, uint16_t newColourScheme);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
