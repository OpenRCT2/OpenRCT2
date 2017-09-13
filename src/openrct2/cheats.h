#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

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
extern bool gCheatsUnlockAllPrices;
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

extern sint32 park_rating_spinner_value;

void game_command_cheat(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

void cheats_reset();

const char* cheats_get_cheat_string(int cheat, int edx, int edi);

#endif
