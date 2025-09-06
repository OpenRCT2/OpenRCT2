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
#include "network/Network.h"
#include "ride/Ride.h"
#include "world/Climate.h"
#include "world/Footpath.h"
#include "world/Park.h"
#include "world/Scenery.h"

using namespace OpenRCT2;

// TODO: Refactor this. Cheat variables should contain the cheat type
// and a serialisation method.

void CheatsReset()
{
    auto& gameState = getGameState();
    gameState.cheats.sandboxMode = false;
    gameState.cheats.disableClearanceChecks = false;
    gameState.cheats.disableSupportLimits = false;
    gameState.cheats.showAllOperatingModes = false;
    gameState.cheats.showVehiclesFromOtherTrackTypes = false;
    gameState.cheats.disableTrainLengthLimit = false;
    gameState.cheats.enableChainLiftOnAllTrack = false;
    gameState.cheats.unlockOperatingLimits = false;
    gameState.cheats.disableBrakesFailure = false;
    gameState.cheats.disableAllBreakdowns = false;
    gameState.cheats.buildInPauseMode = false;
    gameState.cheats.ignoreRideIntensity = false;
    gameState.cheats.ignorePrice = false;
    gameState.cheats.disableVandalism = false;
    gameState.cheats.disableLittering = false;
    gameState.cheats.neverendingMarketing = false;
    gameState.cheats.freezeWeather = false;
    gameState.cheats.disablePlantAging = false;
    gameState.cheats.allowArbitraryRideTypeChanges = false;
    gameState.cheats.disableRideValueAging = false;
    gameState.cheats.ignoreResearchStatus = false;
    gameState.cheats.enableAllDrawableTrackPieces = false;
    gameState.cheats.allowTrackPlaceInvalidHeights = false;
    gameState.cheats.allowRegularPathAsQueue = false;
    gameState.cheats.allowSpecialColourSchemes = false;
    gameState.cheats.makeAllDestructible = false;
    gameState.cheats.selectedStaffSpeed = StaffSpeedCheat::None;
    gameState.cheats.forcedParkRating = kForcedParkRatingDisabled;
}

