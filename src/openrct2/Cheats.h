/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _CHEATS_H_
#define _CHEATS_H_

#include "common.h"

extern bool gCheatsSandboxMode;
extern bool gCheatsDisableClearanceChecks;
extern bool gCheatsDisableSupportLimits;
extern bool gCheatsShowAllOperatingModes;
extern bool gCheatsShowVehiclesFromOtherTrackTypes;
extern bool gCheatsFastLiftHill;
extern bool gCheatsDisableBrakesFailure;
extern bool gCheatsDisableAllBreakdowns;
extern bool gCheatsBuildInPauseMode;
extern bool gCheatsIgnoreRideIntensity;
extern bool gCheatsDisableVandalism;
extern bool gCheatsDisableLittering;
extern bool gCheatsNeverendingMarketing;
extern bool gCheatsFreezeClimate;
extern bool gCheatsDisableTrainLengthLimit;
extern bool gCheatsDisablePlantAging;
extern bool gCheatsDisableRideValueAging;
extern bool gCheatsEnableChainLiftOnAllTrack;
extern bool gCheatsAllowArbitraryRideTypeChanges;
extern bool gCheatsIgnoreResearchStatus;
extern bool gCheatsEnableAllDrawableTrackPieces;


enum {
    CHEAT_SANDBOXMODE,
    CHEAT_DISABLECLEARANCECHECKS,
    CHEAT_DISABLESUPPORTLIMITS,
    CHEAT_SHOWALLOPERATINGMODES,
    CHEAT_SHOWVEHICLESFROMOTHERTRACKTYPES,
    CHEAT_DISABLETRAINLENGTHLIMIT,
    CHEAT_ENABLECHAINLIFTONALLTRACK,
    CHEAT_FASTLIFTHILL,
    CHEAT_DISABLEBRAKESFAILURE,
    CHEAT_DISABLEALLBREAKDOWNS,
    CHEAT_UNLOCKALLPRICES,
    CHEAT_BUILDINPAUSEMODE,
    CHEAT_IGNORERIDEINTENSITY,
    CHEAT_DISABLEVANDALISM,
    CHEAT_DISABLELITTERING,
    CHEAT_NOMONEY,
    CHEAT_ADDMONEY,
    CHEAT_SETMONEY,
    CHEAT_CLEARLOAN,
    CHEAT_SETGUESTPARAMETER,
    CHEAT_GENERATEGUESTS,
    CHEAT_REMOVEALLGUESTS,
    CHEAT_EXPLODEGUESTS,
    CHEAT_GIVEALLGUESTS,
    CHEAT_SETGRASSLENGTH,
    CHEAT_WATERPLANTS,
    CHEAT_DISABLEPLANTAGING,
    CHEAT_FIXVANDALISM,
    CHEAT_REMOVELITTER,
    CHEAT_SETSTAFFSPEED,
    CHEAT_RENEWRIDES,
    CHEAT_MAKEDESTRUCTIBLE,
    CHEAT_FIXRIDES,
    CHEAT_RESETCRASHSTATUS,
    CHEAT_10MINUTEINSPECTIONS,
    CHEAT_WINSCENARIO,
    CHEAT_FORCEWEATHER,
    CHEAT_FREEZECLIMATE,
    CHEAT_OPENCLOSEPARK,
    CHEAT_HAVEFUN,
    CHEAT_SETFORCEDPARKRATING,
    CHEAT_NEVERENDINGMARKETING,
    CHEAT_RESETDATE,
    CHEAT_ALLOW_ARBITRARY_RIDE_TYPE_CHANGES,
    CHEAT_OWNALLLAND,
    CHEAT_DISABLERIDEVALUEAGING,
    CHEAT_IGNORERESEARCHSTATUS,
    CHEAT_ENABLEALLDRAWABLETRACKPIECES,
    CHEAT_DATE_SET,
};

enum {
    GUEST_PARAMETER_HAPPINESS,
    GUEST_PARAMETER_ENERGY,
    GUEST_PARAMETER_HUNGER,
    GUEST_PARAMETER_THIRST,
    GUEST_PARAMETER_NAUSEA,
    GUEST_PARAMETER_NAUSEA_TOLERANCE,
    GUEST_PARAMETER_BATHROOM,
    GUEST_PARAMETER_PREFERRED_RIDE_INTENSITY
};

enum {
    OBJECT_MONEY,
    OBJECT_PARK_MAP,
    OBJECT_BALLOON,
    OBJECT_UMBRELLA
};

#define CHEATS_GIVE_GUESTS_MONEY MONEY(1000,00)
#define CHEATS_TRAM_INCREMENT 250
#define CHEATS_STAFF_FAST_SPEED 0xFF
#define CHEATS_STAFF_NORMAL_SPEED 0x60
#define CHEATS_STAFF_FREEZE_SPEED 0

extern int32_t park_rating_spinner_value;
extern int32_t year_spinner_value;
extern int32_t month_spinner_value;
extern int32_t day_spinner_value;

void game_command_cheat(int32_t* eax, int32_t* ebx, int32_t* ecx, int32_t* edx, int32_t* esi, int32_t* edi, int32_t* ebp);

void cheats_reset();

const char* cheats_get_cheat_string(int cheat, int edx, int edi);

#endif
