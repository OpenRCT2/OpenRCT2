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
                case CheatType::sandboxMode:
                    ds << gameState.cheats.sandboxMode;
                    break;
                case CheatType::disableClearanceChecks:
                    ds << gameState.cheats.disableClearanceChecks;
                    break;
                case CheatType::disableSupportLimits:
                    ds << gameState.cheats.disableSupportLimits;
                    break;
                case CheatType::showAllOperatingModes:
                    ds << gameState.cheats.showAllOperatingModes;
                    break;
                case CheatType::showVehiclesFromOtherTrackTypes:
                    ds << gameState.cheats.showVehiclesFromOtherTrackTypes;
                    break;
                case CheatType::fastLiftHill:
                    ds << gameState.cheats.unlockOperatingLimits;
                    break;
                case CheatType::disableBrakesFailure:
                    ds << gameState.cheats.disableBrakesFailure;
                    break;
                case CheatType::disableAllBreakdowns:
                    ds << gameState.cheats.disableAllBreakdowns;
                    break;
                case CheatType::buildInPauseMode:
                    ds << gameState.cheats.buildInPauseMode;
                    break;
                case CheatType::ignoreRideIntensity:
                    ds << gameState.cheats.ignoreRideIntensity;
                    break;
                case CheatType::ignorePrice:
                    ds << gameState.cheats.ignorePrice;
                    break;
                case CheatType::disableVandalism:
                    ds << gameState.cheats.disableVandalism;
                    break;
                case CheatType::disableLittering:
                    ds << gameState.cheats.disableLittering;
                    break;
                case CheatType::neverendingMarketing:
                    ds << gameState.cheats.neverendingMarketing;
                    break;
                case CheatType::freezeWeather:
                    ds << gameState.cheats.freezeWeather;
                    break;
                case CheatType::disableTrainLengthLimit:
                    ds << gameState.cheats.disableTrainLengthLimit;
                    break;
                case CheatType::disablePlantAging:
                    ds << gameState.cheats.disablePlantAging;
                    break;
                case CheatType::enableChainLiftOnAllTrack:
                    ds << gameState.cheats.enableChainLiftOnAllTrack;
                    break;
                case CheatType::allowArbitraryRideTypeChanges:
                    ds << gameState.cheats.allowArbitraryRideTypeChanges;
                    break;
                case CheatType::disableRideValueAging:
                    ds << gameState.cheats.disableRideValueAging;
                    break;
                case CheatType::ignoreResearchStatus:
                    ds << gameState.cheats.ignoreResearchStatus;
                    break;
                case CheatType::enableAllDrawableTrackPieces:
                    ds << gameState.cheats.enableAllDrawableTrackPieces;
                    break;
                case CheatType::allowTrackPlaceInvalidHeights:
                    ds << gameState.cheats.allowTrackPlaceInvalidHeights;
                    break;
                case CheatType::noCapOnQueueLengthDummy:
                    ds << dummyBool;
                    break;
                case CheatType::allowRegularPathAsQueue:
                    ds << gameState.cheats.allowRegularPathAsQueue;
                    break;
                case CheatType::allowSpecialColourSchemes:
                    ds << gameState.cheats.allowSpecialColourSchemes;
                    break;
                case CheatType::makeDestructible:
                    ds << gameState.cheats.makeAllDestructible;
                    break;
                case CheatType::setStaffSpeed:
                    ds << gameState.cheats.selectedStaffSpeed;
                    break;
                case CheatType::setForcedParkRating:
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
        case CheatType::sandboxMode:
            return LanguageGetString(STR_CHEAT_SANDBOX_MODE);
        case CheatType::disableClearanceChecks:
            return LanguageGetString(STR_DISABLE_CLEARANCE_CHECKS);
        case CheatType::disableSupportLimits:
            return LanguageGetString(STR_DISABLE_SUPPORT_LIMITS);
        case CheatType::showAllOperatingModes:
            return LanguageGetString(STR_CHEAT_SHOW_ALL_OPERATING_MODES);
        case CheatType::showVehiclesFromOtherTrackTypes:
            return LanguageGetString(STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES);
        case CheatType::fastLiftHill:
            return LanguageGetString(STR_CHEAT_UNLOCK_OPERATING_LIMITS);
        case CheatType::disableBrakesFailure:
            return LanguageGetString(STR_CHEAT_DISABLE_BRAKES_FAILURE);
        case CheatType::disableAllBreakdowns:
            return LanguageGetString(STR_CHEAT_DISABLE_BREAKDOWNS);
        case CheatType::disableTrainLengthLimit:
            return LanguageGetString(STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT);
        case CheatType::enableChainLiftOnAllTrack:
            return LanguageGetString(STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK);
        case CheatType::buildInPauseMode:
            return LanguageGetString(STR_CHEAT_BUILD_IN_PAUSE_MODE);
        case CheatType::ignoreRideIntensity:
            return LanguageGetString(STR_CHEAT_IGNORE_INTENSITY);
        case CheatType::ignorePrice:
            return LanguageGetString(STR_CHEAT_IGNORE_PRICE);
        case CheatType::disableVandalism:
            return LanguageGetString(STR_CHEAT_DISABLE_VANDALISM);
        case CheatType::disableLittering:
            return LanguageGetString(STR_CHEAT_DISABLE_LITTERING);
        case CheatType::noMoney:
            return LanguageGetString(STR_MAKE_PARK_NO_MONEY);
        case CheatType::addMoney:
            return LanguageGetString(STR_LOG_CHEAT_ADD_MONEY);
        case CheatType::clearLoan:
            return LanguageGetString(STR_CHEAT_CLEAR_LOAN);
        case CheatType::setGuestParameter:
            return LanguageGetString(STR_CHEAT_SET_GUESTS_PARAMETERS);
        case CheatType::generateGuests:
            return LanguageGetString(STR_CHEAT_LARGE_TRAM_GUESTS);
        case CheatType::removeAllGuests:
            return LanguageGetString(STR_CHEAT_REMOVE_ALL_GUESTS);
        case CheatType::giveAllGuests:
            return LanguageGetString(STR_CHEAT_GIVE_ALL_GUESTS);
        case CheatType::setGrassLength:
            return LanguageGetString(STR_CHEAT_CLEAR_GRASS);
        case CheatType::waterPlants:
            return LanguageGetString(STR_CHEAT_WATER_PLANTS);
        case CheatType::fixVandalism:
            return LanguageGetString(STR_CHEAT_FIX_VANDALISM);
        case CheatType::removeLitter:
            return LanguageGetString(STR_CHEAT_REMOVE_LITTER);
        case CheatType::disablePlantAging:
            return LanguageGetString(STR_CHEAT_DISABLE_PLANT_AGING);
        case CheatType::setStaffSpeed:
            return LanguageGetString(STR_CHEAT_STAFF_SPEED);
        case CheatType::renewRides:
            return LanguageGetString(STR_CHEAT_RENEW_RIDES);
        case CheatType::makeDestructible:
            return LanguageGetString(STR_CHEAT_MAKE_DESTRUCTABLE);
        case CheatType::fixRides:
            return LanguageGetString(STR_CHEAT_FIX_ALL_RIDES);
        case CheatType::resetCrashStatus:
            return LanguageGetString(STR_CHEAT_RESET_CRASH_STATUS);
        case CheatType::tenMinuteInspections:
            return LanguageGetString(STR_CHEAT_10_MINUTE_INSPECTIONS);
        case CheatType::winScenario:
            return LanguageGetString(STR_CHEAT_WIN_SCENARIO);
        case CheatType::forceWeather:
            return LanguageGetString(STR_CHANGE_WEATHER);
        case CheatType::freezeWeather:
            return LanguageGetString(STR_CHEAT_FREEZE_WEATHER);
        case CheatType::neverendingMarketing:
            return LanguageGetString(STR_CHEAT_NEVERENDING_MARKETING);
        case CheatType::openClosePark:
            return LanguageGetString(STR_CHEAT_OPEN_PARK);
        case CheatType::haveFun:
            return LanguageGetString(STR_CHEAT_HAVE_FUN);
        case CheatType::setForcedParkRating:
            return LanguageGetString(STR_FORCE_PARK_RATING);
        case CheatType::allowArbitraryRideTypeChanges:
            return LanguageGetString(STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES);
        case CheatType::setMoney:
            return LanguageGetString(STR_SET_MONEY);
        case CheatType::ownAllLand:
            return LanguageGetString(STR_CHEAT_OWN_ALL_LAND);
        case CheatType::disableRideValueAging:
            return LanguageGetString(STR_CHEAT_DISABLE_RIDE_VALUE_AGING);
        case CheatType::ignoreResearchStatus:
            return LanguageGetString(STR_CHEAT_IGNORE_RESEARCH_STATUS);
        case CheatType::enableAllDrawableTrackPieces:
            return LanguageGetString(STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES);
        case CheatType::allowTrackPlaceInvalidHeights:
            return LanguageGetString(STR_CHEAT_ALLOW_TRACK_PLACE_INVALID_HEIGHTS);
        case CheatType::allowRegularPathAsQueue:
            return LanguageGetString(STR_CHEAT_ALLOW_PATH_AS_QUEUE);
        case CheatType::allowSpecialColourSchemes:
            return LanguageGetString(STR_CHEAT_ALLOW_SPECIAL_COLOUR_SCHEMES);
        case CheatType::removeParkFences:
            return LanguageGetString(STR_CHEAT_REMOVE_PARK_FENCES);
        default:
            return "Unknown Cheat";
    }
}
