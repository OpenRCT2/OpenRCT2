/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetPriceAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ride/ShopItem.h"
#include "../ui/WindowManager.h"
#include "../world/Map.h"
#include "../world/Park.h"

namespace OpenRCT2::GameActions
{
    RideSetPriceAction::RideSetPriceAction(RideId rideIndex, money64 price, bool primaryPrice)
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void RideSetPriceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_rideIndex) << DS_TAG(_price) << DS_TAG(_primaryPrice);
    }

    Result RideSetPriceAction::Query(GameState_t& gameState) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
        }

        const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
        if (rideEntry == nullptr)
        {
            LOG_ERROR("Ride entry not found for ride subtype %u", ride->subtype);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_OBJECT_ENTRY_NOT_FOUND);
        }

        if (_price < kRideMinPrice || _price > kRideMaxPrice)
        {
            LOG_ERROR("Attempting to set an invalid price for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, kStringIdEmpty);
        }

        return Result();
    }

    Result RideSetPriceAction::Execute(GameState_t& gameState) const
    {
        Result res = Result();
        res.expenditure = ExpenditureType::parkRideTickets;

        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
        }

        const auto* rideEntry = GetRideEntryByIndex(ride->subtype);
        if (rideEntry == nullptr)
        {
            LOG_ERROR("Ride entry not found for ride subtype %u", ride->subtype);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_OBJECT_ENTRY_NOT_FOUND);
        }

        if (_price < kRideMinPrice || _price > kRideMaxPrice)
        {
            LOG_ERROR("Attempting to set an invalid price for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, kStringIdEmpty);
        }

        if (!ride->overallView.IsNull())
        {
            auto location = ride->overallView.ToTileCentre();
            res.position = { location, TileElementHeight(location) };
        }

        auto* windowMgr = Ui::GetWindowManager();

        ShopItem shopItem;
        if (_primaryPrice)
        {
            shopItem = ShopItem::admission;

            const auto& rtd = ride->getRideTypeDescriptor();
            if (rtd.specialType != RtdSpecialType::toilet)
            {
                shopItem = rideEntry->shop_item[0];
                if (shopItem == ShopItem::none)
                {
                    ride->price[0] = _price;
                    windowMgr->InvalidateByClass(WindowClass::ride);
                    return res;
                }
            }
            // Check same price in park flags
            if (!ShopItemHasCommonPrice(shopItem))
            {
                ride->price[0] = _price;
                windowMgr->InvalidateByClass(WindowClass::ride);
                return res;
            }
        }
        else
        {
            shopItem = rideEntry->shop_item[1];
            if (shopItem == ShopItem::none)
            {
                shopItem = ride->getRideTypeDescriptor().PhotoItem;
                if ((ride->lifecycleFlags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) == 0)
                {
                    ride->price[1] = _price;
                    windowMgr->InvalidateByClass(WindowClass::ride);
                    return res;
                }
            }
            // Check same price in park flags
            if (!ShopItemHasCommonPrice(shopItem))
            {
                ride->price[1] = _price;
                windowMgr->InvalidateByClass(WindowClass::ride);
                return res;
            }
        }

        // Synchronize prices if enabled.
        RideSetCommonPrice(gameState, shopItem);

        return res;
    }

    void RideSetPriceAction::RideSetCommonPrice(GameState_t& gameState, ShopItem shopItem) const
    {
        for (auto& ride : RideManager(gameState))
        {
            auto invalidate = false;
            auto rideEntry = GetRideEntryByIndex(ride.subtype);
            const auto& rtd = ride.getRideTypeDescriptor();
            if (rtd.specialType == RtdSpecialType::toilet && shopItem == ShopItem::admission)
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
                    || (rideEntry->shop_item[1] == ShopItem::none && GetShopItemDescriptor(shopItem).IsPhoto()))
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
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->InvalidateByNumber(WindowClass::ride, ride.id.ToUnderlying());
            }
        }
    }
} // namespace OpenRCT2::GameActions
