/*****************************************************************************
* Copyright (c) 2014 Dániel Tar
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "../addresses.h"
#include "../game.h"
#include "../interface/viewport.h"
#include "../localisation/string_ids.h"
#include "../management/finance.h"
#include "../world/sprite.h"
#include "peep.h"
#include "staff.h"

/**
 *
 *  rct2: 0x00669E55
 */
void game_command_update_staff_colour(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	uint8 staffType, colour;
	int spriteIndex;
	rct_peep *peep;
	
	staffType = (*ebx >> 8) & 0xFF;
	colour = (*edx >> 8) & 0xFF;

	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		RCT2_ADDRESS(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8)[staffType] = colour;

		FOR_ALL_PEEPS(spriteIndex, peep) {
			if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == staffType) {
				peep->tshirt_colour = colour;
				peep->trousers_colour = colour;
			}
		}
	}

	gfx_invalidate_screen();
	*ebx = 0;
}

/**
 *
 *  rct2: 0x006BEFA1
 */
void game_command_hire_new_staff_member(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp)
{
	uint8 _bl = *ebx & 0xFF, staff_type = (*ebx & 0xFF00) >> 8;
	uint16 _ax = *eax & 0xFFFF, _cx = *ecx & 0xFFFF, _dx = *edx & 0xFFFF;

	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_WAGES * 4;
	RCT2_GLOBAL(0x009DEA5E, uint16) = _ax;
	RCT2_GLOBAL(0x009DEA60, uint16) = _cx;
	RCT2_GLOBAL(0x009DEA62, uint16) = _dx;

	if (RCT2_GLOBAL(0x13573C8, uint16) < 0x190) {
		*ebx = 0x80000000;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TOO_MANY_PEOPLE_IN_GAME;
		return;
	}

	int i;
	for (i = 0; i < STAFF_MAX_COUNT; i++) {
		if (!(RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] & 1))
			break;
	}

	if (i == STAFF_MAX_COUNT) {
		*ebx = 0x80000000;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TOO_MANY_STAFF_IN_GAME;
		return;
	}

	int newStaffId = i;

	int _eax, _ebx, _ecx = _cx, _edx;
	_ebx = _bl;

	rct_peep* newPeep = &(create_sprite(_bl)->peep);

	if (newPeep == NULL)
	{
		*ebx = 0x80000000;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TOO_MANY_PEOPLE_IN_GAME;
		return;
	}

	if (_bl == 0) {
		sprite_remove((rct_sprite*)newPeep);
	} else {
		move_sprite_to_list((rct_sprite *)newPeep, SPRITE_LINKEDLIST_OFFSET_PEEP);

		newPeep->sprite_identifier = 1;
		newPeep->sprite_height_negative = 0x0F;
		newPeep->sprite_height_positive = 5;
		newPeep->sprite_width = 8;
		newPeep->sprite_direction = 0;

		sprite_move(_ax, *ecx, _dx, (rct_sprite*)newPeep);

		newPeep->state = PEEP_STATE_PICKED;
		if (newPeep->x != -32768) {
			newPeep->state = 0;
		}

		newPeep->var_45 = 0;
		newPeep->action = 0xFF;
		newPeep->var_6D = 0;
		newPeep->action_sprite_image_offset = 0;
		newPeep->no_action_frame_no = 0;
		newPeep->action_sprite_type = 0;
		newPeep->var_C4 = 0;
		newPeep->type = PEEP_TYPE_STAFF;
		newPeep->var_2A = 0;
		newPeep->flags = 0;
		newPeep->paid_to_enter = 0;
		newPeep->paid_on_rides = 0;
		newPeep->paid_on_food = 0;
		newPeep->paid_on_souvenirs = 0;

		newPeep->var_C6 = 0;
		if (staff_type == 0) {
			newPeep->var_C6 = 7;
		}
		else if (staff_type == 1) {
			newPeep->var_C6 = 3;
		}

		newPeep->staff_type = 0xFF;

		uint16 idSearchSpriteIndex;
		rct_peep* idSearchPeep;

		// We search for the first available id for a given staff type
		int newStaffIndex = 0;
		for (;;) {
			int found = 0;
			newStaffIndex++;

			FOR_ALL_STAFF(idSearchSpriteIndex, idSearchPeep) {
				if (idSearchPeep->staff_type != staff_type) {
					continue;
				}

				if (idSearchPeep->id == newStaffIndex) {
					found = 1;
					break;
				}
			}

			if (found == 0)
				break;
		}

		newPeep->id = newStaffIndex;
		newPeep->staff_type = staff_type;

		_eax = RCT2_ADDRESS(0x009929FC, uint8)[staff_type];
		newPeep->name_string_idx = staff_type + 0x300;
		newPeep->sprite_type = _eax;

		_edx = RCT2_ADDRESS(0x0098270C, uint32)[_eax * 2];
		newPeep->sprite_width = *((uint8*)_edx);
		newPeep->sprite_height_negative = *((uint8*)(_edx + 1));
		newPeep->sprite_height_positive = *((uint8*)(_edx + 2));

		sprite_move( newPeep->x, newPeep->y, newPeep->z, (rct_sprite*)newPeep);
		invalidate_sprite((rct_sprite*)newPeep);

		newPeep->time_in_park = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
		newPeep->var_CC = 0xFFFFFFFF;

		uint8 colour = RCT2_ADDRESS(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8)[staff_type > 2 ? 2 : staff_type];
		newPeep->tshirt_colour = colour;
		newPeep->trousers_colour = colour;

		newPeep->energy = 0x60;
		newPeep->energy_growth_rate = 0x60;
		newPeep->var_E2 = 0;

		RCT2_CALLPROC_X(0x00699115, (uint32)ebp & 0xFFFFFF3F, 0, 0, 0, (int)newPeep, 0,
			(*ebp << 25) | (*ebp >> 6));

		newPeep->staff_id = newStaffId;

		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[newStaffId] = STAFF_MODE_WALK;

		for (int edi = 0; edi < 0x80; edi++) {
			int addr = 0x013B0E72 + (newStaffId << 9) + edi * 4;
			RCT2_GLOBAL(addr, uint32) = 0;
		}
	}

	*ebx = 0;
	*edi = newPeep->sprite_index;
}

