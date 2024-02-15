/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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

void CheatsReset()
{
    GetGameState().Cheats.SandboxMode = false;
    GetGameState().Cheats.DisableClearanceChecks = false;
    GetGameState().Cheats.DisableSupportLimits = false;
    GetGameState().Cheats.ShowAllOperatingModes = false;
    GetGameState().Cheats.ShowVehiclesFromOtherTrackTypes = false;
    GetGameState().Cheats.DisableTrainLengthLimit = false;
    GetGameState().Cheats.EnableChainLiftOnAllTrack = false;
    GetGameState().Cheats.UnlockOperatingLimits = false;
    GetGameState().Cheats.DisableBrakesFailure = false;
    GetGameState().Cheats.DisableAllBreakdowns = false;
    GetGameState().Cheats.BuildInPauseMode = false;
    GetGameState().Cheats.IgnoreRideIntensity = false;
    GetGameState().Cheats.DisableVandalism = false;
    GetGameState().Cheats.DisableLittering = false;
    GetGameState().Cheats.NeverendingMarketing = false;
    GetGameState().Cheats.FreezeWeather = false;
    GetGameState().Cheats.DisablePlantAging = false;
    GetGameState().Cheats.AllowArbitraryRideTypeChanges = false;
    GetGameState().Cheats.DisableRideValueAging = false;
    GetGameState().Cheats.IgnoreResearchStatus = false;
    GetGameState().Cheats.EnableAllDrawableTrackPieces = false;
    GetGameState().Cheats.AllowTrackPlaceInvalidHeights = false;
    GetGameState().Cheats.AllowRegularPathAsQueue = false;
    GetGameState().Cheats.AllowSpecialColourSchemes = false;
    GetGameState().Cheats.MakeAllDestructible = false;
    GetGameState().Cheats.SelectedStaffSpeed = StaffSpeedCheat::None;
}

