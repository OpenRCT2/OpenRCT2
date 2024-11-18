/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "core/Money.hpp"

enum class StaffSpeedCheat
{
    None,
    Frozen,
    Fast,
};

struct CheatsState
{
    bool SandboxMode;
    bool DisableClearanceChecks;
    bool DisableSupportLimits;
    bool ShowAllOperatingModes;
    bool ShowVehiclesFromOtherTrackTypes;
    bool UnlockOperatingLimits;
    bool DisableBrakesFailure;
    bool DisableAllBreakdowns;
    bool BuildInPauseMode;
    bool IgnoreRideIntensity;
    bool IgnorePrice;
    bool DisableVandalism;
    bool DisableLittering;
    bool NeverendingMarketing;
    bool FreezeWeather;
    bool DisableTrainLengthLimit;
    bool DisablePlantAging;
    bool DisableRideValueAging;
    bool EnableChainLiftOnAllTrack;
    bool AllowArbitraryRideTypeChanges;
    bool IgnoreResearchStatus;
    bool EnableAllDrawableTrackPieces;
    bool AllowTrackPlaceInvalidHeights;
    bool AllowRegularPathAsQueue;
    bool AllowSpecialColourSchemes;
    bool MakeAllDestructible;
    StaffSpeedCheat SelectedStaffSpeed;
    int32_t forcedParkRating;
};

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
    NoCapOnQueueLengthDummy, // Removed; this dummy exists only for deserialisation parks that had it saved
    AllowRegularPathAsQueue,
    AllowSpecialColourSchemes,
    RemoveParkFences,
    IgnorePrice,
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

constexpr auto kCheatsGiveGuestsMoney = 1000.00_GBP;
constexpr int kCheatsTramIncrement = 250;
constexpr int kCheatsDuckIncrement = 20;
constexpr int kCheatsStaffFastSpeed = 0xFF;
constexpr int kCheatsStaffNormalSpeed = 0x60;
constexpr int kCheatsStaffFreezeSpeed = 0;
constexpr int32_t kForcedParkRatingDisabled = -1;

void CheatsReset();
const char* CheatsGetName(CheatType cheatType);
void CheatsSet(CheatType cheatType, int64_t param1 = 0, int64_t param2 = 0);
void CheatsSerialise(class DataSerialiser& ds);
