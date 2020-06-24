/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/ShopItem.h"
#include "../world/Park.h"
#include "../world/Sprite.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(RideSetPriceAction, GAME_COMMAND_SET_RIDE_PRICE, GameActionResult)
{
private:
    NetworkRideId_t _rideIndex{ RideIdNewNull };
    money16 _price = MONEY16_UNDEFINED;
    bool _primaryPrice = true;

public:
    RideSetPriceAction()
    {
    }
    RideSetPriceAction(ride_id_t rideIndex, money16 price, bool primaryPrice)
        : _rideIndex(rideIndex)
        , _price(price)
        , _primaryPrice(primaryPrice)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("price", _price);
        visitor.Visit("isPrimaryPrice", _primaryPrice);
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_price) << DS_TAG(_primaryPrice);
    }

    GameActionResult::Ptr Query() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();

        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command, ride_id = %u", uint32_t(_rideIndex));
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->Expenditure = ExpenditureType::ParkRideTickets;

        auto ride = get_ride(_rideIndex);
        if (ride == nullptr)
        {
            log_warning("Invalid game command, ride_id = %u", uint32_t(_rideIndex));
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            log_warning("Invalid game command for ride %u", uint32_t(_rideIndex));
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (!ride->overall_view.isNull())
        {
            auto location = ride->overall_view.ToTileCentre();
            res->Position = { location, tile_element_height(location) };
        }

        uint32_t shopItem;
        if (_primaryPrice)
        {
            shopItem = SHOP_ITEM_ADMISSION;
            if (ride->type != RIDE_TYPE_TOILETS)
            {
                shopItem = rideEntry->shop_item[0];
                if (shopItem == SHOP_ITEM_NONE)
                {
                    ride->price[0] = _price;
                    window_invalidate_by_class(WC_RIDE);
                    return res;
                }
            }
            // Check same price in park flags
            if (!shop_item_has_common_price(shopItem))
            {
                ride->price[0] = _price;
                window_invalidate_by_class(WC_RIDE);
                return res;
            }
        }
        else
        {
            shopItem = rideEntry->shop_item[1];
            if (shopItem == SHOP_ITEM_NONE)
            {
                shopItem = RideTypeDescriptors[ride->type].PhotoItem;
                if ((ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) == 0)
                {
                    ride->price[1] = _price;
                    window_invalidate_by_class(WC_RIDE);
                    return res;
                }
            }
            // Check same price in park flags
            if (!shop_item_has_common_price(shopItem))
            {
                ride->price[1] = _price;
                window_invalidate_by_class(WC_RIDE);
                return res;
            }
        }

        // Synchronize prices if enabled.
        RideSetCommonPrice(shopItem);

        return res;
    }

private:
    void RideSetCommonPrice(int32_t shopItem) const
    {
        for (auto& ride : GetRideManager())
        {
            auto invalidate = false;
            auto rideEntry = get_ride_entry(ride.subtype);
            if (ride.type == RIDE_TYPE_TOILETS && shopItem == SHOP_ITEM_ADMISSION)
            {
                if (ride.price[0] != _price)
                {
                    ride.price[0] = _price;
                    invalidate = true;
                }
            }
            else if (rideEntry != nullptr && rideEntry->shop_item[0] == shopItem)
            {
                if (ride.price[0] != _price)
                {
                    ride.price[0] = _price;
                    invalidate = true;
                }
            }
            if (rideEntry != nullptr)
            {
                // If the shop item is the same or an on-ride photo
                if (rideEntry->shop_item[1] == shopItem
                    || (rideEntry->shop_item[1] == SHOP_ITEM_NONE && ShopItems[shopItem].IsPhoto()))
                {
                    if (ride.price[1] != _price)
                    {
                        ride.price[1] = _price;
                        invalidate = true;
                    }
                }
            }
            if (invalidate)
            {
                window_invalidate_by_number(WC_RIDE, ride.id);
            }
        }
    }
};
