#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
	PARK_FLAGS_LOCK_REAL_NAMES_OPTION = (1 << 15),
	PARK_FLAGS_NO_MONEY_SCENARIO = (1 << 17),  // equivalent to PARK_FLAGS_NO_MONEY, but used in scenario editor
	PARK_FLAGS_18 = (1 << 18),
	PARK_FLAGS_SIX_FLAGS_DEPRECATED = (1 << 19) // Not used anymore
};

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
extern int _guestGenerationProbability;
extern int _suggestedGuestMaximum;

extern sint16 gParkEntranceX[4];
extern sint16 gParkEntranceY[4];
extern sint16 gParkEntranceZ[4];
extern uint8 gParkEntranceDirection[4];

extern bool gParkEntranceGhostExists;
extern rct_xyz16 gParkEntranceGhostPosition;
extern uint8 gParkEntranceGhostDirection;
extern money32 gParkEntranceGhostPrice;

void set_forced_park_rating(int rating);
int get_forced_park_rating();

int park_is_open();
void park_init();
void park_reset_history();
int park_calculate_size();

int calculate_park_rating();
money32 calculate_park_value();
money32 calculate_company_value();
void reset_park_entrances();
void generate_new_guest();

void park_update();
void park_update_histories();
void update_park_fences(int x, int y);

uint8 calculate_guest_initial_happiness(uint8 percentage);

void park_set_open(int open);
int park_get_entrance_index(int x, int y, int z);
void park_set_name(const char *name);
void park_set_entrance_fee(money32 value);

int map_buy_land_rights(int x0, int y0, int x1, int y1, int setting, int flags);

void game_command_set_park_entrance_fee(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_park_open(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_remove_park_entrance(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_park_name(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_buy_land_rights(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);

void map_invalidate_tile(int x, int y, int z0, int z1);

void park_remove_ghost_entrance();
money32 park_place_ghost_entrance(int x, int y, int z, int direction);

money16 park_get_entrance_fee();

#endif