void CheatsSet(CheatType cheatType, int64_t param1 /* = 0*/, int64_t param2 /* = 0*/)
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

        CheatEntrySerialise(ds, CheatType::SandboxMode, GetGameState().Cheats.SandboxMode, count);
        CheatEntrySerialise(ds, CheatType::DisableClearanceChecks, GetGameState().Cheats.DisableClearanceChecks, count);
        CheatEntrySerialise(ds, CheatType::DisableSupportLimits, GetGameState().Cheats.DisableSupportLimits, count);
        CheatEntrySerialise(ds, CheatType::ShowAllOperatingModes, GetGameState().Cheats.ShowAllOperatingModes, count);
        CheatEntrySerialise(ds, CheatType::ShowVehiclesFromOtherTrackTypes, GetGameState().Cheats.ShowVehiclesFromOtherTrackTypes, count);
        CheatEntrySerialise(ds, CheatType::FastLiftHill, GetGameState().Cheats.UnlockOperatingLimits, count);
        CheatEntrySerialise(ds, CheatType::DisableBrakesFailure, GetGameState().Cheats.DisableBrakesFailure, count);
        CheatEntrySerialise(ds, CheatType::DisableAllBreakdowns, GetGameState().Cheats.DisableAllBreakdowns, count);
        CheatEntrySerialise(ds, CheatType::BuildInPauseMode, GetGameState().Cheats.BuildInPauseMode, count);
        CheatEntrySerialise(ds, CheatType::IgnoreRideIntensity, GetGameState().Cheats.IgnoreRideIntensity, count);
        CheatEntrySerialise(ds, CheatType::DisableVandalism, GetGameState().Cheats.DisableVandalism, count);
        CheatEntrySerialise(ds, CheatType::DisableLittering, GetGameState().Cheats.DisableLittering, count);
        CheatEntrySerialise(ds, CheatType::NeverEndingMarketing, GetGameState().Cheats.NeverendingMarketing, count);
        CheatEntrySerialise(ds, CheatType::FreezeWeather, GetGameState().Cheats.FreezeWeather, count);
        CheatEntrySerialise(ds, CheatType::DisableTrainLengthLimit, GetGameState().Cheats.DisableTrainLengthLimit, count);
        CheatEntrySerialise(ds, CheatType::DisablePlantAging, GetGameState().Cheats.DisablePlantAging, count);
        CheatEntrySerialise(ds, CheatType::EnableChainLiftOnAllTrack, GetGameState().Cheats.EnableChainLiftOnAllTrack, count);
        CheatEntrySerialise(ds, CheatType::AllowArbitraryRideTypeChanges, GetGameState().Cheats.AllowArbitraryRideTypeChanges, count);
        CheatEntrySerialise(ds, CheatType::DisableRideValueAging, GetGameState().Cheats.DisableRideValueAging, count);
        CheatEntrySerialise(ds, CheatType::IgnoreResearchStatus, GetGameState().Cheats.IgnoreResearchStatus, count);
        CheatEntrySerialise(ds, CheatType::EnableAllDrawableTrackPieces, GetGameState().Cheats.EnableAllDrawableTrackPieces, count);
        CheatEntrySerialise(
            ds, CheatType::AllowTrackPlaceInvalidHeights, GetGameState().Cheats.AllowTrackPlaceInvalidHeights, count);
        CheatEntrySerialise(ds, CheatType::AllowRegularPathAsQueue, GetGameState().Cheats.AllowRegularPathAsQueue, count);
        CheatEntrySerialise(ds, CheatType::AllowSpecialColourSchemes, GetGameState().Cheats.AllowSpecialColourSchemes, count);
        CheatEntrySerialise(ds, CheatType::MakeDestructible, GetGameState().Cheats.MakeAllDestructible, count);
        CheatEntrySerialise(ds, CheatType::SetStaffSpeed, GetGameState().Cheats.SelectedStaffSpeed, count);

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
                    ds << GetGameState().Cheats.SandboxMode;
                    break;
                case CheatType::DisableClearanceChecks:
                    ds << GetGameState().Cheats.DisableClearanceChecks;
                    break;
                case CheatType::DisableSupportLimits:
                    ds << GetGameState().Cheats.DisableSupportLimits;
                    break;
                case CheatType::ShowAllOperatingModes:
                    ds << GetGameState().Cheats.ShowAllOperatingModes;
                    break;
                case CheatType::ShowVehiclesFromOtherTrackTypes:
                    ds << GetGameState().Cheats.ShowVehiclesFromOtherTrackTypes;
                    break;
                case CheatType::FastLiftHill:
                    ds << GetGameState().Cheats.UnlockOperatingLimits;
                    break;
                case CheatType::DisableBrakesFailure:
                    ds << GetGameState().Cheats.DisableBrakesFailure;
                    break;
                case CheatType::DisableAllBreakdowns:
                    ds << GetGameState().Cheats.DisableAllBreakdowns;
                    break;
                case CheatType::BuildInPauseMode:
                    ds << GetGameState().Cheats.BuildInPauseMode;
                    break;
                case CheatType::IgnoreRideIntensity:
                    ds << GetGameState().Cheats.IgnoreRideIntensity;
                    break;
                case CheatType::DisableVandalism:
                    ds << GetGameState().Cheats.DisableVandalism;
                    break;
                case CheatType::DisableLittering:
                    ds << GetGameState().Cheats.DisableLittering;
                    break;
                case CheatType::NeverEndingMarketing:
                    ds << GetGameState().Cheats.NeverendingMarketing;
                    break;
                case CheatType::FreezeWeather:
                    ds << GetGameState().Cheats.FreezeWeather;
                    break;
                case CheatType::DisableTrainLengthLimit:
                    ds << GetGameState().Cheats.DisableTrainLengthLimit;
                    break;
                case CheatType::DisablePlantAging:
                    ds << GetGameState().Cheats.DisablePlantAging;
                    break;
                case CheatType::EnableChainLiftOnAllTrack:
                    ds << GetGameState().Cheats.EnableChainLiftOnAllTrack;
                    break;
                case CheatType::AllowArbitraryRideTypeChanges:
                    ds << GetGameState().Cheats.AllowArbitraryRideTypeChanges;
                    break;
                case CheatType::DisableRideValueAging:
                    ds << GetGameState().Cheats.DisableRideValueAging;
                    break;
                case CheatType::IgnoreResearchStatus:
                    ds << GetGameState().Cheats.IgnoreResearchStatus;
                    break;
                case CheatType::EnableAllDrawableTrackPieces:
                    ds << GetGameState().Cheats.EnableAllDrawableTrackPieces;
                    break;
                case CheatType::AllowTrackPlaceInvalidHeights:
                    ds << GetGameState().Cheats.AllowTrackPlaceInvalidHeights;
                    break;
                case CheatType::NoCapOnQueueLengthDummy:
                    ds << dummyBool;
                    break;
                case CheatType::AllowRegularPathAsQueue:
                    ds << GetGameState().Cheats.AllowRegularPathAsQueue;
                    break;
                case CheatType::AllowSpecialColourSchemes:
                    ds << GetGameState().Cheats.AllowSpecialColourSchemes;
                    break;
                case CheatType::MakeDestructible:
                    ds << GetGameState().Cheats.MakeAllDestructible;
                    break;
                case CheatType::SetStaffSpeed:
                    ds << GetGameState().Cheats.SelectedStaffSpeed;
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
        case CheatType::AllowSpecialColourSchemes:
            return LanguageGetString(STR_CHEAT_ALLOW_SPECIAL_COLOUR_SCHEMES);
        case CheatType::RemoveParkFences:
            return LanguageGetString(STR_CHEAT_REMOVE_PARK_FENCES);
        default:
            return "Unknown Cheat";
    }
}
