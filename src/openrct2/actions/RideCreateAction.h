/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    class RideCreateAction final : public GameActionBase<GameCommand::CreateRide>
    {
    private:
        ride_type_t _rideType{ kRideTypeNull };
        ObjectEntryIndex _subType{ kObjectEntryIndexNull };
        ObjectEntryIndex _entranceObjectIndex{ kObjectEntryIndexNull };
        colour_t _colour1{ COLOUR_NULL };
        colour_t _colour2{ COLOUR_NULL };

    public:
        RideCreateAction() = default;
        RideCreateAction(
            ride_type_t rideType, ObjectEntryIndex subType, colour_t colour1, colour_t colour2,
            ObjectEntryIndex entranceStyleIndex);

        void AcceptParameters(GameActionParameterVisitor&) final;

        ride_type_t GetRideType() const;
        ObjectEntryIndex GetRideObject() const;
        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
