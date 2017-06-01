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

#ifndef _STAFF_H_
#define _STAFF_H_

#include "../common.h"
#include "peep.h"

#define STAFF_MAX_COUNT		200
#define STAFF_TYPE_COUNT	4

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

enum ENTERTAINER_COSTUME {
	ENTERTAINER_COSTUME_PANDA,
	ENTERTAINER_COSTUME_TIGER,
	ENTERTAINER_COSTUME_ELEPHANT,
	ENTERTAINER_COSTUME_ROMAN,
	ENTERTAINER_COSTUME_GORILLA,
	ENTERTAINER_COSTUME_SNOWMAN,
	ENTERTAINER_COSTUME_KNIGHT,
	ENTERTAINER_COSTUME_ASTRONAUT,
	ENTERTAINER_COSTUME_BANDIT,
	ENTERTAINER_COSTUME_SHERIFF,
	ENTERTAINER_COSTUME_PIRATE,

	ENTERTAINER_COSTUME_COUNT
};

extern const rct_string_id StaffCostumeNames[ENTERTAINER_COSTUME_COUNT];

extern uint32 gStaffPatrolAreas[204 * 128];
extern uint8 gStaffModes[204];
extern uint16 gStaffDrawPatrolAreas;
extern colour_t gStaffHandymanColour;
extern colour_t gStaffMechanicColour;
extern colour_t gStaffSecurityColour;

void game_command_update_staff_colour(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_hire_new_staff_member(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_callback_hire_new_staff_member(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp);
void game_command_set_staff_order(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_staff_patrol(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_fire_staff_member(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_set_staff_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp);
void game_command_pickup_staff(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp);

void staff_reset_modes();
void update_staff_colour(uint8 staffType, uint16 colour);
uint16 hire_new_staff_member(uint8 staffType);
void staff_update_greyed_patrol_areas();
sint32 staff_is_location_in_patrol(rct_peep *mechanic, sint32 x, sint32 y);
sint32 staff_path_finding(rct_peep* peep);
void staff_reset_stats();
bool staff_is_patrol_area_set(sint32 staffIndex, sint32 x, sint32 y);
void staff_set_patrol_area(sint32 staffIndex, sint32 x, sint32 y, bool value);
void staff_toggle_patrol_area(sint32 staffIndex, sint32 x, sint32 y);
colour_t staff_get_colour(uint8 staffType);
bool staff_set_colour(uint8 staffType, colour_t value);
uint32 staff_get_available_entertainer_costumes();
sint32 staff_get_available_entertainer_costume_list(uint8 * costumeList);

#endif
