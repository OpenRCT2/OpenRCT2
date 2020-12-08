/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

DEFINE_GAME_ACTION(RideSetPriceAction, GAME_COMMAND_SET_RIDE_PRICE, GameActions::Result)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    money16 _price{ MONEY16_UNDEFINED };
    bool _primaryPrice{ true };

public:
    RideSetPriceAction() = default;
    RideSetPriceAction(ride_id_t rideIndex, money16 price, bool primaryPrice)
        : _rideIndex(rideIndex)
        , _price(price)
        , _primaryPrice(primaryPrice)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override;

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;

    GameActions::Result::Ptr Query() const override;

    GameActions::Result::Ptr Execute() const override;

private:
    void RideSetCommonPrice(ShopItem shopItem) const;
};
