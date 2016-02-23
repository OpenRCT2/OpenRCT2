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
#include "../util/util.h"
#include "../world/sprite.h"
#include "../world/footpath.h"
#include "peep.h"
#include "staff.h"

uint32 *gStaffPatrolAreas = (uint32*)RCT2_ADDRESS_STAFF_PATROL_AREAS;
uint8 *gStaffModes = (uint8*)RCT2_ADDRESS_STAFF_MODE_ARRAY;

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
	RCT2_GLOBAL(RCT2_ADDRESS_COMMAND_MAP_X, uint16) = _ax;
	RCT2_GLOBAL(RCT2_ADDRESS_COMMAND_MAP_Y, uint16) = _cx;
	RCT2_GLOBAL(RCT2_ADDRESS_COMMAND_MAP_Z, uint16) = _dx;

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

	int _eax, _ebx, _ecx = _cx;
	rct_sprite_bounds *spriteBounds;
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
		newPeep->special_sprite = 0;
		newPeep->action_sprite_image_offset = 0;
		newPeep->no_action_frame_no = 0;
		newPeep->action_sprite_type = 0;
		newPeep->var_C4 = 0;
		newPeep->type = PEEP_TYPE_STAFF;
		newPeep->outside_of_park = 0;
		newPeep->peep_flags = 0;
		newPeep->paid_to_enter = 0;
		newPeep->paid_on_rides = 0;
		newPeep->paid_on_food = 0;
		newPeep->paid_on_souvenirs = 0;

		newPeep->staff_orders = 0;
		if (staff_type == 0) {
			newPeep->staff_orders = 7;
		}
		else if (staff_type == 1) {
			newPeep->staff_orders = 3;
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

		spriteBounds = g_sprite_entries[_eax].sprite_bounds;
		newPeep->sprite_width = spriteBounds->sprite_width;
		newPeep->sprite_height_negative = spriteBounds->sprite_height_negative;
		newPeep->sprite_height_positive = spriteBounds->sprite_height_positive;

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
			invalidate_sprite_2((rct_sprite*)newPeep);
		} else {
			newPeep->state = PEEP_STATE_PICKED;

			sprite_move(newPeep->x, newPeep->y, newPeep->z, (rct_sprite*)newPeep);
			invalidate_sprite_2((rct_sprite*)newPeep);
		}

		newPeep->time_in_park = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
		newPeep->pathfind_goal.x = 0xFF;
		newPeep->pathfind_goal.y = 0xFF;
		newPeep->pathfind_goal.z = 0xFF;
		newPeep->pathfind_goal.direction = 0xFF;

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
			int addr = RCT2_ADDRESS_STAFF_PATROL_AREAS + (newStaffId << 9) + edi * 4;
			RCT2_GLOBAL(addr, uint32) = 0;
		}
	}

	if(staff_type == STAFF_TYPE_HANDYMAN && gConfigGeneral.handymen_mow_default) {
		int flags = ((newPeep->staff_orders ^ (1 << 3)) << 8) | 1;
		game_do_command(newPeep->x, flags, newPeep->y, newPeep->sprite_index, GAME_COMMAND_SET_STAFF_ORDER, (int)newPeep, 0);
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
	if (sprite_id >= MAX_SPRITES)
	{
		log_warning("Invalid game command, sprite_id = %u", sprite_id);
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		rct_peep *peep = &g_sprite_list[sprite_id].peep;
		if(order_id & 0x80){ // change costume
			uint8 sprite_type = order_id & ~0x80;
			sprite_type += 4;
			peep->sprite_type = sprite_type;
			peep->peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
			if(RCT2_ADDRESS(0x00982134, uint8)[sprite_type] & 1){
				peep->peep_flags |= PEEP_FLAGS_SLOW_WALK;
			}
			peep->action_frame = 0;
			sub_693B58(peep);
			invalidate_sprite_2((rct_sprite*)peep);
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
		if (sprite_id >= MAX_SPRITES)
		{
			*ebx = MONEY32_UNDEFINED;
			log_warning("Invalid sprite id %u", sprite_id);
			return;
		}
		rct_sprite *sprite = &g_sprite_list[sprite_id];
		if (sprite->unknown.sprite_identifier != SPRITE_IDENTIFIER_PEEP || sprite->peep.type != PEEP_TYPE_STAFF)
		{
			*ebx = MONEY32_UNDEFINED;
			log_warning("Invalid type of sprite %u for game command", sprite_id);
			return;
		}
		rct_peep *peep = &sprite->peep;
		int patrolOffset = peep->staff_id * (64 * 64 / 8);
		int patrolIndex = ((x & 0x1F80) >> 7) | ((y & 0x1F80) >> 1);
		int mask = 1u << (patrolIndex & 0x1F);
		int base = patrolIndex >> 5;

		uint32 *patrolBits = (uint32*)(RCT2_ADDRESS_STAFF_PATROL_AREAS + patrolOffset + (base * 4));
		*patrolBits ^= mask;

		int ispatrolling = 0;
		for(int i = 0; i < 128; i++){
			ispatrolling |= *(uint32*)(RCT2_ADDRESS_STAFF_PATROL_AREAS + patrolOffset + (i * 4));
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
		if (sprite_id >= MAX_SPRITES)
		{
			log_warning("Invalid game command, sprite_id = %u", sprite_id);
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		rct_peep *peep = &g_sprite_list[sprite_id].peep;
		if (peep->sprite_identifier != SPRITE_IDENTIFIER_PEEP || peep->type != PEEP_TYPE_STAFF)
		{
			log_warning("Invalid game command, peep->sprite_identifier = %u, peep->type = %u", peep->sprite_identifier, peep->type);
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		remove_peep_from_ride(peep);
		peep_sprite_remove(peep);
	}
	*ebx = 0;
}

/**
 * Updates the colour of the given staff type.
 */
void update_staff_colour(uint8 staffType, uint16 colour)
{
	game_do_command(0, (staffType << 8) | GAME_COMMAND_FLAG_APPLY, 0, (colour << 8) | 4, GAME_COMMAND_SET_STAFF_COLOUR, 0, 0);
}

/**
 * Hires a new staff member of the given type. If the hire cannot be completed (eg. the maximum
 * number of staff is reached or there are too many people in the game) it returns 0xFFFF.
 */
uint16 hire_new_staff_member(uint8 staffType)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_HIRE_NEW_STAFF;

	int eax, ebx, ecx, edx, esi, edi, ebp;
	ecx = edx = esi = edi = ebp = 0;
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
			RCT2_ADDRESS(RCT2_ADDRESS_STAFF_PATROL_AREAS + ((staff_type + STAFF_MAX_COUNT) * 512), uint32)[i] = 0;

		for (uint16 sprite_index = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_PEEP, uint16); sprite_index != SPRITE_INDEX_NULL; sprite_index = peep->next)
		{
			peep = GET_PEEP(sprite_index);

			if (peep->type == PEEP_TYPE_STAFF && staff_type == peep->staff_type)
			{
				for (int i = 0; i < 128; ++i)
					RCT2_ADDRESS(RCT2_ADDRESS_STAFF_PATROL_AREAS + ((staff_type + STAFF_MAX_COUNT) * 512), uint32)[i] |= RCT2_ADDRESS(RCT2_ADDRESS_STAFF_PATROL_AREAS + (peep->staff_id * 512), uint32)[i];

			}
		}
	}
}

static int staff_is_location_in_patrol_area(rct_peep *peep, int x, int y)
{
	// Patrol quads are stored in a bit map (8 patrol quads per byte)
	// Each patrol quad is 4x4
	// Therefore there are in total 64 x 64 patrol quads in the 256 x 256 map
	int patrolOffset = peep->staff_id * (64 * 64 / 8);
	int patrolIndex = ((x & 0x1F80) >> 7) | ((y & 0x1F80) >> 1);
	int mask = 1u << (patrolIndex & 0x1F);
	int base = patrolIndex >> 5;

	uint32 *patrolBits = (uint32*)(RCT2_ADDRESS_STAFF_PATROL_AREAS + patrolOffset + (base * 4));
	return (*patrolBits & mask) != 0;
}

/**
 *
 *  rct2: 0x006C0905
 */
int staff_is_location_in_patrol(rct_peep *staff, int x, int y)
{
	// Check if location is in the park
	if (!map_is_location_owned(x, y, staff->z))
		return 0;

	// Check if staff has patrol area
	if (!(RCT2_ADDRESS(RCT2_ADDRESS_STAFF_MODE_ARRAY, uint8)[staff->staff_id] & 2))
		return 1;

	return staff_is_location_in_patrol_area(staff, x, y);
}

/**
 *
 *  rct2: 0x006C095B
 *  returns 0xF if not in a valid patrol area
 */
static uint8 staff_get_valid_patrol_directions(rct_peep* peep, sint16 x, sint16 y) {
	uint8 directions = 0;

	if (staff_is_location_in_patrol(peep, x - 32, y)) {
		directions |= (1 << 0);
	}

	if (staff_is_location_in_patrol(peep, x, y + 32)) {
		directions |= (1 << 1);
	}

	if (staff_is_location_in_patrol(peep, x + 32, y)) {
		directions |= (1 << 2);
	}

	if (staff_is_location_in_patrol(peep, x, y - 32)) {
		directions |= (1 << 3);
	}

	if (directions == 0) {
		directions = 0xF;
	}

	// For backwards compatibility. 
	// Remove when all references to 0x00F43910 removed
	RCT2_GLOBAL(0x00F43910, uint32) = directions;

	return directions;
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

/**
 * 
 *  rct2: 0x006BFBE8
 * 
 * Returns 0xFF when no nearby litter or unpathable litter
 */
static uint8 staff_handyman_direction_to_nearest_litter(rct_peep* peep){
	uint16 nearestLitterDist = (uint16)-1;
	rct_litter* nearestLitter = NULL;
	rct_litter* litter = NULL;
	
	for(uint16 litterIndex = RCT2_GLOBAL(RCT2_ADDRESS_SPRITES_START_LITTER, uint16); litterIndex != 0xFFFF; litterIndex = litter->next){
		litter = &g_sprite_list[litterIndex].litter;
		
		uint16 distance = 
			abs(litter->x - peep->x) + 
			abs(litter->y - peep->y) + 
			abs(litter->z - peep->z) / 4;
	
		if (distance < nearestLitterDist){
			nearestLitterDist = distance;
			nearestLitter = litter;
		}
	}
	
	if (nearestLitterDist > 0x60){
		return 0xFF;
	}
	
	rct_xy16 litterTile = { 
		.x = litter->x & 0xFFE0,
		.y = litter->y & 0xFFE0
	};
	
	if (!staff_is_location_in_patrol(peep, litterTile.x, litterTile.y)){
		return 0xFF;
	}
	
	litterTile.x += 16;
	litterTile.y += 16;
	
	sint16 x_diff = litterTile.x - peep->x;
	sint16 y_diff = litterTile.y - peep->y;
	
	uint8 nextDirection = 0;
	
	if (abs(x_diff) <= abs(y_diff)){
		nextDirection = y_diff < 0 ? 3 : 1;
	}
	else {
		nextDirection = x_diff < 0 ? 0 : 2;
	}
	
	rct_xy16 nextTile = {
		.x = (litter->x & 0xFFE0) - TileDirectionDelta[nextDirection].x,
		.y = (litter->y & 0xFFE0) - TileDirectionDelta[nextDirection].y
	};
	
	sint16 nextZ = ((peep->z + 8) & 0xFFF0) / 8;
	
	rct_map_element* mapElement = map_get_first_element_at(nextTile.x / 32, nextTile.y / 32);
	
	do {
		if (mapElement->base_height != nextZ)
			continue;
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE ||
			map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK){
			return 0xFF;
		}
	} while(!map_element_is_last_for_tile(mapElement++));
	
	nextTile.x = (peep->x & 0xFFE0) + TileDirectionDelta[nextDirection].x;
	nextTile.y = (peep->y & 0xFFE0) + TileDirectionDelta[nextDirection].y;
	
	mapElement = map_get_first_element_at(nextTile.x / 32, nextTile.y / 32);
			
	do {
		if (mapElement->base_height != nextZ)
			continue;
		if (map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE ||
			map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_TRACK){
			return 0xFF;
		}
	} while(!map_element_is_last_for_tile(mapElement++));
	
	return nextDirection;
}

/**
 *
 *  rct2: 0x006BF931
 */
static uint8 staff_handyman_direction_to_uncut_grass(rct_peep* peep, uint8 valid_directions) {
	if (!(peep->next_var_29 & 0x18)) {

		rct_map_element* mapElement = map_get_surface_element_at(peep->next_x / 32, peep->next_y / 32);

		if (peep->next_z != mapElement->base_height)
			return 0xFF;

		if (peep->next_var_29 & 0x4) {
			if ((mapElement->properties.surface.slope & MAP_ELEMENT_SLOPE_MASK) != RCT2_ADDRESS(0x0098D800, uint8)[peep->next_var_29 & 0x3])
				return 0xFF;
		}
		else if ((mapElement->properties.surface.slope & MAP_ELEMENT_SLOPE_MASK) != 0)
			return 0xFF;
	}

	uint8 chosenDirection = scenario_rand() & 0x3;
	for (uint8 i = 0; i < 4; ++i, ++chosenDirection) {
		chosenDirection &= 0x3;

		if (!(valid_directions & (1 << chosenDirection))) {
			continue;
		}

		rct_xy16 chosenTile = {
			.x = peep->next_x + TileDirectionDelta[chosenDirection].x,
			.y = peep->next_y + TileDirectionDelta[chosenDirection].y,
		};

		if (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF)
			continue;

		rct_map_element* mapElement = map_get_surface_element_at(chosenTile.x / 32, chosenTile.y / 32);

		if (map_element_get_terrain(mapElement) != 0)
			continue;

		if (abs(mapElement->base_height - peep->next_z) > 2)
			continue;

		if (!(mapElement->properties.surface.grass_length & GRASS_LENGTH_CLUMPS_2))
			continue;

		return chosenDirection;
	}
	return 0xFF;
}

/**
 *
 *  rct2: 0x006BFD9C
 */
static int staff_handyman_direction_rand_surface(rct_peep* peep, uint8 validDirections) {
	uint8 direction = scenario_rand() & 3;
	for (int i = 0; i < 4; ++i, ++direction) {
		direction &= 3;
		if (!(validDirections & (1 << direction)))
			continue;

		rct_xy16 chosenTile = {
			.x = peep->next_x + TileDirectionDelta[direction].x,
			.y = peep->next_y + TileDirectionDelta[direction].y,
		};

		if (map_surface_is_blocked(chosenTile.x, chosenTile.y))
			continue;

		break;
	}
	// If it tries all directions this is required
	// to make it back to the first direction and 
	// override validDirections
	direction &= 3;
	return direction;
}

/**
 *
 *  rct2: 0x006BFBA8
 */
static int staff_path_finding_handyman(rct_peep* peep) {
	peep->var_E2++;

	RCT2_GLOBAL(0x00F43918, uint8) = 0xFF;
	uint8 validDirections = staff_get_valid_patrol_directions(peep, peep->next_x, peep->next_y);

	if (peep->staff_orders & STAFF_ORDERS_SWEEPING &&
		((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) + peep->sprite_index) & 0xFFF) > 110) {
		RCT2_GLOBAL(0x00F43918, uint8) = staff_handyman_direction_to_nearest_litter(peep);
	}
	
	uint8 direction = 0xFF;
	if (RCT2_GLOBAL(0x00F43918, uint8) == 0xFF && 
		(peep->staff_orders & STAFF_ORDERS_MOWING) &&
		peep->var_E2 >= 12) {
		direction = staff_handyman_direction_to_uncut_grass(peep, validDirections);
	}



	if (direction == 0xFF) {
		if (peep->next_var_29 & 0x18) {
			direction = staff_handyman_direction_rand_surface(peep, validDirections);
		}
		else {
			rct_map_element* mapElement = map_get_path_element_at(
				peep->next_x / 32,
				peep->next_y / 32,
				peep->next_z);

			if (mapElement == NULL)
				return 1;

			uint8 pathDirections = (mapElement->properties.path.edges & validDirections) & 0xF;
			if (pathDirections == 0) {
				direction = staff_handyman_direction_rand_surface(peep, validDirections);
			}
			else {
				bool chooseRandom = true;
				if (RCT2_GLOBAL(0x00F43918, uint8) != 0xFF &&
					pathDirections & (1 << RCT2_GLOBAL(0x00F43918, uint8))) {

					if ((scenario_rand() & 0xFFFF) >= 0x1999) {
						chooseRandom = false;
						direction = RCT2_GLOBAL(0x00F43918, uint8);
					}
				}
				else {
					pathDirections &= ~(1 << (peep->var_78 ^ (1 << 1)));
					if (pathDirections == 0) {
						pathDirections |= 1 << (peep->var_78 ^ (1 << 1));
					}
				}


				if (chooseRandom == true) {
					do {
						direction = scenario_rand() & 3;
					} while ((pathDirections & (1 << direction)) == 0);
				}
			}
		}

	}

	// countof(TileDirectionDelta)
	assert(direction < 8);

	rct_xy16 chosenTile = {
		.x = peep->next_x + TileDirectionDelta[direction].x,
		.y = peep->next_y + TileDirectionDelta[direction].y
	};

	while (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF) {
		direction = staff_handyman_direction_rand_surface(peep, validDirections);
		chosenTile.x = peep->next_x + TileDirectionDelta[direction].x;
		chosenTile.y = peep->next_y + TileDirectionDelta[direction].y;
	}

	peep->var_78 = direction;
	peep->destination_x = chosenTile.x + 16;
	peep->destination_y = chosenTile.y + 16;
	peep->destination_tolerence = 3;
	if (peep->state == PEEP_STATE_QUEUING) {
		peep->destination_tolerence = (scenario_rand() & 7) + 2;
	}
	return 0;
}

