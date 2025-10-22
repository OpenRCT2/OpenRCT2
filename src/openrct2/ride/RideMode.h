/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "../core/EnumUtils.hpp"
#include "../core/Money.hpp"
#include "../localisation/StringIds.h"
#include "RideRatings.h"
#include "Vehicle.h"

namespace OpenRCT2
{
    static const enum class RideModeID : uint8_t
    {
        NORMAL,
        CONTINUOUS_CIRCUIT,
        REVERSE_INCLINE_LAUNCHED_SHUTTLE,
        POWERED_LAUNCH_PASSTHROUGH,
        SHUTTLE,
        BOAT_HIRE,
        UPWARD_LAUNCH,
        ROTATING_LIFT,
        STATION_TO_STATION,
        SINGLE_RIDE_PER_ADMISSION,
        UNLIMITED_RIDES_PER_ADMISSION,
        MAZE,
        RACE,
        DODGEMS,
        SWING,
        SHOP_STALL,
        ROTATION,
        FORWARD_ROTATION,
        BACKWARD_ROTATION,
        FILM_AVENGING_AVIATORS,
        FILM_MOUSE_TAILS,
        SPACE_RINGS,
        BEGINNERS,
        LIM_POWERED_LAUNCH,
        FILM_THRILL_RIDERS,
        FILM_STORM_CHASERS,
        FILM_SPACE_RAIDERS,
        INTENSE,
        BERSERK,
        HAUNTED_HOUSE,
        CIRCUS_SHOW,
        DOWNWARD_LAUNCH,
        CROOKED_HOUSE,
        FREEFALL_DROP,
        CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
        POWERED_LAUNCH,
        POWERED_LAUNCH_BLOCK_SECTIONED,
        COUNT,
    };

    enum : uint8_t
    {
        RIDE_MODE_FLAG_NONE = 0,
        RIDE_MODE_FLAG_IS_BLOCK_SECTIONED = (1 << 0),
        RIDE_MODE_FLAG_IS_POWERED_LAUNCH = (1 << 1),
        RIDE_MODE_FLAG_IS_FERRIS_WHEEL = (1 << 2),
        RIDE_MODE_FLAG_SINGLE_TRAIN = (1 << 3), // Mode only supports a single train
    };

    using StationUpdateFunction = void (*)(Ride&, StationIndex);
    using VehicleUpdateFunction = void (*)(Vehicle*, Ride*);
    using VehicleCarEntryUpdateFunction = void (*)(Vehicle*, Ride*, CarEntry);

    // Number of stations supported by the mode
    enum : uint8_t
    {
        RIDE_MODE_STATIONS_ANY,
        RIDE_MODE_STATIONS_ONLY_ONE,
        RIDE_MODE_STATIONS_MORE_THAN_ONE,
    };

    // Window UI Mode
    enum : uint8_t
    {
        RIDE_MODE_UI_NORMAL,
        RIDE_MODE_UI_LAUNCH,
        RIDE_MODE_UI_SWING,
        RIDE_MODE_UI_ROTATION,
        RIDE_MODE_UI_DODGEMS,
        RIDE_MODE_UI_RACE,
        RIDE_MODE_UI_CHAIRLIFT,
    };

    struct RideMode
    {
        RideModeID Index{};
        StringId Name{};
        RideRating::Tuple StatBonus{};
        money64 UpkeepCost{};
        uint64_t Flags{};
        uint8_t StationType{};
        uint8_t ModeSubstate{}; // Used by Top Spin, Motion Sim, & 3D Cinema
        StationUpdateFunction FuncUpdateStation{};
        VehicleUpdateFunction FuncUpdateMovingToEndOfStation{};
        VehicleUpdateFunction FuncUpdateWaitingToDepart{};
        VehicleCarEntryUpdateFunction FuncUpdateDeparting{};
        VehicleUpdateFunction FuncUpdateArriving{};
        bool TweakUITextInput{};
        uint8_t UIOptionFormat{};
        StringId ErrorMessage{};

        friend bool operator==(const RideMode a, const RideMode b)
        {
            return &a == &b || a.Index == b.Index;
        }

        bool hasFlag(uint8_t flag) const
        {
            return Flags & flag;
        }
    };

