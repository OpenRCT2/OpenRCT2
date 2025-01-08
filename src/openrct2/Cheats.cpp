/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include "core/DataSerialiser.h"
#include "network/network.h"
#include "ride/Ride.h"
#include "scenario/Scenario.h"
#include "world/Climate.h"
#include "world/Footpath.h"
#include "world/Map.h"
#include "world/Park.h"
#include "world/Scenery.h"

using namespace OpenRCT2;

// TODO: Refactor this. Cheat variables should contain the cheat type
// and a serialisation method.

void CheatsReset()
{
    auto& gameState = GetGameState();
    gameState.Cheats.sandboxMode = false;
    gameState.Cheats.disableClearanceChecks = false;
    gameState.Cheats.disableSupportLimits = false;
    gameState.Cheats.showAllOperatingModes = false;
    gameState.Cheats.showVehiclesFromOtherTrackTypes = false;
    gameState.Cheats.disableTrainLengthLimit = false;
    gameState.Cheats.enableChainLiftOnAllTrack = false;
    gameState.Cheats.unlockOperatingLimits = false;
    gameState.Cheats.disableBrakesFailure = false;
    gameState.Cheats.disableAllBreakdowns = false;
    gameState.Cheats.buildInPauseMode = false;
    gameState.Cheats.ignoreRideIntensity = false;
    gameState.Cheats.ignorePrice = false;
    gameState.Cheats.disableVandalism = false;
    gameState.Cheats.disableLittering = false;
    gameState.Cheats.neverendingMarketing = false;
    gameState.Cheats.freezeWeather = false;
    gameState.Cheats.disablePlantAging = false;
    gameState.Cheats.allowArbitraryRideTypeChanges = false;
    gameState.Cheats.disableRideValueAging = false;
    gameState.Cheats.ignoreResearchStatus = false;
    gameState.Cheats.enableAllDrawableTrackPieces = false;
    gameState.Cheats.allowTrackPlaceInvalidHeights = false;
    gameState.Cheats.allowRegularPathAsQueue = false;
    gameState.Cheats.allowSpecialColourSchemes = false;
    gameState.Cheats.makeAllDestructible = false;
    gameState.Cheats.selectedStaffSpeed = StaffSpeedCheat::None;
    gameState.Cheats.forcedParkRating = kForcedParkRatingDisabled;
}

void CheatsSet(CheatType cheatType, int64_t param1 /* = 0*/, int64_t param2 /* = 0*/)
{
    auto cheatSetAction = CheatSetAction(cheatType, param1, param2);
    GameActions::Execute(&cheatSetAction);
}

template<typename T>
static void CheatEntrySerialise(DataSerialiser& ds, CheatType type, const T& value, uint16_t& count)
{
    ds << static_cast<int32_t>(type) << value;
    count++;
}