uint8 staff_direction_surface(rct_peep* peep, uint8 initialDirection) {
	uint8 direction = initialDirection;
	for (int i = 0; i < 3; ++i) {
		// Looks left and right from initial direction
		switch (i) {
		case 1:
			direction++;
			if (scenario_rand() & 1) {
				direction -= 2;
			}
			break;
		case 2:
			direction -= 2;
			break;
		}

		direction &= 3;

		if (fence_in_the_way(
			peep->next_x,
			peep->next_y,
			peep->next_z,
			peep->next_z + 4,
			direction) == true)
			continue;

		if (fence_in_the_way(
			peep->next_x,
			peep->next_y,
			peep->next_z,
			peep->next_z + 4,
			direction ^ (1 << 1)) == true)
			continue;

		rct_xy16 chosenTile = {
			.x = peep->next_x + TileDirectionDelta[direction].x,
			.y = peep->next_y + TileDirectionDelta[direction].y
		};

		if (map_surface_is_blocked(chosenTile.x, chosenTile.y) == false) {
			return direction;
		}
	}
	return initialDirection;
}

/**
 *
 *  rct2: 0x006BFF45
 */
static uint8 staff_mechanic_direction_surface(rct_peep* peep) {
	uint8 direction = scenario_rand() & 3;

	if ((peep->state == PEEP_STATE_ANSWERING || peep->state == PEEP_STATE_HEADING_TO_INSPECTION) &&
		scenario_rand() & 1) {

		rct_ride* ride = get_ride(peep->current_ride);

		uint16 location = ride->exits[peep->current_ride_station];
		if (location == 0xFFFF) {
			location = ride->entrances[peep->current_ride_station];
		}

		rct_xy16 chosenTile = {
			.x = (location & 0xFF) * 32,
			.y = (location >> 8) * 32
		};

		sint16 x_diff = chosenTile.x - peep->x;
		sint16 y_diff = chosenTile.y - peep->y;

		if (abs(x_diff) <= abs(y_diff)) {
			direction = y_diff < 0 ? 3 : 1;
		}
		else {
			direction = x_diff < 0 ? 0 : 2;
		}
	}

	return staff_direction_surface(peep, direction);
}