void CheatsSet(CheatType cheatType, int64_t param1 /* = 0*/, int64_t param2 /* = 0*/)
{
    auto cheatSetAction = GameActions::CheatSetAction(cheatType, param1, param2);
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
    auto& gameState = getGameState();

    if (ds.IsSaving())
    {
        OpenRCT2::IStream& stream = ds.GetStream();

        // Temporarily write 0, will be updated after every cheat is written.
        uint64_t countOffset = stream.GetPosition();
        ds << count;

        CheatEntrySerialise(ds, CheatType::SandboxMode, gameState.cheats.sandboxMode, count);
        CheatEntrySerialise(ds, CheatType::DisableClearanceChecks, gameState.cheats.disableClearanceChecks, count);
        CheatEntrySerialise(ds, CheatType::DisableSupportLimits, gameState.cheats.disableSupportLimits, count);
        CheatEntrySerialise(ds, CheatType::ShowAllOperatingModes, gameState.cheats.showAllOperatingModes, count);
        CheatEntrySerialise(
            ds, CheatType::ShowVehiclesFromOtherTrackTypes, gameState.cheats.showVehiclesFromOtherTrackTypes, count);
        CheatEntrySerialise(ds, CheatType::FastLiftHill, gameState.cheats.unlockOperatingLimits, count);
        CheatEntrySerialise(ds, CheatType::DisableBrakesFailure, gameState.cheats.disableBrakesFailure, count);
        CheatEntrySerialise(ds, CheatType::DisableAllBreakdowns, gameState.cheats.disableAllBreakdowns, count);
        CheatEntrySerialise(ds, CheatType::BuildInPauseMode, gameState.cheats.buildInPauseMode, count);
        CheatEntrySerialise(ds, CheatType::IgnoreRideIntensity, gameState.cheats.ignoreRideIntensity, count);
        CheatEntrySerialise(ds, CheatType::DisableVandalism, gameState.cheats.disableVandalism, count);
        CheatEntrySerialise(ds, CheatType::DisableLittering, gameState.cheats.disableLittering, count);
        CheatEntrySerialise(ds, CheatType::NeverendingMarketing, gameState.cheats.neverendingMarketing, count);
        CheatEntrySerialise(ds, CheatType::FreezeWeather, gameState.cheats.freezeWeather, count);
        CheatEntrySerialise(ds, CheatType::DisableTrainLengthLimit, gameState.cheats.disableTrainLengthLimit, count);
        CheatEntrySerialise(ds, CheatType::DisablePlantAging, gameState.cheats.disablePlantAging, count);
        CheatEntrySerialise(ds, CheatType::EnableChainLiftOnAllTrack, gameState.cheats.enableChainLiftOnAllTrack, count);
        CheatEntrySerialise(
            ds, CheatType::AllowArbitraryRideTypeChanges, gameState.cheats.allowArbitraryRideTypeChanges, count);
        CheatEntrySerialise(ds, CheatType::DisableRideValueAging, gameState.cheats.disableRideValueAging, count);
        CheatEntrySerialise(ds, CheatType::IgnoreResearchStatus, gameState.cheats.ignoreResearchStatus, count);
        CheatEntrySerialise(ds, CheatType::EnableAllDrawableTrackPieces, gameState.cheats.enableAllDrawableTrackPieces, count);
        CheatEntrySerialise(
            ds, CheatType::AllowTrackPlaceInvalidHeights, gameState.cheats.allowTrackPlaceInvalidHeights, count);
        CheatEntrySerialise(ds, CheatType::AllowRegularPathAsQueue, gameState.cheats.allowRegularPathAsQueue, count);
        CheatEntrySerialise(ds, CheatType::AllowSpecialColourSchemes, gameState.cheats.allowSpecialColourSchemes, count);
        CheatEntrySerialise(ds, CheatType::MakeDestructible, gameState.cheats.makeAllDestructible, count);
        CheatEntrySerialise(ds, CheatType::SetStaffSpeed, gameState.cheats.selectedStaffSpeed, count);
        CheatEntrySerialise(ds, CheatType::IgnorePrice, gameState.cheats.ignorePrice, count);
        CheatEntrySerialise(ds, CheatType::SetForcedParkRating, gameState.cheats.forcedParkRating, count);

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
                    ds << gameState.cheats.sandboxMode;
                    break;
                case CheatType::DisableClearanceChecks:
                    ds << gameState.cheats.disableClearanceChecks;
                    break;
                case CheatType::DisableSupportLimits:
                    ds << gameState.cheats.disableSupportLimits;
                    break;
                case CheatType::ShowAllOperatingModes:
                    ds << gameState.cheats.showAllOperatingModes;
                    break;
                case CheatType::ShowVehiclesFromOtherTrackTypes:
                    ds << gameState.cheats.showVehiclesFromOtherTrackTypes;
                    break;
                case CheatType::FastLiftHill:
                    ds << gameState.cheats.unlockOperatingLimits;
                    break;
                case CheatType::DisableBrakesFailure:
                    ds << gameState.cheats.disableBrakesFailure;
                    break;
                case CheatType::DisableAllBreakdowns:
                    ds << gameState.cheats.disableAllBreakdowns;
                    break;
                case CheatType::BuildInPauseMode:
                    ds << gameState.cheats.buildInPauseMode;
                    break;
                case CheatType::IgnoreRideIntensity:
                    ds << gameState.cheats.ignoreRideIntensity;
                    break;
                case CheatType::IgnorePrice:
                    ds << gameState.cheats.ignorePrice;
                    break;
                case CheatType::DisableVandalism:
                    ds << gameState.cheats.disableVandalism;
                    break;
                case CheatType::DisableLittering:
                    ds << gameState.cheats.disableLittering;
                    break;
                case CheatType::NeverendingMarketing:
                    ds << gameState.cheats.neverendingMarketing;
                    break;
                case CheatType::FreezeWeather:
                    ds << gameState.cheats.freezeWeather;
                    break;
                case CheatType::DisableTrainLengthLimit:
                    ds << gameState.cheats.disableTrainLengthLimit;
                    break;
                case CheatType::DisablePlantAging:
                    ds << gameState.cheats.disablePlantAging;
                    break;
                case CheatType::EnableChainLiftOnAllTrack:
                    ds << gameState.cheats.enableChainLiftOnAllTrack;
                    break;
                case CheatType::AllowArbitraryRideTypeChanges:
                    ds << gameState.cheats.allowArbitraryRideTypeChanges;
                    break;
                case CheatType::DisableRideValueAging:
                    ds << gameState.cheats.disableRideValueAging;
                    break;
                case CheatType::IgnoreResearchStatus:
                    ds << gameState.cheats.ignoreResearchStatus;
                    break;
                case CheatType::EnableAllDrawableTrackPieces:
                    ds << gameState.cheats.enableAllDrawableTrackPieces;
                    break;
                case CheatType::AllowTrackPlaceInvalidHeights:
                    ds << gameState.cheats.allowTrackPlaceInvalidHeights;
                    break;
                case CheatType::NoCapOnQueueLengthDummy:
                    ds << dummyBool;
                    break;
                case CheatType::AllowRegularPathAsQueue:
                    ds << gameState.cheats.allowRegularPathAsQueue;
                    break;
                case CheatType::AllowSpecialColourSchemes:
                    ds << gameState.cheats.allowSpecialColourSchemes;
                    break;
                case CheatType::MakeDestructible:
                    ds << gameState.cheats.makeAllDestructible;
                    break;
                case CheatType::SetStaffSpeed:
                    ds << gameState.cheats.selectedStaffSpeed;
                    break;
                case CheatType::SetForcedParkRating:
                    ds << gameState.cheats.forcedParkRating;
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
