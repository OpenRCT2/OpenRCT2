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
    enum class RideModifyType : uint8_t
    {
        demolish,
        renew,
    };

    class RideDemolishAction final : public GameActionBase<GameCommand::DemolishRide>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        RideModifyType _modifyType{ RideModifyType::demolish };

    public:
        RideDemolishAction() = default;
        RideDemolishAction(RideId rideIndex, RideModifyType modifyType);

        void AcceptParameters(GameActionParameterVisitor& visitor) override;

        uint32_t GetCooldownTime() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query() const override;
        Result Execute() const override;

    private:
        Result DemolishRide(Ride& ride) const;
        money64 MazeRemoveTrack(const CoordsXYZD& coords) const;
        money64 DemolishTracks() const;
        Result RefurbishRide(Ride& ride) const;
        money64 GetRefurbishPrice(const Ride& ride) const;
        money64 GetRefundPrice(const Ride& ride) const;
    };
} // namespace OpenRCT2::GameActions