/**
 *
 *  rct2: 0x006C02D1
 */
static uint8 staff_mechanic_direction_path_rand(rct_peep* peep, uint8 pathDirections) {
	if (scenario_rand() & 1) {
		if (pathDirections & (1 << peep->var_78))
			return peep->var_78;
	}
	
	// Modified from original to spam scenario_rand less
	uint8 direction = scenario_rand() & 3;
	for (int i = 0; i < 4; ++i, ++direction) {
		direction &= 3;
		if (pathDirections & (1 << direction))
			return direction;
	}
	// This will never happen as pathDirections always has a bit set.
	return peep->var_78;
}

/**
 *
 *  rct2: 0x006C0121
 */
static uint8 staff_mechanic_direction_path(rct_peep* peep, uint8 validDirections, rct_map_element* pathElement) {
	uint8 direction = 0xFF;
	uint8 pathDirections = pathElement->properties.path.edges & 0xF;
	if (peep->state != PEEP_STATE_ANSWERING && peep->state != PEEP_STATE_HEADING_TO_INSPECTION) {
		pathDirections &= validDirections;
	}

	if (pathDirections == 0) {
		return staff_mechanic_direction_surface(peep);
	}

	pathDirections &= ~(1 << (peep->var_78 ^ (1 << 1)));
	if (pathDirections == 0) {
		pathDirections |= (1 << (peep->var_78 ^ (1 << 1)));
	}

	direction = bitscanforward(pathDirections);
	pathDirections &= ~(1 << direction);
	if (pathDirections == 0) {
		if (peep->state != PEEP_STATE_ANSWERING && peep->state != PEEP_STATE_HEADING_TO_INSPECTION) {
			return direction;
		}

		if (peep->sub_state != 2) {
			return direction;
		}
		peep->sub_state = 3;
	}

	pathDirections |= (1 << direction);

	if (peep->state == PEEP_STATE_ANSWERING || peep->state == PEEP_STATE_HEADING_TO_INSPECTION) {
		rct_ride* ride = get_ride(peep->current_ride);
		uint8 z = ride->station_heights[peep->current_ride_station];
		RCT2_GLOBAL(RCT2_ADDRESS_PEEP_PATHFINDING_GOAL_Z, uint8) = z;

		uint16 location = ride->exits[peep->current_ride_station];
		if (location == 0xFFFF) {
			location = ride->entrances[peep->current_ride_station];
		}

		rct_xy16 chosenTile = {
			.x = (location & 0xFF) * 32,
			.y = (location >> 8) * 32
		};

		RCT2_GLOBAL(RCT2_ADDRESS_PEEP_PATHFINDING_GOAL_X, sint16) = chosenTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_PEEP_PATHFINDING_GOAL_Y, sint16) = chosenTile.y;

		bool entranceFound = false;
		rct_map_element* mapElement = map_get_first_element_at(chosenTile.x / 32, chosenTile.y / 32);
		do {
			if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_ENTRANCE)
				continue;

			if (mapElement->base_height != z)
				continue;

			if (mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_ENTRANCE &&
				mapElement->properties.entrance.type != ENTRANCE_TYPE_RIDE_EXIT)
				continue;

			entranceFound = true;
			break;
		} while (!map_element_is_last_for_tile(mapElement++));

		if (entranceFound == false) {
			return staff_mechanic_direction_path_rand(peep, pathDirections);
		}

		uint8 entranceDirection = map_element_get_direction(mapElement);
		chosenTile.x -= TileDirectionDelta[entranceDirection].x;
		chosenTile.y -= TileDirectionDelta[entranceDirection].y;
		RCT2_GLOBAL(RCT2_ADDRESS_PEEP_PATHFINDING_GOAL_X, sint16) = chosenTile.x;
		RCT2_GLOBAL(RCT2_ADDRESS_PEEP_PATHFINDING_GOAL_Y, sint16) = chosenTile.y;

		if (chosenTile.x == peep->next_x &&
			chosenTile.y == peep->next_y &&
			z == peep->next_z) {
			return entranceDirection;
		}

		RCT2_GLOBAL(0x00F1AEE0, uint8) = 0;
		RCT2_GLOBAL(0x00F1AEE1, uint8) = 0xFF;

		int pathfindDirection = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

		if (pathfindDirection == -1) {
			return staff_mechanic_direction_path_rand(peep, pathDirections);
		}

		return (uint8)pathfindDirection;
	}
	return staff_mechanic_direction_path_rand(peep, pathDirections);
}

