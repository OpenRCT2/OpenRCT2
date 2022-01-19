/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetAppearanceAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../ride/Ride.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

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
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command, ride_id = %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    switch (_type)
    {
        case RideSetAppearanceType::TrackColourMain:
        case RideSetAppearanceType::TrackColourAdditional:
        case RideSetAppearanceType::TrackColourSupports:
            if (_index >= std::size(ride->track_colour))
            {
                log_warning("Invalid game command, index %d out of bounds", _index);
                return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
            }
            break;
        case RideSetAppearanceType::VehicleColourBody:
        case RideSetAppearanceType::VehicleColourTrim:
        case RideSetAppearanceType::VehicleColourTernary:
            if (_index >= std::size(ride->vehicle_colours))
            {
                log_warning("Invalid game command, index %d out of bounds", _index);
                return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
            }
            break;
        case RideSetAppearanceType::VehicleColourScheme:
        case RideSetAppearanceType::EntranceStyle:
            break;
        default:
            log_warning("Invalid game command, type %d not recognised", _type);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return GameActions::Result();
}

GameActions::Result RideSetAppearanceAction::Execute() const
{
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command, ride_id = %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    switch (_type)
    {
        case RideSetAppearanceType::TrackColourMain:
            ride->track_colour[_index].main = _value;
            gfx_invalidate_screen();
            break;
        case RideSetAppearanceType::TrackColourAdditional:
            ride->track_colour[_index].additional = _value;
            gfx_invalidate_screen();
            break;
        case RideSetAppearanceType::TrackColourSupports:
            ride->track_colour[_index].supports = _value;
            gfx_invalidate_screen();
            break;
        case RideSetAppearanceType::VehicleColourBody:
            ride->vehicle_colours[_index].Body = _value;
            ride_update_vehicle_colours(ride);
            break;
        case RideSetAppearanceType::VehicleColourTrim:
            ride->vehicle_colours[_index].Trim = _value;
            ride_update_vehicle_colours(ride);
            break;
        case RideSetAppearanceType::VehicleColourTernary:
            ride->vehicle_colours[_index].Ternary = _value;
            ride_update_vehicle_colours(ride);
            break;
        case RideSetAppearanceType::VehicleColourScheme:
            ride->colour_scheme_type &= ~(RIDE_COLOUR_SCHEME_DIFFERENT_PER_TRAIN | RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR);
            ride->colour_scheme_type |= _value;
            for (uint32_t i = 1; i < std::size(ride->vehicle_colours); i++)
            {
                ride->vehicle_colours[i] = ride->vehicle_colours[0];
            }
            ride_update_vehicle_colours(ride);
            break;
        case RideSetAppearanceType::EntranceStyle:
            ride->entrance_style = _value;
            gfx_invalidate_screen();
            break;
    }
    window_invalidate_by_number(WC_RIDE, _rideIndex.ToUnderlying());

    auto res = GameActions::Result();
    if (!ride->overall_view.IsNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res.Position = { location, tile_element_height(location) };
    }

    return res;
}
