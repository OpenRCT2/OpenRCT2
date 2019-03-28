/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Cheats.h"

#include "GameState.h"
#include "actions/ParkSetLoanAction.hpp"
#include "config/Config.h"
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
#include "world/Sprite.h"
#include "world/Surface.h"

using namespace OpenRCT2;

bool gCheatsSandboxMode = false;
bool gCheatsDisableClearanceChecks = false;
bool gCheatsDisableSupportLimits = false;
bool gCheatsShowAllOperatingModes = false;
bool gCheatsShowVehiclesFromOtherTrackTypes = false;
bool gCheatsFastLiftHill = false;
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

void cheats_reset()
{
    gCheatsSandboxMode = false;
    gCheatsDisableClearanceChecks = false;
    gCheatsDisableSupportLimits = false;
    gCheatsShowAllOperatingModes = false;
    gCheatsShowVehiclesFromOtherTrackTypes = false;
    gCheatsDisableTrainLengthLimit = false;
    gCheatsEnableChainLiftOnAllTrack = false;
    gCheatsFastLiftHill = false;
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
}

const char* cheats_get_cheat_string(CheatType cheatType)
{
    switch (cheatType)
    {
        case CheatType::CHEAT_SANDBOXMODE:
            return language_get_string(STR_CHEAT_SANDBOX_MODE);
        case CheatType::CHEAT_DISABLECLEARANCECHECKS:
            return language_get_string(STR_DISABLE_CLEARANCE_CHECKS);
        case CheatType::CHEAT_DISABLESUPPORTLIMITS:
            return language_get_string(STR_DISABLE_SUPPORT_LIMITS);
        case CheatType::CHEAT_SHOWALLOPERATINGMODES:
            return language_get_string(STR_CHEAT_SHOW_ALL_OPERATING_MODES);
        case CheatType::CHEAT_SHOWVEHICLESFROMOTHERTRACKTYPES:
            return language_get_string(STR_CHEAT_SHOW_VEHICLES_FROM_OTHER_TRACK_TYPES);
        case CheatType::CHEAT_FASTLIFTHILL:
            return language_get_string(STR_CHEAT_UNLOCK_OPERATING_LIMITS);
        case CheatType::CHEAT_DISABLEBRAKESFAILURE:
            return language_get_string(STR_CHEAT_DISABLE_BRAKES_FAILURE);
        case CheatType::CHEAT_DISABLEALLBREAKDOWNS:
            return language_get_string(STR_CHEAT_DISABLE_BREAKDOWNS);
        case CheatType::CHEAT_DISABLETRAINLENGTHLIMIT:
            return language_get_string(STR_CHEAT_DISABLE_TRAIN_LENGTH_LIMIT);
        case CheatType::CHEAT_ENABLECHAINLIFTONALLTRACK:
            return language_get_string(STR_CHEAT_ENABLE_CHAIN_LIFT_ON_ALL_TRACK);
        case CheatType::CHEAT_BUILDINPAUSEMODE:
            return language_get_string(STR_CHEAT_BUILD_IN_PAUSE_MODE);
        case CheatType::CHEAT_IGNORERIDEINTENSITY:
            return language_get_string(STR_CHEAT_IGNORE_INTENSITY);
        case CheatType::CHEAT_DISABLEVANDALISM:
            return language_get_string(STR_CHEAT_DISABLE_VANDALISM);
        case CheatType::CHEAT_DISABLELITTERING:
            return language_get_string(STR_CHEAT_DISABLE_LITTERING);
        case CheatType::CHEAT_NOMONEY:
            return language_get_string(STR_MAKE_PARK_NO_MONEY);
        case CheatType::CHEAT_ADDMONEY:
            return language_get_string(STR_LOG_CHEAT_ADD_MONEY);
        case CheatType::CHEAT_CLEARLOAN:
            return language_get_string(STR_CHEAT_CLEAR_LOAN);
        case CheatType::CHEAT_SETGUESTPARAMETER:
            return language_get_string(STR_CHEAT_SET_GUESTS_PARAMETERS);
        case CheatType::CHEAT_GENERATEGUESTS:
            return language_get_string(STR_CHEAT_LARGE_TRAM_GUESTS);
        case CheatType::CHEAT_REMOVEALLGUESTS:
            return language_get_string(STR_CHEAT_REMOVE_ALL_GUESTS);
        case CheatType::CHEAT_EXPLODEGUESTS:
            return language_get_string(STR_CHEAT_EXPLODE);
        case CheatType::CHEAT_GIVEALLGUESTS:
            return language_get_string(STR_CHEAT_GIVE_ALL_GUESTS);
        case CheatType::CHEAT_SETGRASSLENGTH:
            return language_get_string(STR_CHEAT_CLEAR_GRASS);
        case CheatType::CHEAT_WATERPLANTS:
            return language_get_string(STR_CHEAT_WATER_PLANTS);
        case CheatType::CHEAT_FIXVANDALISM:
            return language_get_string(STR_CHEAT_FIX_VANDALISM);
        case CheatType::CHEAT_REMOVELITTER:
            return language_get_string(STR_CHEAT_REMOVE_LITTER);
        case CheatType::CHEAT_DISABLEPLANTAGING:
            return language_get_string(STR_CHEAT_DISABLE_PLANT_AGING);
        case CheatType::CHEAT_SETSTAFFSPEED:
            return language_get_string(STR_CHEAT_STAFF_SPEED);
        case CheatType::CHEAT_RENEWRIDES:
            return language_get_string(STR_CHEAT_RENEW_RIDES);
        case CheatType::CHEAT_MAKEDESTRUCTIBLE:
            return language_get_string(STR_CHEAT_MAKE_DESTRUCTABLE);
        case CheatType::CHEAT_FIXRIDES:
            return language_get_string(STR_CHEAT_FIX_ALL_RIDES);
        case CheatType::CHEAT_RESETCRASHSTATUS:
            return language_get_string(STR_CHEAT_RESET_CRASH_STATUS);
        case CheatType::CHEAT_10MINUTEINSPECTIONS:
            return language_get_string(STR_CHEAT_10_MINUTE_INSPECTIONS);
        case CheatType::CHEAT_WINSCENARIO:
            return language_get_string(STR_CHEAT_WIN_SCENARIO);
        case CheatType::CHEAT_FORCEWEATHER:
            return language_get_string(STR_FORCE_WEATHER);
        case CheatType::CHEAT_FREEZEWEATHER:
            return language_get_string(STR_CHEAT_FREEZE_WEATHER);
        case CheatType::CHEAT_NEVERENDINGMARKETING:
            return language_get_string(STR_CHEAT_NEVERENDING_MARKETING);
        case CheatType::CHEAT_OPENCLOSEPARK:
            return language_get_string(STR_CHEAT_OPEN_PARK);
        case CheatType::CHEAT_HAVEFUN:
            return language_get_string(STR_CHEAT_HAVE_FUN);
        case CheatType::CHEAT_SETFORCEDPARKRATING:
            return language_get_string(STR_FORCE_PARK_RATING);
        case CheatType::CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES:
            return language_get_string(STR_CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES);
        case CheatType::CHEAT_SETMONEY:
            return language_get_string(STR_SET_MONEY);
        case CheatType::CHEAT_OWNALLLAND:
            return language_get_string(STR_CHEAT_OWN_ALL_LAND);
        case CheatType::CHEAT_DISABLERIDEVALUEAGING:
            return language_get_string(STR_CHEAT_DISABLE_RIDE_VALUE_AGING);
        case CheatType::CHEAT_IGNORERESEARCHSTATUS:
            return language_get_string(STR_CHEAT_IGNORE_RESEARCH_STATUS);
        case CheatType::CHEAT_ENABLEALLDRAWABLETRACKPIECES:
            return language_get_string(STR_CHEAT_ENABLE_ALL_DRAWABLE_TRACK_PIECES);
        default:
            return "Unknown Cheat";
    }
}
