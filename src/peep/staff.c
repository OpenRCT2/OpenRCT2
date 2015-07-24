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
#include "../config.h"
#include "../game.h"
#include "../scenario.h"
#include "../interface/viewport.h"
#include "../localisation/string_ids.h"
#include "../management/finance.h"
#include "../world/sprite.h"
#include "peep.h"
#include "staff.h"

uint32 *gStaffPatrolAreas = (uint32*)0x013B0E72;
uint8 *gStaffModes = (uint8*)0x013CA672;

/**
 *
 *  rct2: 0x006BD3A4
 */
void staff_reset_modes()
{
	for (int i = 0; i < 200; i++)
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_NONE;

	for (int i = 200; i < 204; i++)
		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] = STAFF_MODE_WALK;

	staff_update_greyed_patrol_areas();
}

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
		*ebx = MONEY32_UNDEFINED;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TOO_MANY_PEOPLE_IN_GAME;
		return;
	}

	int i;
	for (i = 0; i < STAFF_MAX_COUNT; i++) {
		if (!(RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[i] & 1))
			break;
	}

	if (i == STAFF_MAX_COUNT) {
		*ebx = MONEY32_UNDEFINED;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TOO_MANY_STAFF_IN_GAME;
		return;
	}

	int newStaffId = i;

	int _eax, _ebx, _ecx = _cx, _edx;
	_ebx = _bl;

	rct_peep* newPeep = &(create_sprite(_bl)->peep);

	if (newPeep == NULL)
	{
		*ebx = MONEY32_UNDEFINED;
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, uint16) = STR_TOO_MANY_PEOPLE_IN_GAME;
		return;
	}

	if (_bl == 0) {
		sprite_remove((rct_sprite*)newPeep);
	} else {
		move_sprite_to_list((rct_sprite *)newPeep, SPRITE_LINKEDLIST_OFFSET_PEEP);
		
		newPeep->sprite_identifier = 1;
		newPeep->window_invalidate_flags = 0;
		newPeep->action = PEEP_ACTION_NONE_2;
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

		if ((gConfigGeneral.auto_staff_placement != 0) != ((SDL_GetModState() & KMOD_SHIFT) != 0)) {
			newPeep->state = PEEP_STATE_FALLING;

			sint16 x, y, z;
			uint32 count = 0;
			uint16 sprite_index;
			rct_peep *guest = NULL;

			FOR_ALL_GUESTS(sprite_index, guest)
				if (guest->state == PEEP_STATE_WALKING) ++count;

			if (count == 0) {
				count = 0;
				uint8 i;
				for (i = 0; i < 4; ++i) {
					if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[i] != SPRITE_LOCATION_NULL) ++count;
				}

				if (count > 0) {
					uint32 rand = scenario_rand_max(count);
					for (i = 0; i < 4; ++i) {
						if (RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[i] != SPRITE_LOCATION_NULL) {
							if (rand == 0) break;
							--rand;
						}
					}

					uint8 dir = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_DIRECTION, uint8)[i];
					x = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_X, sint16)[i];
					y = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Y, sint16)[i];
					z = RCT2_ADDRESS(RCT2_ADDRESS_PARK_ENTRANCE_Z, sint16)[i];
					x += 16 + ((dir & 1) == 0 ? ((dir & 2) ? 32 : -32) : 0);
					y += 16 + ((dir & 1) == 1 ? ((dir & 2) ? -32 : 32) : 0);
				} else {
					newPeep->state = PEEP_STATE_PICKED;
					x = newPeep->x;
					y = newPeep->y;
					z = newPeep->z;
				}
			} else {
				uint32 rand = scenario_rand_max(count);
				FOR_ALL_GUESTS(sprite_index, guest)
					if (guest->state == PEEP_STATE_WALKING) {
						if (rand == 0) break;
						--rand;
					}

				x = guest->x;
				y = guest->y;
				z = guest->z;
			}

			sprite_move(x, y, z + 16, (rct_sprite*)newPeep);
			invalidate_sprite((rct_sprite*)newPeep);
		} else {
			newPeep->state = PEEP_STATE_PICKED;

			sprite_move(newPeep->x, newPeep->y, newPeep->z, (rct_sprite*)newPeep);
			invalidate_sprite((rct_sprite*)newPeep);
		}

		newPeep->time_in_park = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
		newPeep->var_CC = 0xFFFFFFFF;

		uint8 colour = RCT2_ADDRESS(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8)[staff_type > 2 ? 2 : staff_type];
		newPeep->tshirt_colour = colour;
		newPeep->trousers_colour = colour;

		// Staff energy determines their walking speed
		newPeep->energy = 0x60;
		newPeep->energy_growth_rate = 0x60;
		newPeep->var_E2 = 0;

		peep_update_name_sort(newPeep);

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

/**
 *
 *  rct2: 0x006C0BB5
 */
