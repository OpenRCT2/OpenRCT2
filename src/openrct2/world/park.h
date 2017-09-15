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

#ifndef _PARK_H_
#define _PARK_H_

#include "../common.h"
#include "map.h"

#define DECRYPT_MONEY(money) ((money32)rol32((money) ^ 0xF4EC9621, 13))
#define ENCRYPT_MONEY(money) ((money32)(ror32((money), 13) ^ 0xF4EC9621))


enum {
    PARK_FLAGS_PARK_OPEN = (1 << 0),
    PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT = (1 << 1),
    PARK_FLAGS_FORBID_LANDSCAPE_CHANGES = (1 << 2),
    PARK_FLAGS_FORBID_TREE_REMOVAL = (1 << 3),
    PARK_FLAGS_SHOW_REAL_GUEST_NAMES = (1 << 4),
    PARK_FLAGS_FORBID_HIGH_CONSTRUCTION = (1 << 5), // below tree height
    PARK_FLAGS_PREF_LESS_INTENSE_RIDES = (1 << 6),
    PARK_FLAGS_FORBID_MARKETING_CAMPAIGN = (1 << 7),
    PARK_FLAGS_ANTI_CHEAT_DEPRECATED = (1 << 8), // Not used anymore, used for cheat detection
    PARK_FLAGS_PREF_MORE_INTENSE_RIDES = (1 << 9),
    PARK_FLAGS_NO_MONEY = (1 << 11),
    PARK_FLAGS_DIFFICULT_GUEST_GENERATION = (1 << 12),
    PARK_FLAGS_PARK_FREE_ENTRY = (1 << 13),
    PARK_FLAGS_DIFFICULT_PARK_RATING = (1 << 14),
    PARK_FLAGS_LOCK_REAL_NAMES_OPTION_DEPRECATED = (1 << 15), // Deprecated now we use a persistent 'real names' setting
    PARK_FLAGS_NO_MONEY_SCENARIO = (1 << 17),  // equivalent to PARK_FLAGS_NO_MONEY, but used in scenario editor
    PARK_FLAGS_SPRITES_INITIALISED = (1 << 18), // After a scenario is loaded this prevents edits in the scenario editor
    PARK_FLAGS_SIX_FLAGS_DEPRECATED = (1 << 19) // Not used anymore
};

enum
{
    BUY_LAND_RIGHTS_FLAG_BUY_LAND,
    BUY_LAND_RIGHTS_FLAG_UNOWN_TILE,
    BUY_LAND_RIGHTS_FLAG_BUY_CONSTRUCTION_RIGHTS,
    BUY_LAND_RIGHTS_FLAG_UNOWN_CONSTRUCTION_RIGHTS,
    BUY_LAND_RIGHTS_FLAG_SET_FOR_SALE,
    BUY_LAND_RIGHTS_FLAG_SET_CONSTRUCTION_RIGHTS_FOR_SALE,
    BUY_LAND_RIGHTS_FLAG_SET_OWNERSHIP_WITH_CHECKS, // Used in scenario editor
};

#ifdef __cplusplus
extern "C" {
#endif

extern rct_string_id gParkName;
extern uint32 gParkNameArgs;
extern uint32 gParkFlags;
extern uint16 gParkRating;
extern money16 gParkEntranceFee;
extern uint16 gParkSize;
extern money16 gLandPrice;
extern money16 gConstructionRightsPrice;

extern uint32 gTotalAdmissions;
extern money32 gTotalIncomeFromAdmissions;

extern money32 gParkValue;
extern money32 gCompanyValue;

extern sint16 gParkRatingCasualtyPenalty;
extern uint8 gParkRatingHistory[32];
extern uint8 gGuestsInParkHistory[32];
extern sint32 _guestGenerationProbability;
extern sint32 _suggestedGuestMaximum;

void set_forced_park_rating(sint32 rating);
sint32 get_forced_park_rating();

sint32 park_is_open();
void park_init();
void park_reset_history();
sint32 park_calculate_size();

sint32 calculate_park_rating();
money32 calculate_park_value();
money32 calculate_company_value();
void reset_park_entry();
rct_peep * park_generate_new_guest();

void park_update();
void park_update_histories();
void update_park_fences(sint32 x, sint32 y);
void update_park_fences_around_tile(sint32 x, sint32 y);

uint8 calculate_guest_initial_happiness(uint8 percentage);

void park_set_open(sint32 open);
sint32 park_entrance_get_index(sint32 x, sint32 y, sint32 z);
void park_set_name(const char *name);
void park_set_entrance_fee(money32 value);

sint32 map_buy_land_rights(sint32 x0, sint32 y0, sint32 x1, sint32 y1, sint32 setting, sint32 flags);

void game_command_set_park_entrance_fee(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_park_open(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_park_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_buy_land_rights(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);

money16 park_get_entrance_fee();

#ifdef __cplusplus
}
#endif

#endif
