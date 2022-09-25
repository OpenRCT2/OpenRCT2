/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetVehicleAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../util/Util.h"
#include "../world/Park.h"

constexpr static StringId SetVehicleTypeErrorTitle[] = {
    STR_RIDE_SET_VEHICLE_SET_NUM_TRAINS_FAIL,
    STR_RIDE_SET_VEHICLE_SET_NUM_CARS_PER_TRAIN_FAIL,
    STR_RIDE_SET_VEHICLE_TYPE_FAIL,
};

RideSetVehicleAction::RideSetVehicleAction(RideId rideIndex, RideSetVehicleType type, uint16_t value, uint8_t colour)
    : _rideIndex(rideIndex)
    , _type(type)
    , _value(value)
    , _colour(colour)
{
}

void RideSetVehicleAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("type", _type);
    visitor.Visit("value", _value);
    visitor.Visit("colour", _colour);
}

uint16_t RideSetVehicleAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void RideSetVehicleAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_rideIndex) << DS_TAG(_type) << DS_TAG(_value) << DS_TAG(_colour);
}

GameActions::Result RideSetVehicleAction::Query() const
{
    if (_type >= RideSetVehicleType::Count)
    {
        log_warning("Invalid type. type = %d", _type);
    }
    auto errTitle = SetVehicleTypeErrorTitle[EnumValue(_type)];

    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command, ride_id = %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
    }

    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        return GameActions::Result(GameActions::Status::Broken, errTitle, STR_HAS_BROKEN_DOWN_AND_REQUIRES_FIXING);
    }

    if (ride->status != RideStatus::Closed && ride->status != RideStatus::Simulating)
    {
        return GameActions::Result(GameActions::Status::NotClosed, errTitle, STR_MUST_BE_CLOSED_FIRST);
    }

    switch (_type)
    {
        case RideSetVehicleType::NumTrains:
        case RideSetVehicleType::NumCarsPerTrain:
            break;
        case RideSetVehicleType::RideEntry:
        {
            if (!ride_is_vehicle_type_valid(ride))
            {
                log_error("Invalid vehicle type. type = %d", _value);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }
            auto rideEntry = get_ride_entry(_value);
            if (rideEntry == nullptr)
            {
                log_warning("Invalid ride entry, ride->subtype = %d", ride->subtype);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }

            // Validate preset
            vehicle_colour_preset_list* presetList = rideEntry->vehicle_preset_list;
            if (_colour >= presetList->count && _colour != 255 && _colour != 0)
            {
                log_error("Unknown vehicle colour preset. colour = %d", _colour);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }
            break;
        }

        default:
            log_error("Unknown vehicle command. type = %d", _type);
            return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
    }

    return GameActions::Result();
}

GameActions::Result RideSetVehicleAction::Execute() const
{
    auto errTitle = SetVehicleTypeErrorTitle[EnumValue(_type)];
    auto ride = get_ride(_rideIndex);
    if (ride == nullptr)
    {
        log_warning("Invalid game command, ride_id = %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
    }

    switch (_type)
    {
        case RideSetVehicleType::NumTrains:
            ride_clear_for_construction(ride);
            ride->RemovePeeps();
            ride->vehicle_change_timeout = 100;

            ride->ProposedNumTrains = _value;
            break;
        case RideSetVehicleType::NumCarsPerTrain:
        {
            ride_clear_for_construction(ride);
            ride->RemovePeeps();
            ride->vehicle_change_timeout = 100;

            invalidate_test_results(ride);
            auto rideEntry = get_ride_entry(ride->subtype);
            if (rideEntry == nullptr)
            {
                log_warning("Invalid ride entry, ride->subtype = %d", ride->subtype);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }
            uint8_t clampValue = _value;
            static_assert(sizeof(clampValue) == sizeof(ride->proposed_num_cars_per_train));
            if (!gCheatsDisableTrainLengthLimit)
            {
                clampValue = std::clamp(clampValue, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
            }
            ride->proposed_num_cars_per_train = clampValue;
            break;
        }
        case RideSetVehicleType::RideEntry:
        {
            ride_clear_for_construction(ride);
            ride->RemovePeeps();
            ride->vehicle_change_timeout = 100;

            invalidate_test_results(ride);
            ride->subtype = _value;
            auto rideEntry = get_ride_entry(ride->subtype);
            if (rideEntry == nullptr)
            {
                log_warning("Invalid ride entry, ride->subtype = %d", ride->subtype);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }

            ride_set_vehicle_colours_to_random_preset(ride, _colour);
            if (!gCheatsDisableTrainLengthLimit)
            {
                ride->proposed_num_cars_per_train = std::clamp(
                    ride->proposed_num_cars_per_train, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
            }
            break;
        }

        default:
            log_error("Unknown vehicle command. type = %d", _type);
            return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
    }

    ride->num_circuits = 1;
    ride->UpdateMaxVehicles();

    auto res = GameActions::Result();
    if (!ride->overall_view.IsNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res.Position = { location, tile_element_height(res.Position) };
    }

    auto intent = Intent(INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE);
    intent.putExtra(INTENT_EXTRA_RIDE_ID, _rideIndex.ToUnderlying());
    context_broadcast_intent(&intent);

    gfx_invalidate_screen();
    return res;
}

bool RideSetVehicleAction::ride_is_vehicle_type_valid(Ride* ride) const
{
    bool selectionShouldBeExpanded;
    int32_t rideTypeIterator, rideTypeIteratorMax;

    if (gCheatsShowVehiclesFromOtherTrackTypes
        && !(
            ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE) || ride->type == RIDE_TYPE_MAZE
            || ride->type == RIDE_TYPE_MINI_GOLF))
    {
        selectionShouldBeExpanded = true;
        rideTypeIterator = 0;
        rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
    }
    else
    {
        selectionShouldBeExpanded = false;
        rideTypeIterator = ride->type;
        rideTypeIteratorMax = ride->type;
    }

    for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
    {
        if (selectionShouldBeExpanded)
        {
            if (GetRideTypeDescriptor(rideTypeIterator).HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
                continue;
            if (rideTypeIterator == RIDE_TYPE_MAZE || rideTypeIterator == RIDE_TYPE_MINI_GOLF)
                continue;
        }

        auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        auto& rideEntries = objManager.GetAllRideEntries(rideTypeIterator);
        for (auto rideEntryIndex : rideEntries)
        {
            if (rideEntryIndex == _value)
            {
                if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                {
                    return false;
                }

                return true;
            }
        }
    }

    return false;
}
