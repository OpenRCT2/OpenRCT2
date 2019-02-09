/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
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
    NetworkRideId_t _rideIndex{ -1 };
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

        if (_rideIndex >= MAX_RIDES || _rideIndex == RIDE_ID_NULL)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        Ride* ride = get_ride(_rideIndex);
        if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command, ride_id = %u", _rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        GameActionResult::Ptr res = std::make_unique<GameActionResult>();
        res->ExpenditureType = RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS;

        Ride* ride = get_ride(_rideIndex);
        if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
        {
            log_warning("Invalid game command, ride_id = %u", _rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
        if (rideEntry == nullptr)
        {
            log_warning("Invalid game command for ride %u", _rideIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        if (ride->overall_view.xy != RCT_XY8_UNDEFINED)
        {
            LocationXYZ16 coord;
            coord.x = ride->overall_view.x * 32 + 16;
            coord.y = ride->overall_view.y * 32 + 16;
            coord.z = tile_element_height(coord.x, coord.y);
            network_set_player_last_action_coord(network_get_player_index(game_command_playerid), coord);
        }

        uint32_t shopItem;
        if (_primaryPrice)
        {
            shopItem = SHOP_ITEM_ADMISSION;
            if (ride->type != RIDE_TYPE_TOILETS)
            {
                shopItem = rideEntry->shop_item;
                if (shopItem == SHOP_ITEM_NONE)
                {
                    ride->price = _price;
                    window_invalidate_by_class(WC_RIDE);
                    return res;
                }
            }
            // Check same price in park flags
            if (!shop_item_has_common_price(shopItem))
            {
                ride->price = _price;
                window_invalidate_by_class(WC_RIDE);
                return res;
            }
        }
        else
        {
            shopItem = rideEntry->shop_item_secondary;
            if (shopItem == SHOP_ITEM_NONE)
            {
                shopItem = RidePhotoItems[ride->type];
                if ((ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) == 0)
                {
                    ride->price_secondary = _price;
                    window_invalidate_by_class(WC_RIDE);
                    return res;
                }
            }
            // Check same price in park flags
            if (!shop_item_has_common_price(shopItem))
            {
                ride->price_secondary = _price;
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
        Ride* ride = get_ride(0);
        for (uint8_t rideId = 0; rideId < MAX_RIDES; rideId++, ride++)
        {
            // Unplaced rides have a type of NULL
            if (ride->type == RIDE_TYPE_NULL)
                continue;

            rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);

            if (ride->type != RIDE_TYPE_TOILETS || shopItem != SHOP_ITEM_ADMISSION)
            {
                if (rideEntry->shop_item == shopItem)
                {
                    ride->price = _price;
                    window_invalidate_by_number(WC_RIDE, rideId);
                }
            }
            else
            {
                ride->price = _price;
                window_invalidate_by_number(WC_RIDE, rideId);
            }

            // If the shop item is the same or an on-ride photo
            if (rideEntry->shop_item_secondary == shopItem
                || (rideEntry->shop_item_secondary == SHOP_ITEM_NONE && shop_item_is_photo(shopItem)))
            {
                ride->price_secondary = _price;
                window_invalidate_by_number(WC_RIDE, rideId);
            }
        }
    }
};
