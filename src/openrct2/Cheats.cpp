/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Cheats.h"

#include "GameState.h"
#include "actions/CheatSetAction.h"
#include "actions/ParkSetLoanAction.h"
#include "config/Config.h"
#include "core/DataSerialiser.h"
#include "localisation/Localisation.h"
#include "network/network.h"
#include "ride/Ride.h"
#include "scenario/Scenario.h"
#include "util/Util.h"
#include "world/Climate.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/Park.h"
#include "world/Scenery.h"
#include "world/Surface.h"

using namespace OpenRCT2;

// TODO: Refactor this. Cheat variables should contain the cheat type
// and a serialisation method.
bool gCheatsSandboxMode = false;
bool gCheatsDisableClearanceChecks = false;
bool gCheatsDisableSupportLimits = false;
bool gCheatsShowAllOperatingModes = false;
bool gCheatsShowVehiclesFromOtherTrackTypes = false;
bool gCheatsUnlockOperatingLimits = false;
bool gCheatsDisableBrakesFailure = false;
bool gCheatsDisableAllBreakdowns = false;
bool gCheatsBuildInPauseMode = false;
bool gCheatsIgnoreRideIntensity = false;
bool gCheatsDisableVandalism = false;
bool gCheatsDisableLittering = false;
bool gCheatsNeverendingMarketing = false;
bool gCheatsFreezeWeather = false;
bool gCheatsDisableTrainLengthLimit = false;
bool gCheatsDisablePlantAging = false;
bool gCheatsEnableChainLiftOnAllTrack = false;
bool gCheatsAllowArbitraryRideTypeChanges = false;
bool gCheatsDisableRideValueAging = false;
bool gCheatsIgnoreResearchStatus = false;
bool gCheatsEnableAllDrawableTrackPieces = false;
bool gCheatsAllowTrackPlaceInvalidHeights = false;
bool gCheatsAllowRegularPathAsQueue = false;

void CheatsReset()
{
    gCheatsSandboxMode = false;
    gCheatsDisableClearanceChecks = false;
    gCheatsDisableSupportLimits = false;
    gCheatsShowAllOperatingModes = false;
    gCheatsShowVehiclesFromOtherTrackTypes = false;
    gCheatsDisableTrainLengthLimit = false;
    gCheatsEnableChainLiftOnAllTrack = false;
    gCheatsUnlockOperatingLimits = false;
    gCheatsDisableBrakesFailure = false;
    gCheatsDisableAllBreakdowns = false;
    gCheatsBuildInPauseMode = false;
    gCheatsIgnoreRideIntensity = false;
    gCheatsDisableVandalism = false;
    gCheatsDisableLittering = false;
    gCheatsNeverendingMarketing = false;
    gCheatsFreezeWeather = false;
    gCheatsDisablePlantAging = false;
    gCheatsAllowArbitraryRideTypeChanges = false;
    gCheatsDisableRideValueAging = false;
    gCheatsIgnoreResearchStatus = false;
    gCheatsEnableAllDrawableTrackPieces = false;
    gCheatsAllowTrackPlaceInvalidHeights = false;
    gCheatsAllowRegularPathAsQueue = false;
}

void CheatsSet(CheatType cheatType, int32_t param1 /* = 0*/, int32_t param2 /* = 0*/)
{
    auto cheatSetAction = CheatSetAction(cheatType, param1, param2);
    GameActions::Execute(&cheatSetAction);
}

template<typename T> static void CheatEntrySerialise(DataSerialiser& ds, CheatType type, const T& value, uint16_t& count)
{
    ds << static_cast<int32_t>(type) << value;
    count++;
}