/**
 *
 *  rct2: 0x006BFF2C
 */
static int staff_path_finding_mechanic(rct_peep* peep) {
	uint8 validDirections = staff_get_valid_patrol_directions(peep, peep->next_x, peep->next_y);
	uint8 direction = 0xFF;
	if (peep->next_var_29 & 0x18) {
		direction = staff_mechanic_direction_surface(peep);
	}
	else {
		rct_map_element* pathElement = map_get_path_element_at(peep->next_x / 32, peep->next_y / 32, peep->next_z);
		if (pathElement == NULL)
			return 1;

		direction = staff_mechanic_direction_path(peep, validDirections, pathElement);
	}

	// countof(TileDirectionDelta)
	assert(direction < 8);

	rct_xy16 chosenTile = {
		.x = peep->next_x + TileDirectionDelta[direction].x,
		.y = peep->next_y + TileDirectionDelta[direction].y
	};

	while (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF) {
		direction = staff_mechanic_direction_surface(peep);
		chosenTile.x = peep->next_x + TileDirectionDelta[direction].x;
		chosenTile.y = peep->next_y + TileDirectionDelta[direction].y;
	}

	peep->var_78 = direction;
	peep->destination_x = chosenTile.x + 16;
	peep->destination_y = chosenTile.y + 16;
	peep->destination_tolerence = (scenario_rand() & 7) + 2;
	
	return 0;
}

