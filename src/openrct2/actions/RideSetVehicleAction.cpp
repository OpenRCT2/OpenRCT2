/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetVehicleAction.h"

#include "../Cheats.h"
#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/EnumUtils.hpp"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Research.h"
#include "../object/ObjectManager.h"
#include "../ride/Ride.h"
#include "../ride/RideData.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../world/Park.h"

using namespace OpenRCT2;

constexpr static StringId SetVehicleTypeErrorTitle[] = {
    STR_RIDE_SET_VEHICLE_SET_NUM_TRAINS_FAIL,
    STR_RIDE_SET_VEHICLE_SET_NUM_CARS_PER_TRAIN_FAIL,
    STR_RIDE_SET_VEHICLE_TYPE_FAIL,
    STR_RIDE_SET_VEHICLE_REVERSED_FAIL,
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
        LOG_ERROR("Invalid ride vehicle type %d", _type);
    }
    auto errTitle = SetVehicleTypeErrorTitle[EnumValue(_type)];

    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_ERR_RIDE_NOT_FOUND);
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
        case RideSetVehicleType::TrainsReversed:
            break;
        case RideSetVehicleType::RideEntry:
        {
            if (!RideIsVehicleTypeValid(*ride))
            {
                LOG_ERROR("Invalid vehicle type %d", _value);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_ERR_VALUE_OUT_OF_RANGE);
            }
            auto rideEntry = GetRideEntryByIndex(_value);
            if (rideEntry == nullptr)
            {
                LOG_ERROR("Ride entry not found for _value %d", _value);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }

            // Validate preset
            VehicleColourPresetList* presetList = rideEntry->vehicle_preset_list;
            if (_colour >= presetList->count && _colour != 255 && _colour != 0)
            {
                LOG_ERROR("Unknown vehicle colour preset. colour = %d", _colour);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_ERR_INVALID_COLOUR);
            }
            break;
        }

        default:
            LOG_ERROR("Invalid ride vehicle setting %d", _type);
            return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    return GameActions::Result();
}

GameActions::Result RideSetVehicleAction::Execute() const
{
    auto errTitle = SetVehicleTypeErrorTitle[EnumValue(_type)];
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_ERR_RIDE_NOT_FOUND);
    }

    switch (_type)
    {
        case RideSetVehicleType::NumTrains:
            RideClearForConstruction(*ride);
            ride->RemovePeeps();
            ride->vehicle_change_timeout = 100;

            ride->ProposedNumTrains = _value;
            break;
        case RideSetVehicleType::NumCarsPerTrain:
        {
            RideClearForConstruction(*ride);
            ride->RemovePeeps();
            ride->vehicle_change_timeout = 100;

            InvalidateTestResults(*ride);
            auto rideEntry = GetRideEntryByIndex(ride->subtype);
            if (rideEntry == nullptr)
            {
                LOG_ERROR("Ride entry not found for index %d", ride->subtype);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }
            uint8_t clampValue = _value;
            static_assert(sizeof(clampValue) == sizeof(ride->proposed_num_cars_per_train));
            if (!GetGameState().Cheats.disableTrainLengthLimit)
            {
                clampValue = std::clamp(clampValue, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
            }
            ride->proposed_num_cars_per_train = clampValue;
            break;
        }
        case RideSetVehicleType::RideEntry:
        {
            RideClearForConstruction(*ride);
            ride->RemovePeeps();
            ride->vehicle_change_timeout = 100;

            InvalidateTestResults(*ride);
            ride->subtype = _value;
            auto rideEntry = GetRideEntryByIndex(ride->subtype);
            if (rideEntry == nullptr)
            {
                LOG_ERROR("Ride entry not found for index %d", ride->subtype);
                return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
            }

            RideSetVehicleColoursToRandomPreset(*ride, _colour);
            if (!GetGameState().Cheats.disableTrainLengthLimit)
            {
                ride->proposed_num_cars_per_train = std::clamp(
                    ride->proposed_num_cars_per_train, rideEntry->min_cars_in_train, rideEntry->max_cars_in_train);
            }
            break;
        }
        case RideSetVehicleType::TrainsReversed:
        {
            RideClearForConstruction(*ride);
            ride->RemovePeeps();
            ride->vehicle_change_timeout = 100;

            ride->SetLifecycleFlag(RIDE_LIFECYCLE_REVERSED_TRAINS, _value);
            break;
        }

        default:
            LOG_ERROR("Invalid ride vehicle setting %d", _type);
            return GameActions::Result(GameActions::Status::InvalidParameters, errTitle, STR_NONE);
    }

    ride->num_circuits = 1;
    ride->UpdateMaxVehicles();

    auto res = GameActions::Result();
    if (!ride->overall_view.IsNull())
    {
        auto location = ride->overall_view.ToTileCentre();
        res.Position = { location, TileElementHeight(res.Position) };
    }

    auto intent = Intent(INTENT_ACTION_RIDE_PAINT_RESET_VEHICLE);
    intent.PutExtra(INTENT_EXTRA_RIDE_ID, _rideIndex.ToUnderlying());
    ContextBroadcastIntent(&intent);

    GfxInvalidateScreen();
    return res;
}

bool RideSetVehicleAction::RideIsVehicleTypeValid(const Ride& ride) const
{
    bool selectionShouldBeExpanded;
    ride_type_t rideTypeIterator, rideTypeIteratorMax;
    auto& gameState = GetGameState();

    {
        const auto& rtd = ride.GetRideTypeDescriptor();
        if (gameState.Cheats.showVehiclesFromOtherTrackTypes
            && !(
                ride.GetRideTypeDescriptor().HasFlag(RtdFlag::isFlatRide) || rtd.specialType == RtdSpecialType::maze
                || rtd.specialType == RtdSpecialType::miniGolf))
        {
            selectionShouldBeExpanded = true;
            rideTypeIterator = 0;
            rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
        }
        else
        {
            selectionShouldBeExpanded = false;
            rideTypeIterator = ride.type;
            rideTypeIteratorMax = ride.type;
        }
    }

    for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
    {
        if (selectionShouldBeExpanded)
        {
            if (GetRideTypeDescriptor(rideTypeIterator).HasFlag(RtdFlag::isFlatRide))
                continue;

            const auto& rtd = GetRideTypeDescriptor(rideTypeIterator);
            if (rtd.specialType == RtdSpecialType::maze || rtd.specialType == RtdSpecialType::miniGolf)
                continue;
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto& rideEntries = objManager.GetAllRideEntries(rideTypeIterator);
        for (auto rideEntryIndex : rideEntries)
        {
            if (rideEntryIndex == _value)
            {
                if (!RideEntryIsInvented(rideEntryIndex) && !gameState.Cheats.ignoreResearchStatus)
                {
                    return false;
                }

                return true;
            }
        }
    }

    return false;
}
