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
            if (_index >= std::size(ride->track_colour))
            {
                LOG_ERROR("Invalid track colour %u", _index);
                return GameActions::Result(
                    GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_INVALID_COLOUR);
            }
            break;
        case RideSetAppearanceType::VehicleColourBody:
        case RideSetAppearanceType::VehicleColourTrim:
        case RideSetAppearanceType::VehicleColourTertiary:
            if (_index >= std::size(ride->vehicle_colours))
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
            ride->track_colour[_index].main = _value;
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::TrackColourAdditional:
            ride->track_colour[_index].additional = _value;
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::TrackColourSupports:
            ride->track_colour[_index].supports = _value;
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::VehicleColourBody:
            ride->vehicle_colours[_index].Body = _value;
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::VehicleColourTrim:
            ride->vehicle_colours[_index].Trim = _value;
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::VehicleColourTertiary:
            ride->vehicle_colours[_index].Tertiary = _value;
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::VehicleColourScheme:
            ride->vehicleColourSettings = static_cast<VehicleColourSettings>(_value);
            for (uint32_t i = 1; i < std::size(ride->vehicle_colours); i++)
            {
                ride->vehicle_colours[i] = ride->vehicle_colours[0];
            }
            RideUpdateVehicleColours(*ride);
            break;
        case RideSetAppearanceType::EntranceStyle:
            ride->entrance_style = _value;
            GfxInvalidateScreen();
            break;
        case RideSetAppearanceType::SellingItemColourIsRandom:
            ride->SetLifecycleFlag(RIDE_LIFECYCLE_RANDOM_SHOP_COLOURS, static_cast<bool>(_value));
            break;
    }

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());

    auto res = GameActions::Result();
    if (!ride->overall_view.IsNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res.Position = { location, TileElementHeight(location) };
    }

    return res;
}