/**
*
*  rct2: 0x006C050B
*/
static uint8 staff_direction_path(rct_peep* peep, uint8 validDirections, rct_map_element* pathElement) {
	uint8 direction = 0xFF;
	uint8 pathDirections = pathElement->properties.path.edges & 0xF;
	if (peep->state != PEEP_STATE_ANSWERING && peep->state != PEEP_STATE_HEADING_TO_INSPECTION) {
		pathDirections &= validDirections;
	}

	if (pathDirections == 0) {
		return staff_direction_surface(peep, scenario_rand() & 3);
	}

	pathDirections &= ~(1 << (peep->var_78 ^ (1 << 1)));
	if (pathDirections == 0) {
		pathDirections |= (1 << (peep->var_78 ^ (1 << 1)));
	}

	direction = bitscanforward(pathDirections);
	pathDirections &= ~(1 << direction);
	if (pathDirections == 0) {
		return direction;
	}

	pathDirections |= (1 << direction);

	direction = scenario_rand() & 3;
	for (int i = 0; i < 4; ++i, ++direction) {
		direction &= 3;
		if (pathDirections & (1 << direction))
			return direction;
	}

	// This will never happen as pathDirections will always have a bit set
	return direction;
}

/**
 *
 *  rct2: 0x006C0351
 */