    namespace RideModes
    {
        constexpr RideRating::Tuple kStatBonusNone = RideRating::makeTuple(0, 0, 0, 0, 0, 0);

    // clang-format off
    constexpr RideMode normal = {
        .Index = RideModeID::NORMAL,
        .Name = STR_RIDE_MODE_NORMAL, 
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode continuousCircuit = {
        .Index = RideModeID::CONTINUOUS_CIRCUIT,
        .Name = STR_RIDE_MODE_CONTINUOUS_CIRCUIT,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingCircuit,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage =  0
    };
    constexpr RideMode reverseInclineLaunchedShuttle = {
        .Index = RideModeID::REVERSE_INCLINE_LAUNCHED_SHUTTLE,
        .Name = STR_RIDE_MODE_REVERSE_INCLINE_LAUNCHED_SHUTTLE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 30,
        .Flags = RIDE_MODE_FLAG_SINGLE_TRAIN,
        .StationType = RIDE_MODE_STATIONS_ONLY_ONE,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingReverseInclineShuttle,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage =  0
    };
    constexpr RideMode poweredLaunchPassthrough = {
        .Index = RideModeID::POWERED_LAUNCH_PASSTHROUGH,
        .Name = STR_RIDE_MODE_POWERED_LAUNCH_PASSTROUGH,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 160,
        .Flags = RIDE_MODE_FLAG_IS_POWERED_LAUNCH | RIDE_MODE_FLAG_SINGLE_TRAIN,
        .StationType = RIDE_MODE_STATIONS_ONLY_ONE,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingLaunch,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = false,
        .UIOptionFormat = RIDE_MODE_UI_LAUNCH,
        .ErrorMessage = 0
    };
    constexpr RideMode shuttle = {
        .Index = RideModeID::SHUTTLE,
        .Name =  STR_RIDE_MODE_SHUTTLE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_SINGLE_TRAIN,
        .StationType = RIDE_MODE_STATIONS_MORE_THAN_ONE,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode boatHire = {
        .Index = RideModeID::BOAT_HIRE,
        .Name =  STR_RIDE_MODE_BOAT_HIRE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingCircuit,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode upwardLaunch = {
        .Index = RideModeID::UPWARD_LAUNCH,
        .Name =  STR_RIDE_MODE_UPWARD_LAUNCH,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingLaunch,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = false,
        .UIOptionFormat = RIDE_MODE_UI_LAUNCH,
        .ErrorMessage = 0
    };
    constexpr RideMode rotatingLift = {
        .Index = RideModeID::ROTATING_LIFT,
        .Name =  STR_RIDE_MODE_ROTATING_LIFT,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingCircuit,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode stationToStation = {
        .Index = RideModeID::STATION_TO_STATION,
        .Name =  STR_RIDE_MODE_STATION_TO_STATION,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = false,
        .UIOptionFormat = RIDE_MODE_UI_CHAIRLIFT,
        .ErrorMessage = STR_CANT_CHANGE_SPEED
    };
    constexpr RideMode singleRidePerAdmission = {
        .Index = RideModeID::SINGLE_RIDE_PER_ADMISSION,
        .Name =  STR_RIDE_MODE_SINGLE_RIDE_PER_ADMISSION,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode unlimitedRidesPerAdmission = {
        .Index = RideModeID::UNLIMITED_RIDES_PER_ADMISSION,
        .Name = STR_RIDE_MODE_UNLIMITED_RIDES_PER_ADMISSION,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode maze = {
        .Index = RideModeID::MAZE, 
        .Name = STR_RIDE_MODE_MAZE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode race = {
        .Index = RideModeID::RACE, 
        .Name = STR_RIDE_MODE_RACE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationRace,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_RACE,
        .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_LAPS
    };
    constexpr RideMode dodgems = {
        .Index = RideModeID::DODGEMS, 
        .Name = STR_RIDE_MODE_DODGEMS,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationDodgems,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartDodgems,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = false,
        .UIOptionFormat = RIDE_MODE_UI_DODGEMS,
        .ErrorMessage = STR_CANT_CHANGE_TIME_LIMIT
    };
    constexpr RideMode swing = {
        .Index = RideModeID::SWING, 
        .Name = STR_RIDE_MODE_SWING,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartSwing,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_SWING,
        .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_SWINGS
    };
    constexpr RideMode shopStall = {
        .Index = RideModeID::SHOP_STALL, 
        .Name = STR_RIDE_MODE_SHOP_STALL,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode rotation = {
        .Index = RideModeID::ROTATION, 
        .Name = STR_RIDE_MODE_ROTATION,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartRotation,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_ROTATION,
        .ErrorMessage = 0
    };
    constexpr RideMode forwardRotation = {
        .Index = RideModeID::FORWARD_ROTATION, 
        .Name = STR_RIDE_MODE_FORWARD_ROTATION,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_IS_FERRIS_WHEEL,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartFerrisWheel,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_ROTATION,
        .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_ROTATIONS
    };
    constexpr RideMode backwardRotation = {
        .Index = RideModeID::BACKWARD_ROTATION, 
        .Name = STR_RIDE_MODE_BACKWARD_ROTATION,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_IS_FERRIS_WHEEL,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartFerrisWheel,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_ROTATION,
        .ErrorMessage = STR_CANT_CHANGE_NUMBER_OF_ROTATIONS
    };
    constexpr RideMode filmAvengingAviators = {
        .Index = RideModeID::FILM_AVENGING_AVIATORS, 
        .Name = STR_RIDE_MODE_FILM_AVENGING_AVIATORS,
        .StatBonus = RideRating::makeTuple(2, 90, 3, 50, 3, 00),
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartMotionSim,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode mouseTails3DFilm = {
        .Index = RideModeID::FILM_MOUSE_TAILS, 
        .Name = STR_RIDE_MODE_3D_FILM_MOUSE_TAILS,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepart3DCinema,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode spaceRings = {
        .Index = RideModeID::SPACE_RINGS, 
        .Name = STR_RIDE_MODE_SPACE_RINGS,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartSpaceRings,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode beginners = {
        .Index = RideModeID::BEGINNERS, 
        .Name = STR_RIDE_MODE_BEGINNERS,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartTopSpin,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .ErrorMessage = 0
    };
    constexpr RideMode limPoweredLaunch = {
        .Index = RideModeID::LIM_POWERED_LAUNCH, 
        .Name = STR_RIDE_MODE_LIM_POWERED_LAUNCH,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 320,
        .Flags = RIDE_MODE_FLAG_SINGLE_TRAIN,
        .StationType = RIDE_MODE_STATIONS_ONLY_ONE,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingLaunch,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode filmThrillRiders = {
        .Index = RideModeID::FILM_THRILL_RIDERS, 
        .Name = STR_RIDE_MODE_FILM_THRILL_RIDERS,
        .StatBonus = RideRating::makeTuple(3, 25, 4, 10, 3, 30),
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 1,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartMotionSim,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode stormChasers3DFilm = {
        .Index = RideModeID::FILM_STORM_CHASERS, 
        .Name = STR_RIDE_MODE_3D_FILM_STORM_CHASERS,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 1,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepart3DCinema,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode spaceRaiders3DFilm = {
        .Index = RideModeID::FILM_SPACE_RAIDERS, 
        .Name = STR_RIDE_MODE_3D_FILM_SPACE_RAIDERS,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 2,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepart3DCinema,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode intense = {
        .Index = RideModeID::INTENSE, 
        .Name = STR_RIDE_MODE_INTENSE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 1,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartTopSpin,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode berserk = {
        .Index = RideModeID::BERSERK, 
        .Name = STR_RIDE_MODE_BERSERK,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 2,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartTopSpin,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode hauntedHouse = {
        .Index = RideModeID::HAUNTED_HOUSE, 
        .Name = STR_RIDE_MODE_HAUNTED_HOUSE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartHauntedHouse,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode circus = {
        .Index = RideModeID::CIRCUS_SHOW, 
        .Name = STR_RIDE_MODE_CIRCUS_SHOW,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartCircus,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode downwardLaunch = {
        .Index = RideModeID::DOWNWARD_LAUNCH, 
        .Name = STR_RIDE_MODE_DOWNWARD_LAUNCH,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingDownwardLaunch,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode crookedHouse = {
        .Index = RideModeID::CROOKED_HOUSE, 
        .Name = STR_RIDE_MODE_CROOKED_HOUSE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationFlatRide,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartCrookedHouse,
        .FuncUpdateArriving = UpdateArrivingFlatRide,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode freefallDrop = {
        .Index = RideModeID::FREEFALL_DROP, 
        .Name = STR_RIDE_MODE_FREEFALL_DROP,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_NONE,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationTower,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingCircuit,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode continuousCircuitBlockSectioned = {
        .Index = RideModeID::CONTINUOUS_CIRCUIT_BLOCK_SECTIONED, 
        .Name = STR_RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 0,
        .Flags = RIDE_MODE_FLAG_IS_BLOCK_SECTIONED,
        .StationType = RIDE_MODE_STATIONS_ANY,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationBlockSection,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingCircuit,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = true,
        .UIOptionFormat = RIDE_MODE_UI_NORMAL,
        .ErrorMessage = 0
    };
    constexpr RideMode poweredLaunch = {
        .Index = RideModeID::POWERED_LAUNCH, 
        .Name = STR_RIDE_MODE_POWERED_LAUNCH,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 220,
        .Flags = RIDE_MODE_FLAG_IS_POWERED_LAUNCH | RIDE_MODE_FLAG_SINGLE_TRAIN,
        .StationType = RIDE_MODE_STATIONS_ONLY_ONE,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationNormal,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingLaunch,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = false,
        .UIOptionFormat = RIDE_MODE_UI_LAUNCH,
        .ErrorMessage = 0
    };
    constexpr RideMode poweredLaunchBlockSectioned = {
        .Index = RideModeID::POWERED_LAUNCH_BLOCK_SECTIONED, 
        .Name = STR_RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED_MODE,
        .StatBonus = kStatBonusNone,
        .UpkeepCost = 220,
        .Flags = RIDE_MODE_FLAG_IS_BLOCK_SECTIONED | RIDE_MODE_FLAG_IS_POWERED_LAUNCH,
        .StationType = RIDE_MODE_STATIONS_ONLY_ONE,
        .ModeSubstate = 0,
        .FuncUpdateStation = RideUpdateStationBlockSection,
        .FuncUpdateMovingToEndOfStation = UpdateMovingToEndOfStationNormal,
        .FuncUpdateWaitingToDepart = UpdateWaitingToDepartNormal,
        .FuncUpdateDeparting = UpdateDepartingLaunch,
        .FuncUpdateArriving = UpdateArrivingNormal,
        .TweakUITextInput = false,
        .UIOptionFormat = RIDE_MODE_UI_LAUNCH,
        .ErrorMessage = 0
    };

    constexpr RideMode kOperatingModes[static_cast<uint8_t>(RideModeID::COUNT)] = {
        normal,
        continuousCircuit,
        reverseInclineLaunchedShuttle,
        poweredLaunchPassthrough,
        shuttle,
        boatHire,
        upwardLaunch,
        rotatingLift,
        stationToStation,
        singleRidePerAdmission,
        unlimitedRidesPerAdmission,
        maze,
        race,
        dodgems,
        swing,
        shopStall,
        rotation,
        forwardRotation,
        backwardRotation,
        filmAvengingAviators,
        mouseTails3DFilm,
        spaceRings,
        beginners,
        limPoweredLaunch,
        filmThrillRiders,
        stormChasers3DFilm,
        spaceRaiders3DFilm,
        intense,
        berserk,
        hauntedHouse,
        circus,
        downwardLaunch,
        crookedHouse,
        freefallDrop,
        continuousCircuitBlockSectioned,
        poweredLaunch,
        poweredLaunchBlockSectioned,
    };
        // clang-format on

        RideMode FromID(RideModeID index);

        RideMode FromIndex(uint8_t index);

        uint8_t ToIndex(RideMode mode);

        uint64_t ToFlags(std::vector<RideMode> modes);

        constexpr uint64_t IndexToFlags(std::vector<RideModeID> modeID)
        {
            uint64_t result = 0;
            for (RideModeID m : modeID)
            {
                result |= (uint64_t(1) << static_cast<uint8_t>(m));
            }
            return result;
        }

        RideMode GetBlockSectionedCounterpart(RideMode originalMode);

        RideMode GetNonBlockSectionedCounterpart(RideMode originalMode, RideModeID defaultMode);

    } // namespace RideModes
} // namespace OpenRCT2
