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

#ifndef _STAFF_H_
#define _STAFF_H_

#include "../common.h"
#include "peep.h"

#define STAFF_MAX_COUNT 0xC8
#define STAFF_TYPE_COUNT 0x04

enum STAFF_MODE {
	STAFF_MODE_NONE,
	STAFF_MODE_WALK,
	STAFF_MODE_PATROL = 3
};

enum STAFF_TYPE {
	STAFF_TYPE_HANDYMAN,
	STAFF_TYPE_MECHANIC,
	STAFF_TYPE_SECURITY,
	STAFF_TYPE_ENTERTAINER
};

enum STAFF_ORDERS{
	STAFF_ORDERS_SWEEPING = (1 << 0),
	STAFF_ORDERS_WATER_FLOWERS = (1 << 1),
	STAFF_ORDERS_EMPTY_BINS = (1 << 2),
	STAFF_ORDERS_MOWING = (1 << 3),
	STAFF_ORDERS_INSPECT_RIDES = (1 << 0),
	STAFF_ORDERS_FIX_RIDES = (1 << 1)
};

extern uint32 *gStaffPatrolAreas;
extern uint8 *gStaffModes;
#define gStaffDrawPatrolAreas	RCT2_GLOBAL(0x009DEA50, uint16)

#define gStaffColours			RCT2_ADDRESS(RCT2_ADDRESS_HANDYMAN_COLOUR, colour_t)
#define gStaffHandymanColour	RCT2_GLOBAL(RCT2_ADDRESS_HANDYMAN_COLOUR, colour_t)
#define gStaffMechanicColour	RCT2_GLOBAL(RCT2_ADDRESS_MECHANIC_COLOUR, colour_t)
#define gStaffSecurityColour	RCT2_GLOBAL(RCT2_ADDRESS_SECURITY_COLOUR, colour_t)

void game_command_update_staff_colour(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_hire_new_staff_member(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_staff_order(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_staff_patrol(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_fire_staff_member(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);
void game_command_set_staff_name(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp);

void staff_reset_modes();
void update_staff_colour(uint8 staffType, uint16 colour);
uint16 hire_new_staff_member(uint8 staffType);
void staff_update_greyed_patrol_areas();
int staff_is_location_in_patrol(rct_peep *mechanic, int x, int y);
int staff_path_finding(rct_peep* peep);
void staff_reset_stats();
bool staff_is_patrol_area_set(int staffIndex, int x, int y);

#endif