void CheatsSerialise(DataSerialiser& ds)
{
    uint16_t count = 0;

    if (ds.IsSaving())
    {
        OpenRCT2::IStream& stream = ds.GetStream();

        // Temporarily write 0, will be updated after every cheat is written.
        uint64_t countOffset = stream.GetPosition();
        ds << count;

        CheatEntrySerialise(ds, CheatType::SandboxMode, gCheatsSandboxMode, count);
        CheatEntrySerialise(ds, CheatType::DisableClearanceChecks, gCheatsDisableClearanceChecks, count);
        CheatEntrySerialise(ds, CheatType::DisableSupportLimits, gCheatsDisableSupportLimits, count);
        CheatEntrySerialise(ds, CheatType::ShowAllOperatingModes, gCheatsShowAllOperatingModes, count);
        CheatEntrySerialise(ds, CheatType::ShowVehiclesFromOtherTrackTypes, gCheatsShowVehiclesFromOtherTrackTypes, count);
        CheatEntrySerialise(ds, CheatType::FastLiftHill, gCheatsUnlockOperatingLimits, count);
        CheatEntrySerialise(ds, CheatType::DisableBrakesFailure, gCheatsDisableBrakesFailure, count);
        CheatEntrySerialise(ds, CheatType::DisableAllBreakdowns, gCheatsDisableAllBreakdowns, count);
        CheatEntrySerialise(ds, CheatType::BuildInPauseMode, gCheatsBuildInPauseMode, count);
        CheatEntrySerialise(ds, CheatType::IgnoreRideIntensity, gCheatsIgnoreRideIntensity, count);
        CheatEntrySerialise(ds, CheatType::DisableVandalism, gCheatsDisableVandalism, count);
        CheatEntrySerialise(ds, CheatType::DisableLittering, gCheatsDisableLittering, count);
        CheatEntrySerialise(ds, CheatType::NeverEndingMarketing, gCheatsNeverendingMarketing, count);
        CheatEntrySerialise(ds, CheatType::FreezeWeather, gCheatsFreezeWeather, count);
        CheatEntrySerialise(ds, CheatType::DisableTrainLengthLimit, gCheatsDisableTrainLengthLimit, count);
        CheatEntrySerialise(ds, CheatType::DisablePlantAging, gCheatsDisablePlantAging, count);
        CheatEntrySerialise(ds, CheatType::EnableChainLiftOnAllTrack, gCheatsEnableChainLiftOnAllTrack, count);
        CheatEntrySerialise(ds, CheatType::AllowArbitraryRideTypeChanges, gCheatsAllowArbitraryRideTypeChanges, count);
        CheatEntrySerialise(ds, CheatType::DisableRideValueAging, gCheatsDisableRideValueAging, count);
        CheatEntrySerialise(ds, CheatType::IgnoreResearchStatus, gCheatsIgnoreResearchStatus, count);
        CheatEntrySerialise(ds, CheatType::EnableAllDrawableTrackPieces, gCheatsEnableAllDrawableTrackPieces, count);
        CheatEntrySerialise(ds, CheatType::AllowTrackPlaceInvalidHeights, gCheatsAllowTrackPlaceInvalidHeights, count);
        CheatEntrySerialise(ds, CheatType::AllowRegularPathAsQueue, gCheatsAllowRegularPathAsQueue, count);

        // Remember current position and update count.
        uint64_t endOffset = stream.GetPosition();

        stream.SetPosition(countOffset);
        ds << count; // Write correct count.

        // Set position back.
        stream.SetPosition(endOffset);
    }
    else
    {
        ds << count;

        for (uint16_t i = 0; i < count; i++)
        {
            int32_t type = 0;
            ds << type;

            static bool dummyBool;

            switch (static_cast<CheatType>(type))
            {
                case CheatType::SandboxMode:
                    ds << gCheatsSandboxMode;
                    break;
                case CheatType::DisableClearanceChecks:
                    ds << gCheatsDisableClearanceChecks;
                    break;
                case CheatType::DisableSupportLimits:
                    ds << gCheatsDisableSupportLimits;
                    break;
                case CheatType::ShowAllOperatingModes:
                    ds << gCheatsShowAllOperatingModes;
                    break;
                case CheatType::ShowVehiclesFromOtherTrackTypes:
                    ds << gCheatsShowVehiclesFromOtherTrackTypes;
                    break;
                case CheatType::FastLiftHill:
                    ds << gCheatsUnlockOperatingLimits;
                    break;
                case CheatType::DisableBrakesFailure:
                    ds << gCheatsDisableBrakesFailure;
                    break;
                case CheatType::DisableAllBreakdowns:
                    ds << gCheatsDisableAllBreakdowns;
                    break;
                case CheatType::BuildInPauseMode:
                    ds << gCheatsBuildInPauseMode;
                    break;
                case CheatType::IgnoreRideIntensity:
                    ds << gCheatsIgnoreRideIntensity;
                    break;
                case CheatType::DisableVandalism:
                    ds << gCheatsDisableVandalism;
                    break;
                case CheatType::DisableLittering:
                    ds << gCheatsDisableLittering;
                    break;
                case CheatType::NeverEndingMarketing:
                    ds << gCheatsNeverendingMarketing;
                    break;
                case CheatType::FreezeWeather:
                    ds << gCheatsFreezeWeather;
                    break;
                case CheatType::DisableTrainLengthLimit:
                    ds << gCheatsDisableTrainLengthLimit;
                    break;
                case CheatType::DisablePlantAging:
                    ds << gCheatsDisablePlantAging;
                    break;
                case CheatType::EnableChainLiftOnAllTrack:
                    ds << gCheatsEnableChainLiftOnAllTrack;
                    break;
                case CheatType::AllowArbitraryRideTypeChanges:
                    ds << gCheatsAllowArbitraryRideTypeChanges;
                    break;
                case CheatType::DisableRideValueAging:
                    ds << gCheatsDisableRideValueAging;
                    break;
                case CheatType::IgnoreResearchStatus:
                    ds << gCheatsIgnoreResearchStatus;
                    break;
                case CheatType::EnableAllDrawableTrackPieces:
                    ds << gCheatsEnableAllDrawableTrackPieces;
                    break;
                case CheatType::AllowTrackPlaceInvalidHeights:
                    ds << gCheatsAllowTrackPlaceInvalidHeights;
                    break;
                case CheatType::NoCapOnQueueLengthDummy:
                    ds << dummyBool;
                    break;
                case CheatType::AllowRegularPathAsQueue:
                    ds << gCheatsAllowRegularPathAsQueue;
                    break;
                default:
                    break;
            }
        }
    }
}

