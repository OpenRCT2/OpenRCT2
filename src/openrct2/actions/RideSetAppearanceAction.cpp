/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetAppearanceAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ride/RideManager.hpp"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

using namespace OpenRCT2;

RideSetAppearanceAction::RideSetAppearanceAction(RideId rideIndex, RideSetAppearanceType type, uint16_t value, uint32_t index)
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
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void RideSetAppearanceAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_rideIndex) << DS_TAG(_type) << DS_TAG(_value) << DS_TAG(_index);
}

GameActions::Result RideSetAppearanceAction::Query() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
    }

    switch (_type)
    {
        case RideSetAppearanceType::TrackColourMain:
        case RideSetAppearanceType::TrackColourAdditional:
        case RideSetAppearanceType::TrackColourSupports:
            if (_index >= std::size(ride->trackColours))
            {
                LOG_ERROR("Invalid track colour %u", _index);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_INVALID_COLOUR);
            }
            break;
        case RideSetAppearanceType::VehicleColourBody:
        case RideSetAppearanceType::VehicleColourTrim:
        case RideSetAppearanceType::VehicleColourTertiary:
            if (_index >= std::size(ride->vehicleColours))
            {
                LOG_ERROR("Invalid vehicle colour %u", _index);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_INVALID_COLOUR);
            }
            break;
        case RideSetAppearanceType::VehicleColourScheme:
        case RideSetAppearanceType::EntranceStyle:
        case RideSetAppearanceType::SellingItemColourIsRandom:
            break;
        default:
            LOG_ERROR("Invalid ride appearance type %u", _type);
            return GameActions::Result(
                GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    return GameActions::Result();
}

GameActions::Result RideSetAppearanceAction::Execute() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
    }

    switch (_type)
    {
        case RideSetAppearanceType::TrackColourMain:
            ride->trackColours[_index].main = _value;
            if (_index == 0)
            {
                PropagateShopItemCommonColourValue(*ride, &RideSetAppearanceAction::UpdateRideShopItemColour);
            }
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::TrackColourAdditional:
            ride->trackColours[_index].additional = _value;
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::TrackColourSupports:
            ride->trackColours[_index].supports = _value;
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::VehicleColourBody:
            ride->vehicleColours[_index].Body = _value;
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::VehicleColourTrim:
            ride->vehicleColours[_index].Trim = _value;
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::VehicleColourTertiary:
            ride->vehicleColours[_index].Tertiary = _value;
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::VehicleColourScheme:
            ride->vehicleColourSettings = static_cast<VehicleColourSettings>(_value);
            for (uint32_t i = 1; i < std::size(ride->vehicleColours); i++)
            {
                ride->vehicleColours[i] = ride->vehicleColours[0];
            }
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::EntranceStyle:
            ride->entranceStyle = _value;
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::SellingItemColourIsRandom:
            ride->setLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS, static_cast<bool>(_value));
            PropagateShopItemCommonColourValue(*ride, &RideSetAppearanceAction::UpdateRideShopItemRandomColourFlag);
            break;
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());

    auto res = GameActions::Result();
    if (!ride->overallView.IsNull())
    {
        auto location = ride->overallView.ToTileCentre();
        res.Position = { location, TileElementHeight(location) };
    }

    return res;
}

void RideSetAppearanceAction::UpdateRideShopItemColour(Ride& ride) const
{
    ride.trackColours[0].main = _value;
}

void RideSetAppearanceAction::UpdateRideShopItemRandomColourFlag(Ride& ride) const
{
    ride.setLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS, static_cast<bool>(_value));
    auto windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Ride, ride.id.ToUnderlying());
}

void RideSetAppearanceAction::PropagateShopItemCommonColourValue(Ride& changedRide, RideUpdater updater) const
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
    for (auto& ride : GetRideManager())
    {
        if (ride.id == changedRide.id)
            continue;

        auto optShopItem = ride.getRecolourableShopItem();
        if (!optShopItem.has_value() || changedShopItem != optShopItem.value())
            continue;

        std::invoke(updater, this, ride);
    }
}
