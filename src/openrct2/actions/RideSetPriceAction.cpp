/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetPriceAction.h"

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

RideSetPriceAction::RideSetPriceAction(RideId rideIndex, money16 price, bool primaryPrice)
    : _rideIndex(rideIndex)
    , _price(price)
    , _primaryPrice(primaryPrice)
{
}

void RideSetPriceAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("price", _price);
    visitor.Visit("isPrimaryPrice", _primaryPrice);
}

uint16_t RideSetPriceAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void RideSetPriceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_rideIndex) << DS_TAG(_price) << DS_TAG(_primaryPrice);
}

GameActions::Result RideSetPriceAction::Query() const
{
    GameActions::Result res = GameActions::Result();

    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_WARNING("Invalid game command, ride_id = %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == nullptr)
    {
        LOG_WARNING("Invalid game command for ride %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return res;
}

GameActions::Result RideSetPriceAction::Execute() const
{
    GameActions::Result res = GameActions::Result();
    res.Expenditure = ExpenditureType::ParkRideTickets;

    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_WARNING("Invalid game command, ride_id = %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == nullptr)
    {
        LOG_WARNING("Invalid game command for ride %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    if (!ride->overall_view.IsNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res.Position = { location, TileElementHeight(location) };
    }

    ShopItem shopItem;
    if (_primaryPrice)
    {
        shopItem = ShopItem::Admission;

        const auto& rtd = ride->GetRideTypeDescriptor();
        if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET))
        {
            shopItem = rideEntry->shop_item[0];
            if (shopItem == ShopItem::None)
            {
                ride->price[0] = _price;
                WindowInvalidateByClass(WindowClass::Ride);
                return res;
            }
        }
        // Check same price in park flags
        if (!ShopItemHasCommonPrice(shopItem))
        {
            ride->price[0] = _price;
            WindowInvalidateByClass(WindowClass::Ride);
            return res;
        }
    }
    else
    {
        shopItem = rideEntry->shop_item[1];
        if (shopItem == ShopItem::None)
        {
            shopItem = ride->GetRideTypeDescriptor().PhotoItem;
            if ((ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) == 0)
            {
                ride->price[1] = _price;
                WindowInvalidateByClass(WindowClass::Ride);
                return res;
            }
        }
        // Check same price in park flags
        if (!ShopItemHasCommonPrice(shopItem))
        {
            ride->price[1] = _price;
            WindowInvalidateByClass(WindowClass::Ride);
            return res;
        }
    }

    // Synchronize prices if enabled.
    RideSetCommonPrice(shopItem);

    return res;
}

void RideSetPriceAction::RideSetCommonPrice(ShopItem shopItem) const
{
    for (auto& ride : GetRideManager())
    {
        auto invalidate = false;
        auto rideEntry = get_ride_entry(ride.subtype);
        const auto& rtd = ride.GetRideTypeDescriptor();
        if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_TOILET) && shopItem == ShopItem::Admission)
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
                || (rideEntry->shop_item[1] == ShopItem::None && GetShopItemDescriptor(shopItem).IsPhoto()))
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
            WindowInvalidateByNumber(WindowClass::Ride, ride.id.ToUnderlying());
        }
    }
}