const char* CheatsGetName(CheatType cheatType)
{
    switch (cheatType)
    {
        case CheatType::SandboxMode:
            return LanguageGetString(STR_CHEAT_SANDBOX_MODE);
        case CheatType::DisableClearanceChecks:
            return LanguageGetString(STR_DISABLE_CLEARANCE_CHECKS);
        case CheatType::DisableSupportLimits:
            return LanguageGetString(STR_DISABLE_SUPPORT_LIMITS);
        case CheatType::ShowAllOperatingModes:
            return LanguageGetString(STR_CHEAT_SHOW_ALL_OPERATING_MODES);
        case CheatType::ShowVehiclesFromOtherTrackTypes:
            return LanguageGetString(STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES);
        case CheatType::FastLiftHill:
            return LanguageGetString(STR_CHEAT_UNLOCK_OPERATING_LIMITS);
        case CheatType::DisableBrakesFailure:
            return LanguageGetString(STR_CHEAT_DISABLE_BRAKES_FAILURE);
        case CheatType::DisableAllBreakdowns:
            return LanguageGetString(STR_CHEAT_DISABLE_BREAKDOWNS);
        case CheatType::DisableTrainLengthLimit:
            return LanguageGetString(STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT);
        case CheatType::EnableChainLiftOnAllTrack:
            return LanguageGetString(STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK);
        case CheatType::BuildInPauseMode:
            return LanguageGetString(STR_CHEAT_BUILD_IN_PAUSE_MODE);
        case CheatType::IgnoreRideIntensity:
            return LanguageGetString(STR_CHEAT_IGNORE_INTENSITY);
        case CheatType::DisableVandalism:
            return LanguageGetString(STR_CHEAT_DISABLE_VANDALISM);
        case CheatType::DisableLittering:
            return LanguageGetString(STR_CHEAT_DISABLE_LITTERING);
        case CheatType::NoMoney:
            return LanguageGetString(STR_MAKE_PARK_NO_MONEY);
        case CheatType::AddMoney:
            return LanguageGetString(STR_LOG_CHEAT_ADD_MONEY);
        case CheatType::ClearLoan:
            return LanguageGetString(STR_CHEAT_CLEAR_LOAN);
        case CheatType::SetGuestParameter:
            return LanguageGetString(STR_CHEAT_SET_GUESTS_PARAMETERS);
        case CheatType::GenerateGuests:
            return LanguageGetString(STR_CHEAT_LARGE_TRAM_GUESTS);
        case CheatType::RemoveAllGuests:
            return LanguageGetString(STR_CHEAT_REMOVE_ALL_GUESTS);
        case CheatType::GiveAllGuests:
            return LanguageGetString(STR_CHEAT_GIVE_ALL_GUESTS);
        case CheatType::SetGrassLength:
            return LanguageGetString(STR_CHEAT_CLEAR_GRASS);
        case CheatType::WaterPlants:
            return LanguageGetString(STR_CHEAT_WATER_PLANTS);
        case CheatType::FixVandalism:
            return LanguageGetString(STR_CHEAT_FIX_VANDALISM);
        case CheatType::RemoveLitter:
            return LanguageGetString(STR_CHEAT_REMOVE_LITTER);
        case CheatType::DisablePlantAging:
            return LanguageGetString(STR_CHEAT_DISABLE_PLANT_AGING);
        case CheatType::SetStaffSpeed:
            return LanguageGetString(STR_CHEAT_STAFF_SPEED);
        case CheatType::RenewRides:
            return LanguageGetString(STR_CHEAT_RENEW_RIDES);
        case CheatType::MakeDestructible:
            return LanguageGetString(STR_CHEAT_MAKE_DESTRUCTABLE);
        case CheatType::FixRides:
            return LanguageGetString(STR_CHEAT_FIX_ALL_RIDES);
        case CheatType::ResetCrashStatus:
            return LanguageGetString(STR_CHEAT_RESET_CRASH_STATUS);
        case CheatType::TenMinuteInspections:
            return LanguageGetString(STR_CHEAT_10_MINUTE_INSPECTIONS);
        case CheatType::WinScenario:
            return LanguageGetString(STR_CHEAT_WIN_SCENARIO);
        case CheatType::ForceWeather:
            return LanguageGetString(STR_CHANGE_WEATHER);
        case CheatType::FreezeWeather:
            return LanguageGetString(STR_CHEAT_FREEZE_WEATHER);
        case CheatType::NeverEndingMarketing:
            return LanguageGetString(STR_CHEAT_NEVERENDING_MARKETING);
        case CheatType::OpenClosePark:
            return LanguageGetString(STR_CHEAT_OPEN_PARK);
        case CheatType::HaveFun:
            return LanguageGetString(STR_CHEAT_HAVE_FUN);
        case CheatType::SetForcedParkRating:
            return LanguageGetString(STR_FORCE_PARK_RATING);
        case CheatType::AllowArbitraryRideTypeChanges:
            return LanguageGetString(STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES);
        case CheatType::SetMoney:
            return LanguageGetString(STR_SET_MONEY);
        case CheatType::OwnAllLand:
            return LanguageGetString(STR_CHEAT_OWN_ALL_LAND);
        case CheatType::DisableRideValueAging:
            return LanguageGetString(STR_CHEAT_DISABLE_RIDE_VALUE_AGING);
        case CheatType::IgnoreResearchStatus:
            return LanguageGetString(STR_CHEAT_IGNORE_RESEARCH_STATUS);
        case CheatType::EnableAllDrawableTrackPieces:
            return LanguageGetString(STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES);
        case CheatType::AllowTrackPlaceInvalidHeights:
            return LanguageGetString(STR_CHEAT_ALLOW_TRACK_PLACE_INVALID_HEIGHTS);
        case CheatType::AllowRegularPathAsQueue:
            return LanguageGetString(STR_CHEAT_ALLOW_PATH_AS_QUEUE);
        default:
            return "Unknown Cheat";
    }
}
