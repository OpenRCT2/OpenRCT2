/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetAppearanceAction.h"

#include "../../Cheats.h"
#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../core/MemoryStream.h"
#include "../../drawing/Drawing.h"
#include "../../localisation/StringIds.h"
#include "../../ride/Ride.h"
#include "../../ride/RideManager.hpp"
#include "../../ui/WindowManager.h"
#include "../../world/Map.h"
#include "../../world/Park.h"

namespace OpenRCT2::GameActions
{
    RideSetAppearanceAction::RideSetAppearanceAction(
        RideId rideIndex, RideSetAppearanceType type, uint16_t value, uint32_t index)
        : _rideIndex(rideIndex)
        , _type(type)
        , _value(value)
        , _index(index)
    {
    }

    void RideSetAppearanceAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("type", _type);
        visitor.Visit("value", _value);
        visitor.Visit("index", _index);
    }

    uint16_t RideSetAppearanceAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void RideSetAppearanceAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_rideIndex) << DS_TAG(_type) << DS_TAG(_value) << DS_TAG(_index);
    }

    Result RideSetAppearanceAction::Query(GameState_t& gameState, Park::ParkData& park) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
        }

        switch (_type)
        {
            case RideSetAppearanceType::trackColourMain:
            case RideSetAppearanceType::trackColourAdditional:
            case RideSetAppearanceType::trackColourSupports:
                if (_index >= std::size(ride->trackColours))
                {
                    LOG_ERROR("Invalid track colour %u", _index);
                    return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_INVALID_COLOUR);
                }
                break;
            case RideSetAppearanceType::vehicleColourBody:
            case RideSetAppearanceType::vehicleColourTrim:
            case RideSetAppearanceType::vehicleColourTertiary:
                if (_index >= std::size(ride->vehicleColours))
                {
                    LOG_ERROR("Invalid vehicle colour %u", _index);
                    return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_INVALID_COLOUR);
                }
                break;
            case RideSetAppearanceType::vehicleColourScheme:
            case RideSetAppearanceType::entranceStyle:
            case RideSetAppearanceType::sellingItemColourIsRandom:
                break;
            default:
                LOG_ERROR("Invalid ride appearance type %u", _type);
                return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        return Result();
    }

    Result RideSetAppearanceAction::Execute(GameState_t& gameState, Park::ParkData& park) const
    {
        auto ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
        }

        switch (_type)
        {
            case RideSetAppearanceType::trackColourMain:
                ride->trackColours[_index].main = static_cast<Drawing::Colour>(_value);
                if (_index == 0)
                    propagateShopItemCommonColourValue(gameState, *ride, &RideSetAppearanceAction::updateRideShopItemColour);
                GfxInvalidateScreen();
                break;
            case RideSetAppearanceType::trackColourAdditional:
                ride->trackColours[_index].additional = static_cast<Drawing::Colour>(_value);
                GfxInvalidateScreen();
                break;
            case RideSetAppearanceType::trackColourSupports:
                ride->trackColours[_index].supports = static_cast<Drawing::Colour>(_value);
                GfxInvalidateScreen();
                break;
            case RideSetAppearanceType::vehicleColourBody:
                ride->vehicleColours[_index].Body = static_cast<Drawing::Colour>(_value);
                RideUpdateVehicleColours(*ride);
                break;
            case RideSetAppearanceType::vehicleColourTrim:
                ride->vehicleColours[_index].Trim = static_cast<Drawing::Colour>(_value);
                RideUpdateVehicleColours(*ride);
                break;
            case RideSetAppearanceType::vehicleColourTertiary:
                ride->vehicleColours[_index].Tertiary = static_cast<Drawing::Colour>(_value);
                RideUpdateVehicleColours(*ride);
                break;
            case RideSetAppearanceType::vehicleColourScheme:
                ride->vehicleColourSettings = static_cast<VehicleColourSettings>(_value);
                for (uint32_t i = 1; i < std::size(ride->vehicleColours); i++)
                {
                    ride->vehicleColours[i] = ride->vehicleColours[0];
                }
                RideUpdateVehicleColours(*ride);
                break;
            case RideSetAppearanceType::entranceStyle:
                ride->entranceStyle = _value;
                GfxInvalidateScreen();
                break;
            case RideSetAppearanceType::sellingItemColourIsRandom:
                ride->flags.set(RideFlag::randomShopColours, static_cast<bool>(_value));
                propagateShopItemCommonColourValue(
                    gameState, *ride, &RideSetAppearanceAction::updateRideShopItemRandomColourFlag);
                break;
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::ride, _rideIndex.ToUnderlying());

        auto res = Result();
        if (!ride->overallView.IsNull())
        {
            auto location = ride->overallView.ToTileCentre();
            res.position = { location, TileElementHeight(location) };
        }

        return res;
    }

    void RideSetAppearanceAction::updateRideShopItemColour(Ride& ride) const
    {
        ride.trackColours[0].main = static_cast<Drawing::Colour>(_value);
    }

    void RideSetAppearanceAction::updateRideShopItemRandomColourFlag(Ride& ride) const
    {
        ride.flags.set(RideFlag::randomShopColours, static_cast<bool>(_value));
        auto windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::ride, ride.id.ToUnderlying());
    }

    void RideSetAppearanceAction::propagateShopItemCommonColourValue(
        GameState_t& gameState, Ride& changedRide, RideUpdater updater) const
    {
        // first check if the changed ride has a recolourable shop item
        auto optChangedShopItem = changedRide.getRecolourableShopItem();
        if (!optChangedShopItem.has_value())
            return;

        // next check if the shop item has the common colour flag set
        auto changedShopItem = optChangedShopItem.value();
        if (!ShopItemHasCommonColour(changedShopItem))
            return;

        // propagate changed value to other rides with the same shop item
        for (auto& ride : RideManager(gameState))
        {
            if (ride.id == changedRide.id)
                continue;

            auto optShopItem = ride.getRecolourableShopItem();
            if (!optShopItem.has_value() || changedShopItem != optShopItem.value())
                continue;

            std::invoke(updater, this, ride);
        }
    }
} // namespace OpenRCT2::GameActions