void game_command_set_staff_order(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_WAGES * 4;
	uint8 order_id = *ebx >> 8;
	uint16 sprite_id = *edx;
	if(*ebx & GAME_COMMAND_FLAG_APPLY){
		rct_peep *peep = &g_sprite_list[sprite_id].peep;
		if(order_id & 0x80){ // change costume
			uint8 sprite_type = order_id & ~0x80;
			sprite_type += 4;
			peep->sprite_type = sprite_type;
			peep->flags &= ~PEEP_FLAGS_SLOW_WALK;
			if(RCT2_ADDRESS(0x00982134, uint8)[sprite_type] & 1){
				peep->flags |= PEEP_FLAGS_SLOW_WALK;
			}
			peep->action_frame = 0;
			sub_693B58(peep);
			invalidate_sprite((rct_sprite*)peep);
			window_invalidate_by_number(WC_PEEP, sprite_id);
			window_invalidate_by_class(WC_STAFF_LIST);
		}else{
			peep->staff_orders = order_id;
			window_invalidate_by_number(WC_PEEP, sprite_id);
			window_invalidate_by_class(WC_STAFF_LIST);
		}
	}
	*ebx = 0;
}

/**
 *
 *  rct2: 0x006C09D1
 */
void game_command_set_staff_patrol(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	if(*ebx & GAME_COMMAND_FLAG_APPLY){
		int x = *eax;
		int y = *ecx;
		uint16 sprite_id = *edx;
		rct_peep *peep = &g_sprite_list[sprite_id].peep;
		int patrolOffset = peep->staff_id * (64 * 64 / 8);
		int patrolIndex = ((x & 0x1F80) >> 7) | ((y & 0x1F80) >> 1);
		int mask = 1 << (patrolIndex & 0x1F);
		int base = patrolIndex >> 5;

		uint32 *patrolBits = (uint32*)(0x013B0E72 + patrolOffset + (base * 4));
		*patrolBits ^= mask;

		int ispatrolling = 0;
		for(int i = 0; i < 128; i++){
			ispatrolling |= *(uint32*)(0x013B0E72 + patrolOffset + (i * 4));
		}

		RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[peep->staff_id] &= ~2;
		if(ispatrolling){
			RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[peep->staff_id] |= 2;
		}

		for(int y2 = 0; y2 < 4; y2++){
			for(int x2 = 0; x2 < 4; x2++){
				map_invalidate_tile_full((x & 0x1F80) + (x2 * 32), (y & 0x1F80) + (y2 * 32));
			}
		}
		staff_update_greyed_patrol_areas();
	}
	*ebx = 0;
}

/**
 *
 *  rct2: 0x006C0B83
 */
void game_command_fire_staff_member(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
{
	RCT2_GLOBAL(RCT2_ADDRESS_NEXT_EXPENDITURE_TYPE, uint8) = RCT_EXPENDITURE_TYPE_WAGES * 4;
	if(*ebx & GAME_COMMAND_FLAG_APPLY){
		window_close_by_class(WC_FIRE_PROMPT);
		uint16 sprite_id = *edx;
		rct_peep *peep = &g_sprite_list[sprite_id].peep;
		remove_peep_from_ride(peep);
		peep_sprite_remove(peep);
	}
	*ebx = 0;
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

	if (result == MONEY32_UNDEFINED)
		return 0xFFFF;

	return edi;
}

/**
 *
 *  rct2: 0x006C0C3F
 */
void staff_update_greyed_patrol_areas()
{
	rct_peep* peep;

	for (int staff_type = 0; staff_type < STAFF_TYPE_COUNT; ++staff_type)
	{
		for (int i = 0; i < 128; ++i)
			RCT2_ADDRESS(0x13B0E72 + ((staff_type + STAFF_MAX_COUNT) * 512), uint32)[i] = 0;
		
		for (uint16 sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_index != SPRITE_INDEX_NULL; sprite_index = peep->next)
		{
			peep = GET_PEEP(sprite_index);

			if (peep->type == PEEP_TYPE_STAFF && staff_type == peep->staff_type)
			{
				for (int i = 0; i < 128; ++i)
					RCT2_ADDRESS(0x13B0E72 + ((staff_type + STAFF_MAX_COUNT) * 512), uint32)[i] |= RCT2_ADDRESS(0x13B0E72 + (peep->staff_id * 512), uint32)[i];

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

/**
 *
 *  rct2: 0x006C1955
 */
void staff_reset_stats()
{
	uint16 spriteIndex;
	rct_peep *peep;

	FOR_ALL_STAFF(spriteIndex, peep) {
		peep->time_in_park = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
		peep->staff_lawns_mown = 0;
		peep->staff_rides_fixed = 0;
		peep->staff_gardens_watered = 0;
		peep->staff_rides_inspected = 0;
		peep->staff_litter_swept = 0;
		peep->staff_bins_emptied = 0;
	}
}

bool staff_is_patrol_area_set(int staffIndex, int x, int y)
{
	x = (x & 0x1F80) >> 7;
	y = (y & 0x1F80) >> 1;

	int peepOffset = staffIndex * 128;
	int offset = (x | y) >> 5;
	int bitIndex = (x | y) & 0x1F;
	return gStaffPatrolAreas[peepOffset + offset] & (1 << bitIndex);
}
