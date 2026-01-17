/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
    GameActions::Execute(&cheatSetAction, getGameState());
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
        IStream& stream = ds.GetStream();

        // Temporarily write 0, will be updated after every cheat is written.
        uint64_t countOffset = stream.GetPosition();
        ds << count;

        CheatEntrySerialise(ds, CheatType::sandboxMode, gameState.cheats.sandboxMode, count);
        CheatEntrySerialise(ds, CheatType::disableClearanceChecks, gameState.cheats.disableClearanceChecks, count);
        CheatEntrySerialise(ds, CheatType::disableSupportLimits, gameState.cheats.disableSupportLimits, count);
        CheatEntrySerialise(ds, CheatType::showAllOperatingModes, gameState.cheats.showAllOperatingModes, count);
        CheatEntrySerialise(
            ds, CheatType::showVehiclesFromOtherTrackTypes, gameState.cheats.showVehiclesFromOtherTrackTypes, count);
        CheatEntrySerialise(ds, CheatType::fastLiftHill, gameState.cheats.unlockOperatingLimits, count);
        CheatEntrySerialise(ds, CheatType::disableBrakesFailure, gameState.cheats.disableBrakesFailure, count);
        CheatEntrySerialise(ds, CheatType::disableAllBreakdowns, gameState.cheats.disableAllBreakdowns, count);
        CheatEntrySerialise(ds, CheatType::buildInPauseMode, gameState.cheats.buildInPauseMode, count);
        CheatEntrySerialise(ds, CheatType::ignoreRideIntensity, gameState.cheats.ignoreRideIntensity, count);
        CheatEntrySerialise(ds, CheatType::disableVandalism, gameState.cheats.disableVandalism, count);
        CheatEntrySerialise(ds, CheatType::disableLittering, gameState.cheats.disableLittering, count);
        CheatEntrySerialise(ds, CheatType::neverendingMarketing, gameState.cheats.neverendingMarketing, count);
        CheatEntrySerialise(ds, CheatType::freezeWeather, gameState.cheats.freezeWeather, count);
        CheatEntrySerialise(ds, CheatType::disableTrainLengthLimit, gameState.cheats.disableTrainLengthLimit, count);
        CheatEntrySerialise(ds, CheatType::disablePlantAging, gameState.cheats.disablePlantAging, count);
        CheatEntrySerialise(ds, CheatType::enableChainLiftOnAllTrack, gameState.cheats.enableChainLiftOnAllTrack, count);
        CheatEntrySerialise(
            ds, CheatType::allowArbitraryRideTypeChanges, gameState.cheats.allowArbitraryRideTypeChanges, count);
        CheatEntrySerialise(ds, CheatType::disableRideValueAging, gameState.cheats.disableRideValueAging, count);
        CheatEntrySerialise(ds, CheatType::ignoreResearchStatus, gameState.cheats.ignoreResearchStatus, count);
        CheatEntrySerialise(ds, CheatType::enableAllDrawableTrackPieces, gameState.cheats.enableAllDrawableTrackPieces, count);
        CheatEntrySerialise(
            ds, CheatType::allowTrackPlaceInvalidHeights, gameState.cheats.allowTrackPlaceInvalidHeights, count);
        CheatEntrySerialise(ds, CheatType::allowRegularPathAsQueue, gameState.cheats.allowRegularPathAsQueue, count);
        CheatEntrySerialise(ds, CheatType::allowSpecialColourSchemes, gameState.cheats.allowSpecialColourSchemes, count);
        CheatEntrySerialise(ds, CheatType::makeDestructible, gameState.cheats.makeAllDestructible, count);
        CheatEntrySerialise(ds, CheatType::setStaffSpeed, gameState.cheats.selectedStaffSpeed, count);
        CheatEntrySerialise(ds, CheatType::ignorePrice, gameState.cheats.ignorePrice, count);
        CheatEntrySerialise(ds, CheatType::setForcedParkRating, gameState.cheats.forcedParkRating, count);

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
                using enum CheatType;
                case sandboxMode:
                    ds << gameState.cheats.sandboxMode;
                    break;
                case disableClearanceChecks:
                    ds << gameState.cheats.disableClearanceChecks;
                    break;
                case disableSupportLimits:
                    ds << gameState.cheats.disableSupportLimits;
                    break;
                case showAllOperatingModes:
                    ds << gameState.cheats.showAllOperatingModes;
                    break;
                case showVehiclesFromOtherTrackTypes:
                    ds << gameState.cheats.showVehiclesFromOtherTrackTypes;
                    break;
                case fastLiftHill:
                    ds << gameState.cheats.unlockOperatingLimits;
                    break;
                case disableBrakesFailure:
                    ds << gameState.cheats.disableBrakesFailure;
                    break;
                case disableAllBreakdowns:
                    ds << gameState.cheats.disableAllBreakdowns;
                    break;
                case buildInPauseMode:
                    ds << gameState.cheats.buildInPauseMode;
                    break;
                case ignoreRideIntensity:
                    ds << gameState.cheats.ignoreRideIntensity;
                    break;
                case ignorePrice:
                    ds << gameState.cheats.ignorePrice;
                    break;
                case disableVandalism:
                    ds << gameState.cheats.disableVandalism;
                    break;
                case disableLittering:
                    ds << gameState.cheats.disableLittering;
                    break;
                case neverendingMarketing:
                    ds << gameState.cheats.neverendingMarketing;
                    break;
                case freezeWeather:
                    ds << gameState.cheats.freezeWeather;
                    break;
                case disableTrainLengthLimit:
                    ds << gameState.cheats.disableTrainLengthLimit;
                    break;
                case disablePlantAging:
                    ds << gameState.cheats.disablePlantAging;
                    break;
                case enableChainLiftOnAllTrack:
                    ds << gameState.cheats.enableChainLiftOnAllTrack;
                    break;
                case allowArbitraryRideTypeChanges:
                    ds << gameState.cheats.allowArbitraryRideTypeChanges;
                    break;
                case disableRideValueAging:
                    ds << gameState.cheats.disableRideValueAging;
                    break;
                case ignoreResearchStatus:
                    ds << gameState.cheats.ignoreResearchStatus;
                    break;
                case enableAllDrawableTrackPieces:
                    ds << gameState.cheats.enableAllDrawableTrackPieces;
                    break;
                case allowTrackPlaceInvalidHeights:
                    ds << gameState.cheats.allowTrackPlaceInvalidHeights;
                    break;
                case noCapOnQueueLengthDummy:
                    ds << dummyBool;
                    break;
                case allowRegularPathAsQueue:
                    ds << gameState.cheats.allowRegularPathAsQueue;
                    break;
                case allowSpecialColourSchemes:
                    ds << gameState.cheats.allowSpecialColourSchemes;
                    break;
                case makeDestructible:
                    ds << gameState.cheats.makeAllDestructible;
                    break;
                case setStaffSpeed:
                    ds << gameState.cheats.selectedStaffSpeed;
                    break;
                case setForcedParkRating:
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
        using enum CheatType;
        case sandboxMode:
            return LanguageGetString(STR_CHEAT_SANDBOX_MODE);
        case disableClearanceChecks:
            return LanguageGetString(STR_DISABLE_CLEARANCE_CHECKS);
        case disableSupportLimits:
            return LanguageGetString(STR_DISABLE_SUPPORT_LIMITS);
        case showAllOperatingModes:
            return LanguageGetString(STR_CHEAT_SHOW_ALL_OPERATING_MODES);
        case showVehiclesFromOtherTrackTypes:
            return LanguageGetString(STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES);
        case fastLiftHill:
            return LanguageGetString(STR_CHEAT_UNLOCK_OPERATING_LIMITS);
        case disableBrakesFailure:
            return LanguageGetString(STR_CHEAT_DISABLE_BRAKES_FAILURE);
        case disableAllBreakdowns:
            return LanguageGetString(STR_CHEAT_DISABLE_BREAKDOWNS);
        case disableTrainLengthLimit:
            return LanguageGetString(STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT);
        case enableChainLiftOnAllTrack:
            return LanguageGetString(STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK);
        case buildInPauseMode:
            return LanguageGetString(STR_CHEAT_BUILD_IN_PAUSE_MODE);
        case ignoreRideIntensity:
            return LanguageGetString(STR_CHEAT_IGNORE_INTENSITY);
        case ignorePrice:
            return LanguageGetString(STR_CHEAT_IGNORE_PRICE);
        case disableVandalism:
            return LanguageGetString(STR_CHEAT_DISABLE_VANDALISM);
        case disableLittering:
            return LanguageGetString(STR_CHEAT_DISABLE_LITTERING);
        case noMoney:
            return LanguageGetString(STR_MAKE_PARK_NO_MONEY);
        case addMoney:
            return LanguageGetString(STR_LOG_CHEAT_ADD_MONEY);
        case clearLoan:
            return LanguageGetString(STR_CHEAT_CLEAR_LOAN);
        case setGuestParameter:
            return LanguageGetString(STR_CHEAT_SET_GUESTS_PARAMETERS);
        case generateGuests:
            return LanguageGetString(STR_CHEAT_LARGE_TRAM_GUESTS);
        case removeAllGuests:
            return LanguageGetString(STR_CHEAT_REMOVE_ALL_GUESTS);
        case giveAllGuests:
            return LanguageGetString(STR_CHEAT_GIVE_ALL_GUESTS);
        case setGrassLength:
            return LanguageGetString(STR_CHEAT_CLEAR_GRASS);
        case waterPlants:
            return LanguageGetString(STR_CHEAT_WATER_PLANTS);
        case fixVandalism:
            return LanguageGetString(STR_CHEAT_FIX_VANDALISM);
        case removeLitter:
            return LanguageGetString(STR_CHEAT_REMOVE_LITTER);
        case disablePlantAging:
            return LanguageGetString(STR_CHEAT_DISABLE_PLANT_AGING);
        case setStaffSpeed:
            return LanguageGetString(STR_CHEAT_STAFF_SPEED);
        case renewRides:
            return LanguageGetString(STR_CHEAT_RENEW_RIDES);
        case makeDestructible:
            return LanguageGetString(STR_CHEAT_MAKE_DESTRUCTABLE);
        case fixRides:
            return LanguageGetString(STR_CHEAT_FIX_ALL_RIDES);
        case resetCrashStatus:
            return LanguageGetString(STR_CHEAT_RESET_CRASH_STATUS);
        case tenMinuteInspections:
            return LanguageGetString(STR_CHEAT_10_MINUTE_INSPECTIONS);
        case winScenario:
            return LanguageGetString(STR_CHEAT_WIN_SCENARIO);
        case forceWeather:
            return LanguageGetString(STR_CHANGE_WEATHER);
        case freezeWeather:
            return LanguageGetString(STR_CHEAT_FREEZE_WEATHER);
        case neverendingMarketing:
            return LanguageGetString(STR_CHEAT_NEVERENDING_MARKETING);
        case openClosePark:
            return LanguageGetString(STR_CHEAT_OPEN_PARK);
        case haveFun:
            return LanguageGetString(STR_CHEAT_HAVE_FUN);
        case setForcedParkRating:
            return LanguageGetString(STR_FORCE_PARK_RATING);
        case allowArbitraryRideTypeChanges:
            return LanguageGetString(STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES);
        case setMoney:
            return LanguageGetString(STR_SET_MONEY);
        case ownAllLand:
            return LanguageGetString(STR_CHEAT_OWN_ALL_LAND);
        case disableRideValueAging:
            return LanguageGetString(STR_CHEAT_DISABLE_RIDE_VALUE_AGING);
        case ignoreResearchStatus:
            return LanguageGetString(STR_CHEAT_IGNORE_RESEARCH_STATUS);
        case enableAllDrawableTrackPieces:
            return LanguageGetString(STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES);
        case allowTrackPlaceInvalidHeights:
            return LanguageGetString(STR_CHEAT_ALLOW_TRACK_PLACE_INVALID_HEIGHTS);
        case allowRegularPathAsQueue:
            return LanguageGetString(STR_CHEAT_ALLOW_PATH_AS_QUEUE);
        case allowSpecialColourSchemes:
            return LanguageGetString(STR_CHEAT_ALLOW_SPECIAL_COLOUR_SCHEMES);
        case removeParkFences:
            return LanguageGetString(STR_CHEAT_REMOVE_PARK_FENCES);
        default:
            return "Unknown Cheat";
    }
}
