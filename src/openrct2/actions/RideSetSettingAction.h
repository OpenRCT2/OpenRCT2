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
    enum class RideSetSetting : uint8_t
    {
        Mode,
        Departure,
        MinWaitingTime,
        MaxWaitingTime,
        Operation,
        InspectionInterval,
        Music,
        MusicType,
        LiftHillSpeed,
        NumCircuits,
        RideType,
    };

    class RideSetSettingAction final : public GameActionBase<GameCommand::SetRideSetting>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        RideSetSetting _setting{};
        uint8_t _value{};

    public:
        RideSetSettingAction() = default;
        RideSetSettingAction(RideId rideIndex, RideSetSetting setting, uint8_t value);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        bool RideIsModeValid(const Ride& ride) const;
        bool RideIsValidLiftHillSpeed(GameState_t& gameState, const Ride& ride) const;
        bool RideIsValidNumCircuits(GameState_t& gameState) const;
        bool RideIsValidOperationOption(GameState_t& gameState, const Ride& ride) const;
        StringId GetOperationErrorMessage(const Ride& ride) const;
    };
} // namespace OpenRCT2::GameActions
