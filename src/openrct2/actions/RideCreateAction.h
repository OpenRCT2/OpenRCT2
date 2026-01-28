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
    class RideCreateAction final : public GameActionBase<GameCommand::CreateRide>
    {
    private:
        ride_type_t _rideType{ kRideTypeNull };
        ObjectEntryIndex _subType{ kObjectEntryIndexNull };
        ObjectEntryIndex _entranceObjectIndex{ kObjectEntryIndexNull };
        uint8_t _trackColourPreset{ std::numeric_limits<uint8_t>::max() };
        uint8_t _vehicleColourPreset{ std::numeric_limits<uint8_t>::max() };
        RideInspection _inspectionInterval{ RideInspection::every30Minutes };

    public:
        RideCreateAction() = default;
        RideCreateAction(
            ride_type_t rideType, ObjectEntryIndex subType, uint8_t trackColourPreset, uint8_t vehicleColourPreset,
            ObjectEntryIndex entranceStyleIndex, RideInspection inspectionInterval);

        void AcceptParameters(GameActionParameterVisitor&) final;

        ride_type_t GetRideType() const;
        ObjectEntryIndex GetRideObject() const;
        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