void CheatsSerialise(DataSerialiser& ds)
{
    uint16_t count = 0;
    auto& gameState = GetGameState();

    if (ds.IsSaving())
    {
        OpenRCT2::IStream& stream = ds.GetStream();

        // Temporarily write 0, will be updated after every cheat is written.
        uint64_t countOffset = stream.GetPosition();
        ds << count;

        CheatEntrySerialise(ds, CheatType::SandboxMode, gameState.Cheats.sandboxMode, count);
        CheatEntrySerialise(ds, CheatType::DisableClearanceChecks, gameState.Cheats.disableClearanceChecks, count);
        CheatEntrySerialise(ds, CheatType::DisableSupportLimits, gameState.Cheats.disableSupportLimits, count);
        CheatEntrySerialise(ds, CheatType::ShowAllOperatingModes, gameState.Cheats.showAllOperatingModes, count);
        CheatEntrySerialise(
            ds, CheatType::ShowVehiclesFromOtherTrackTypes, gameState.Cheats.showVehiclesFromOtherTrackTypes, count);
        CheatEntrySerialise(ds, CheatType::FastLiftHill, gameState.Cheats.unlockOperatingLimits, count);
        CheatEntrySerialise(ds, CheatType::DisableBrakesFailure, gameState.Cheats.disableBrakesFailure, count);
        CheatEntrySerialise(ds, CheatType::DisableAllBreakdowns, gameState.Cheats.disableAllBreakdowns, count);
        CheatEntrySerialise(ds, CheatType::BuildInPauseMode, gameState.Cheats.buildInPauseMode, count);
        CheatEntrySerialise(ds, CheatType::IgnoreRideIntensity, gameState.Cheats.ignoreRideIntensity, count);
        CheatEntrySerialise(ds, CheatType::DisableVandalism, gameState.Cheats.disableVandalism, count);
        CheatEntrySerialise(ds, CheatType::DisableLittering, gameState.Cheats.disableLittering, count);
        CheatEntrySerialise(ds, CheatType::NeverendingMarketing, gameState.Cheats.neverendingMarketing, count);
        CheatEntrySerialise(ds, CheatType::FreezeWeather, gameState.Cheats.freezeWeather, count);
        CheatEntrySerialise(ds, CheatType::DisableTrainLengthLimit, gameState.Cheats.disableTrainLengthLimit, count);
        CheatEntrySerialise(ds, CheatType::DisablePlantAging, gameState.Cheats.disablePlantAging, count);
        CheatEntrySerialise(ds, CheatType::EnableChainLiftOnAllTrack, gameState.Cheats.enableChainLiftOnAllTrack, count);
        CheatEntrySerialise(
            ds, CheatType::AllowArbitraryRideTypeChanges, gameState.Cheats.allowArbitraryRideTypeChanges, count);
        CheatEntrySerialise(ds, CheatType::DisableRideValueAging, gameState.Cheats.disableRideValueAging, count);
        CheatEntrySerialise(ds, CheatType::IgnoreResearchStatus, gameState.Cheats.ignoreResearchStatus, count);
        CheatEntrySerialise(ds, CheatType::EnableAllDrawableTrackPieces, gameState.Cheats.enableAllDrawableTrackPieces, count);
        CheatEntrySerialise(
            ds, CheatType::AllowTrackPlaceInvalidHeights, gameState.Cheats.allowTrackPlaceInvalidHeights, count);
        CheatEntrySerialise(ds, CheatType::AllowRegularPathAsQueue, gameState.Cheats.allowRegularPathAsQueue, count);
        CheatEntrySerialise(ds, CheatType::AllowSpecialColourSchemes, gameState.Cheats.allowSpecialColourSchemes, count);
        CheatEntrySerialise(ds, CheatType::MakeDestructible, gameState.Cheats.makeAllDestructible, count);
        CheatEntrySerialise(ds, CheatType::SetStaffSpeed, gameState.Cheats.selectedStaffSpeed, count);
        CheatEntrySerialise(ds, CheatType::IgnorePrice, gameState.Cheats.ignorePrice, count);
        CheatEntrySerialise(ds, CheatType::SetForcedParkRating, gameState.Cheats.forcedParkRating, count);

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
                    ds << gameState.Cheats.sandboxMode;
                    break;
                case CheatType::DisableClearanceChecks:
                    ds << gameState.Cheats.disableClearanceChecks;
                    break;
                case CheatType::DisableSupportLimits:
                    ds << gameState.Cheats.disableSupportLimits;
                    break;
                case CheatType::ShowAllOperatingModes:
                    ds << gameState.Cheats.showAllOperatingModes;
                    break;
                case CheatType::ShowVehiclesFromOtherTrackTypes:
                    ds << gameState.Cheats.showVehiclesFromOtherTrackTypes;
                    break;
                case CheatType::FastLiftHill:
                    ds << gameState.Cheats.unlockOperatingLimits;
                    break;
                case CheatType::DisableBrakesFailure:
                    ds << gameState.Cheats.disableBrakesFailure;
                    break;
                case CheatType::DisableAllBreakdowns:
                    ds << gameState.Cheats.disableAllBreakdowns;
                    break;
                case CheatType::BuildInPauseMode:
                    ds << gameState.Cheats.buildInPauseMode;
                    break;
                case CheatType::IgnoreRideIntensity:
                    ds << gameState.Cheats.ignoreRideIntensity;
                    break;
                case CheatType::IgnorePrice:
                    ds << gameState.Cheats.ignorePrice;
                    break;
                case CheatType::DisableVandalism:
                    ds << gameState.Cheats.disableVandalism;
                    break;
                case CheatType::DisableLittering:
                    ds << gameState.Cheats.disableLittering;
                    break;
                case CheatType::NeverendingMarketing:
                    ds << gameState.Cheats.neverendingMarketing;
                    break;
                case CheatType::FreezeWeather:
                    ds << gameState.Cheats.freezeWeather;
                    break;
                case CheatType::DisableTrainLengthLimit:
                    ds << gameState.Cheats.disableTrainLengthLimit;
                    break;
                case CheatType::DisablePlantAging:
                    ds << gameState.Cheats.disablePlantAging;
                    break;
                case CheatType::EnableChainLiftOnAllTrack:
                    ds << gameState.Cheats.enableChainLiftOnAllTrack;
                    break;
                case CheatType::AllowArbitraryRideTypeChanges:
                    ds << gameState.Cheats.allowArbitraryRideTypeChanges;
                    break;
                case CheatType::DisableRideValueAging:
                    ds << gameState.Cheats.disableRideValueAging;
                    break;
                case CheatType::IgnoreResearchStatus:
                    ds << gameState.Cheats.ignoreResearchStatus;
                    break;
                case CheatType::EnableAllDrawableTrackPieces:
                    ds << gameState.Cheats.enableAllDrawableTrackPieces;
                    break;
                case CheatType::AllowTrackPlaceInvalidHeights:
                    ds << gameState.Cheats.allowTrackPlaceInvalidHeights;
                    break;
                case CheatType::NoCapOnQueueLengthDummy:
                    ds << dummyBool;
                    break;
                case CheatType::AllowRegularPathAsQueue:
                    ds << gameState.Cheats.allowRegularPathAsQueue;
                    break;
                case CheatType::AllowSpecialColourSchemes:
                    ds << gameState.Cheats.allowSpecialColourSchemes;
                    break;
                case CheatType::MakeDestructible:
                    ds << gameState.Cheats.makeAllDestructible;
                    break;
                case CheatType::SetStaffSpeed:
                    ds << gameState.Cheats.selectedStaffSpeed;
                    break;
                case CheatType::SetForcedParkRating:
                    ds << gameState.Cheats.forcedParkRating;
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
        case CheatType::IgnorePrice:
            return LanguageGetString(STR_CHEAT_IGNORE_PRICE);
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
        case CheatType::NeverendingMarketing:
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
