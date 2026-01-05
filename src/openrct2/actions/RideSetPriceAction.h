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
    class RideSetPriceAction final : public GameActionBase<GameCommand::SetRidePrice>
    {
    private:
        RideId _rideIndex{ RideId::GetNull() };
        money64 _price{ kMoney64Undefined };
        bool _primaryPrice{ true };

    public:
        RideSetPriceAction() = default;
        RideSetPriceAction(RideId rideIndex, money64 price, bool primaryPrice);

        void AcceptParameters(GameActionParameterVisitor&) final;

        uint16_t GetActionFlags() const override;

        void Serialise(DataSerialiser& stream) override;
        Result Query(GameState_t& gameState) const override;
        Result Execute(GameState_t& gameState) const override;

    private:
        void RideSetCommonPrice(GameState_t& gameState, ShopItem shopItem) const;
    };
} // namespace OpenRCT2::GameActions
