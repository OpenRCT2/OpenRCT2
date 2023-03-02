/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

class RideSetPriceAction final : public GameActionBase<GameCommand::SetRidePrice>
{
private:
    RideId _rideIndex{ RideId::GetNull() };
    money64 _price{ MONEY64_UNDEFINED };
    bool _primaryPrice{ true };

public:
    RideSetPriceAction() = default;
    RideSetPriceAction(RideId rideIndex, money64 price, bool primaryPrice);

    void AcceptParameters(GameActionParameterVisitor& visitor) override;

    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser& stream) override;
    GameActions::Result Query() const override;
    GameActions::Result Execute() const override;

private:
    void RideSetCommonPrice(ShopItem shopItem) const;
};
