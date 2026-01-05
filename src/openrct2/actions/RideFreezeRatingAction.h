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
    enum class RideRatingType : uint8_t
    {
        Excitement,
        Intensity,
        Nausea,
    };

    class RideFreezeRatingAction final : public GameActionBase<GameCommand::FreezeRideRating>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        RideRatingType _type{};
        RideRating_t _value{};

    public:
        RideFreezeRatingAction() = default;
        RideFreezeRatingAction(RideId rideIndex, RideRatingType type, RideRating_t value);

        void AcceptParameters(GameActionParameterVisitor&) final;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;
    };
} // namespace OpenRCT2::GameActions
