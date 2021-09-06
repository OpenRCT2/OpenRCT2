/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

extern bool gCheatsSandboxMode;
extern bool gCheatsDisableClearanceChecks;
extern bool gCheatsDisableSupportLimits;
extern bool gCheatsShowAllOperatingModes;
extern bool gCheatsShowVehiclesFromOtherTrackTypes;
extern bool gCheatsUnlockOperatingLimits;
extern bool gCheatsDisableBrakesFailure;
extern bool gCheatsDisableAllBreakdowns;
extern bool gCheatsBuildInPauseMode;
extern bool gCheatsIgnoreRideIntensity;
extern bool gCheatsDisableVandalism;
extern bool gCheatsDisableLittering;
extern bool gCheatsNeverendingMarketing;
extern bool gCheatsFreezeWeather;
extern bool gCheatsDisableTrainLengthLimit;
extern bool gCheatsDisablePlantAging;
extern bool gCheatsDisableRideValueAging;
extern bool gCheatsEnableChainLiftOnAllTrack;
extern bool gCheatsAllowArbitraryRideTypeChanges;
extern bool gCheatsIgnoreResearchStatus;
extern bool gCheatsEnableAllDrawableTrackPieces;
extern bool gCheatsAllowTrackPlaceInvalidHeights;
extern bool gCheatsKeepDisconnectedRideEntrances;

enum class CheatType : int32_t
{
    SandboxMode,
    DisableClearanceChecks,
    DisableSupportLimits,
    ShowAllOperatingModes,
    ShowVehiclesFromOtherTrackTypes,
    DisableTrainLengthLimit,
    EnableChainLiftOnAllTrack,
    FastLiftHill,
    DisableBrakesFailure,
    DisableAllBreakdowns,
    UnlockAllPrices,
    BuildInPauseMode,
    IgnoreRideIntensity,
    DisableVandalism,
    DisableLittering,
    NoMoney,
    AddMoney,
    SetMoney,
    ClearLoan,
    SetGuestParameter,
    GenerateGuests,
    RemoveAllGuests,
    GiveAllGuests,
    SetGrassLength,
    WaterPlants,
    DisablePlantAging,
    FixVandalism,
    RemoveLitter,
    SetStaffSpeed,
    RenewRides,
    MakeDestructible,
    FixRides,
    ResetCrashStatus,
    TenMinuteInspections,
    WinScenario,
    ForceWeather,
    FreezeWeather,
    OpenClosePark,
    HaveFun,
    SetForcedParkRating,
    NeverEndingMarketing,
    AllowArbitraryRideTypeChanges,
    OwnAllLand,
    DisableRideValueAging,
    IgnoreResearchStatus,
    EnableAllDrawableTrackPieces,
    CreateDucks,
    RemoveDucks,
    AllowTrackPlaceInvalidHeights,
    KeepDisconnectedRideEntrances,
    Count,
};

enum
{
    GUEST_PARAMETER_HAPPINESS,
    GUEST_PARAMETER_ENERGY,
    GUEST_PARAMETER_HUNGER,
    GUEST_PARAMETER_THIRST,
    GUEST_PARAMETER_NAUSEA,
    GUEST_PARAMETER_NAUSEA_TOLERANCE,
    GUEST_PARAMETER_TOILET,
    GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY
};

enum
{
    OBJECT_MONEY,
    OBJECT_PARK_MAP,
    OBJECT_BALLOON,
    OBJECT_UMBRELLA
};

#define CHEATS_GIVE_GUESTS_MONEY MONEY(1000, 00)
#define CHEATS_TRAM_INCREMENT 250
#define CHEATS_DUCK_INCREMENT 20
#define CHEATS_STAFF_FAST_SPEED 0xFF
#define CHEATS_STAFF_NORMAL_SPEED 0x60
#define CHEATS_STAFF_FREEZE_SPEED 0

void CheatsReset();
const char* CheatsGetName(CheatType cheatType);
void CheatsSet(CheatType cheatType, int32_t param1 = 0, int32_t param2 = 0);
void CheatsSerialise(class DataSerialiser& ds);