static int staff_path_finding_misc(rct_peep* peep) {
	uint8 validDirections = staff_get_valid_patrol_directions(peep, peep->next_x, peep->next_y);

	uint8 direction = 0xFF;
	if (peep->next_var_29 & 0x18) {
		direction = staff_direction_surface(peep, scenario_rand() & 3);
	}
	else {
		rct_map_element* pathElement = map_get_path_element_at(peep->next_x / 32, peep->next_y / 32, peep->next_z);
		if (pathElement == NULL)
			return 1;

		direction = staff_direction_path(peep, validDirections, pathElement);
	}

	rct_xy16 chosenTile = {
		.x = peep->next_x + TileDirectionDelta[direction].x,
		.y = peep->next_y + TileDirectionDelta[direction].y
	};

	while (chosenTile.x > 0x1FFF || chosenTile.y > 0x1FFF) {
		direction = staff_direction_surface(peep, scenario_rand() & 3);
		chosenTile.x = peep->next_x + TileDirectionDelta[direction].x;
		chosenTile.y = peep->next_y + TileDirectionDelta[direction].y;
	}

	peep->var_78 = direction;
	peep->destination_x = chosenTile.x + 16;
	peep->destination_y = chosenTile.y + 16;
	peep->destination_tolerence = (scenario_rand() & 7) + 2;

	return 0;
}