/*
 * Updates the colour of the given staff type.
 */
void update_staff_colour(uint8 staffType, uint16 colour)
{
	game_do_command(0, (staffType << 8) | GAME_COMMAND_FLAG_APPLY, 0, (colour << 8) | 4, GAME_COMMAND_SET_STAFF_COLOUR, 0, 0);
}

/*
 * Hires a new staff member of the given type. If the hire cannot be completed (eg. the maximum
 * number of staff is reached or there are too many people in the game) it returns 0xFFFF.
 */
uint16 hire_new_staff_member(uint8 staffType)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_CANT_HIRE_NEW_STAFF;

	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = 0x8000;
	ebx = staffType << 8 | GAME_COMMAND_FLAG_APPLY;

	int result = game_do_command_p(GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);

	if (result == 0x80000000)
		return 0xFFFF;

	return edi;
}

void sub_6C0C3F()
{
	register rct_peep* peep;

	for (register uint8 staff_type = 0; staff_type < STAFF_TYPE_COUNT; ++staff_type)
	{
		for (register uint8 i = 0; i < 128; ++i)
			RCT2_ADDRESS(0x13B0E72 + (staff_type + STAFF_MAX_COUNT) * 512, uint32)[i] = 0;
		
		for (register uint16 sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_index != SPRITE_INDEX_NULL; sprite_index = peep->next)
		{
			peep = GET_PEEP(sprite_index);

			if (peep->type == PEEP_TYPE_STAFF && staff_type == peep->staff_type)
			{
				for (register uint8 i = 0; i < 128; ++i)
					RCT2_ADDRESS(0x13B0E72 + (staff_type + STAFF_MAX_COUNT) * 512, uint32)[i] |= RCT2_ADDRESS(0x13B0E72 + (peep->staff_id * 512) * 512, uint32)[i];

			}
		}
	}
}

int staff_is_location_in_patrol_area(rct_peep *peep, int x, int y)
{
	// Patrol quads are stored in a bit map (8 patrol quads per byte)
	// Each patrol quad is 4x4
	// Therefore there are in total 64 x 64 patrol quads in the 256 x 256 map
	int patrolOffset = peep->staff_id * (64 * 64 / 8);
	int patrolIndex = ((x & 0x1F80) >> 7) | ((y & 0x1F80) >> 1);
	int mask = 1 << (patrolIndex & 0x1F);
	int base = patrolIndex >> 5;

	uint32 *patrolBits = (uint32*)(0x013B0E72 + patrolOffset + (base * 4));
	return (*patrolBits & mask) != 0;
}

/**
 *
 *  rct2: 0x006C0905
 */
int mechanic_is_location_in_patrol(rct_peep *mechanic, int x, int y)
{
	// Check if location is in the park
	if (!map_is_location_owned(x, y, mechanic->z))
		return 0;

	// Check if mechanic has patrol area
	if (!(RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[mechanic->staff_id] & 2))
		return 1;

	return staff_is_location_in_patrol_area(mechanic, x, y);
}