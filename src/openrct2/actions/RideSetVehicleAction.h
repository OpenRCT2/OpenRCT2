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
    enum class RideSetVehicleType : uint8_t
    {
        NumTrains,
        NumCarsPerTrain,
        RideEntry,
        TrainsReversed,
        Count,
    };

    class RideSetVehicleAction final : public GameActionBase<GameCommand::SetRideVehicles>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        RideSetVehicleType _type{};
        uint16_t _value{};
        uint8_t _colour{};

    public:
        RideSetVehicleAction() = default;
        RideSetVehicleAction(RideId rideIndex, RideSetVehicleType type, uint16_t value, uint8_t colour = 0);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        bool RideIsVehicleTypeValid(GameState_t& gameState, const Ride& ride) const;

        static_assert(sizeof(_value) >= sizeof(ObjectEntryIndex));
    };
} // namespace OpenRCT2::GameActions