/**
 *
 *  rct2: 0x006C086D
 */
static void staff_entertainer_update_nearby_peeps(rct_peep* peep) {
	uint16 spriteIndex;
	rct_peep* guest;

	FOR_ALL_GUESTS(spriteIndex, guest) {
		if (guest->x == SPRITE_LOCATION_NULL)
			continue;

		sint16 z_dist = abs(peep->z - guest->z);
		if (z_dist > 48)
			continue;

		sint16 x_dist = abs(peep->x - guest->x);
		sint16 y_dist = abs(peep->y - guest->y);

		if (x_dist > 96)
			continue;

		if (y_dist > 96)
			continue;

		if (peep->state == PEEP_STATE_WALKING) {
			peep->happiness_growth_rate = min(peep->happiness_growth_rate + 4, 255);
		}
		else if (peep->state == PEEP_STATE_QUEUING) {
			peep->time_in_queue -= 200;
			peep->happiness_growth_rate = min(peep->happiness_growth_rate + 3, 255);
		}
	}
}

/**
 *
 *  rct2: 0x006C05AE
 */
static int staff_path_finding_entertainer(rct_peep* peep) {

	if (((scenario_rand() & 0xFFFF) <= 0x4000) &&
		(peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2)) {
		
		invalidate_sprite_2((rct_sprite*)peep);
		
		peep->action = scenario_rand() & 1 ? PEEP_ACTION_WAVE_2 : PEEP_ACTION_JOY;
		peep->action_frame = 0;
		peep->action_sprite_image_offset = 0;

		sub_693B58(peep);
		invalidate_sprite_2((rct_sprite*)peep);
		staff_entertainer_update_nearby_peeps(peep);
	}

	return staff_path_finding_misc(peep);
}

/**
 *
 *  rct2: 0x006BF926
 */
int staff_path_finding(rct_peep* peep) {
	switch (peep->staff_type) {
	case STAFF_TYPE_HANDYMAN:
		return staff_path_finding_handyman(peep);
	case STAFF_TYPE_MECHANIC:
		return staff_path_finding_mechanic(peep);
	case STAFF_TYPE_SECURITY:
		return staff_path_finding_misc(peep);
	case STAFF_TYPE_ENTERTAINER:
		return staff_path_finding_entertainer(peep);

	default:
		assert(false);
		return 0;
	}
}
