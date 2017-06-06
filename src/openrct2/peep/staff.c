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

#include "../config/Config.h"
#include "../Context.h"
#include "../game.h"
#include "../input.h"
#include "../interface/viewport.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../localisation/string_ids.h"
#include "../management/finance.h"
#include "../network/network.h"
#include "../scenario/scenario.h"
#include "../util/util.h"
#include "../world/entrance.h"
#include "../world/footpath.h"
#include "../world/scenery.h"
#include "../world/sprite.h"
#include "peep.h"
#include "staff.h"

const rct_string_id StaffCostumeNames[] = {
		STR_STAFF_OPTION_COSTUME_PANDA,
		STR_STAFF_OPTION_COSTUME_TIGER,
		STR_STAFF_OPTION_COSTUME_ELEPHANT,
		STR_STAFF_OPTION_COSTUME_ROMAN,
		STR_STAFF_OPTION_COSTUME_GORILLA,
		STR_STAFF_OPTION_COSTUME_SNOWMAN,
		STR_STAFF_OPTION_COSTUME_KNIGHT,
		STR_STAFF_OPTION_COSTUME_ASTRONAUT,
		STR_STAFF_OPTION_COSTUME_BANDIT,
		STR_STAFF_OPTION_COSTUME_SHERIFF,
		STR_STAFF_OPTION_COSTUME_PIRATE,
};

uint32 gStaffPatrolAreas[204 * 128];
uint8 gStaffModes[204];
uint16 gStaffDrawPatrolAreas;
colour_t gStaffHandymanColour;
colour_t gStaffMechanicColour;
colour_t gStaffSecurityColour;

/**
 *
 *  rct2: 0x006BD3A4
 */
void staff_reset_modes()
{
	for (sint32 i = 0; i < 200; i++)
		gStaffModes[i] = STAFF_MODE_NONE;

	for (sint32 i = 200; i < 204; i++)
		gStaffModes[i] = STAFF_MODE_WALK;

	staff_update_greyed_patrol_areas();
}

/**
 *
 *  rct2: 0x00669E55
 */
void game_command_update_staff_colour(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		uint8 staffType = (*ebx >> 8) & 0xFF;
		uint8 colour = (*edx >> 8) & 0xFF;

		// Client may send invalid data
		bool ok = staff_set_colour(staffType, colour);
		if (!ok) {
			*ebx = MONEY32_UNDEFINED;
			return;
		}

		sint32 spriteIndex;
		rct_peep *peep;
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

static inline void staff_autoposition_new_staff_member(rct_peep *newPeep)
{
	// Find a location to place new staff member

	newPeep->state = PEEP_STATE_FALLING;

	sint16 x, y, z;
	uint32 count = 0;
	uint16 sprite_index;
	rct_peep *guest = NULL;

	// Count number of walking guests
	FOR_ALL_GUESTS(sprite_index, guest) {
		if (guest->state == PEEP_STATE_WALKING)
			++count;
	}

	if (count > 0) {
		// Place staff at a random guest
		uint32 rand = scenario_rand_max(count);
		FOR_ALL_GUESTS(sprite_index, guest) {
			if (guest->state == PEEP_STATE_WALKING) {
				if (rand == 0)
					break;
				--rand;
			}
		}

		x = guest->x;
		y = guest->y;
		z = guest->z;
	} else {
		// No walking guests; pick random park entrance
		count = 0;
		uint8 i;
		for (i = 0; i < MAX_PARK_ENTRANCES; ++i) {
			if (gParkEntrances[i].x != SPRITE_LOCATION_NULL)
				++count;
		}

		if (count > 0) {
			uint32 rand = scenario_rand_max(count);
			for (i = 0; i < MAX_PARK_ENTRANCES; ++i) {
				if (gParkEntrances[i].x != SPRITE_LOCATION_NULL) {
					if (rand == 0)
						break;
					--rand;
				}
			}

			uint8 dir = gParkEntrances[i].direction;
			x = gParkEntrances[i].x;
			y = gParkEntrances[i].y;
			z = gParkEntrances[i].z;
			x += 16 + ((dir & 1) == 0 ? ((dir & 2) ? 32 : -32) : 0);
			y += 16 + ((dir & 1) == 1 ? ((dir & 2) ? -32 : 32) : 0);
		} else {
			// No more options; user must pick a location
			newPeep->state = PEEP_STATE_PICKED;
			x = newPeep->x;
			y = newPeep->y;
			z = newPeep->z;
		}
	}

	sprite_move(x, y, z + 16, (rct_sprite *)newPeep);
	invalidate_sprite_2((rct_sprite *)newPeep);
}

static money32 staff_hire_new_staff_member(uint8 staff_type, uint8 flags, sint16 command_x, sint16 command_y, sint16 command_z, sint32 autoposition, sint32 *newPeep_sprite_index)
{
	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_WAGES;
	gCommandPosition.x = command_x;
	gCommandPosition.y = command_y;
	gCommandPosition.z = command_z;

	if (gSpriteListCount[SPRITE_LIST_NULL] < 400) {
		gGameCommandErrorText = STR_TOO_MANY_PEOPLE_IN_GAME;
		return MONEY32_UNDEFINED;
	}

	// Staff type matches STAFF_TYPE enum, but ENTERTAINER onwards will match
	// the ENTERTAINER_COSTUME enum
	uint8 entertainerType = ENTERTAINER_COSTUME_PANDA;
	if (staff_type >= STAFF_TYPE_ENTERTAINER)
	{
		entertainerType = staff_type - STAFF_TYPE_ENTERTAINER;
		if (entertainerType >= ENTERTAINER_COSTUME_COUNT)
		{
			// Invalid entertainer costume
			return MONEY32_UNDEFINED;
		}

		uint32 availableCostumes = staff_get_available_entertainer_costumes();
		if (!(availableCostumes & (1 << entertainerType)))
		{
			// Entertainer costume unavailable
			return MONEY32_UNDEFINED;
		}

		staff_type = STAFF_TYPE_ENTERTAINER;
	}

	sint32 i;
	for (i = 0; i < STAFF_MAX_COUNT; ++i) {
		if (!(gStaffModes[i] & 1))
			break;
	}

	if (i == STAFF_MAX_COUNT) {
		gGameCommandErrorText = STR_TOO_MANY_STAFF_IN_GAME;
		return MONEY32_UNDEFINED;
	}

	if (flags & GAME_COMMAND_FLAG_APPLY)
	{
		sint32 newStaffId = i;
		const rct_sprite_bounds *spriteBounds;
		rct_peep *newPeep = &(create_sprite(flags)->peep);

		if (newPeep == NULL) {
			gGameCommandErrorText = STR_TOO_MANY_PEOPLE_IN_GAME;
			return MONEY32_UNDEFINED;
		}

		if (flags == 0) {
			sprite_remove((rct_sprite *)newPeep);
		} else {
			move_sprite_to_list((rct_sprite *)newPeep, SPRITE_LIST_PEEP * 2);

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

			if (staff_type == STAFF_TYPE_HANDYMAN)
				newPeep->staff_orders = STAFF_ORDERS_SWEEPING |
										STAFF_ORDERS_WATER_FLOWERS |
										STAFF_ORDERS_EMPTY_BINS;
			else if (staff_type == STAFF_TYPE_MECHANIC)
				newPeep->staff_orders = STAFF_ORDERS_INSPECT_RIDES |
										STAFF_ORDERS_FIX_RIDES;
			else
				newPeep->staff_orders = 0;

			uint16 idSearchSpriteIndex;
			rct_peep *idSearchPeep;

			// We search for the first available id for a given staff type
			uint32 newStaffIndex = 0;
			for (;;) {
				bool found = false;
				++newStaffIndex;

				FOR_ALL_STAFF(idSearchSpriteIndex, idSearchPeep) {
					if (idSearchPeep->staff_type != staff_type)
						continue;

					if (idSearchPeep->id == newStaffIndex) {
						found = true;
						break;
					}
				}

				if (!found)
					break;
			}

			newPeep->id = newStaffIndex;
			newPeep->staff_type = staff_type;

			static const rct_string_id staffNames[] = {
				STR_HANDYMAN_X,
				STR_MECHANIC_X,
				STR_SECURITY_GUARD_X,
				STR_ENTERTAINER_X,
			};

			/* rct2: 0x009929FC */
			static const uint8 spriteTypes[] = {
				PEEP_SPRITE_TYPE_HANDYMAN,
				PEEP_SPRITE_TYPE_MECHANIC,
				PEEP_SPRITE_TYPE_SECURITY,
				PEEP_SPRITE_TYPE_ENTERTAINER_PANDA,
			};

			uint8 sprite_type = spriteTypes[staff_type];
			if (staff_type == STAFF_TYPE_ENTERTAINER)
			{
				sprite_type = PEEP_SPRITE_TYPE_ENTERTAINER_PANDA + entertainerType;
			}
			newPeep->name_string_idx = staffNames[staff_type];
			newPeep->sprite_type = sprite_type;

			spriteBounds = g_peep_animation_entries[sprite_type].sprite_bounds;
			newPeep->sprite_width = spriteBounds->sprite_width;
			newPeep->sprite_height_negative = spriteBounds->sprite_height_negative;
			newPeep->sprite_height_positive = spriteBounds->sprite_height_positive;

			if (autoposition) {
				staff_autoposition_new_staff_member(newPeep);
			} else {
				newPeep->state = PEEP_STATE_PICKED;

				sprite_move(newPeep->x, newPeep->y, newPeep->z, (rct_sprite *)newPeep);
				invalidate_sprite_2((rct_sprite *)newPeep);
			}

			newPeep->time_in_park = gDateMonthsElapsed;
			newPeep->pathfind_goal.x = 0xFF;
			newPeep->pathfind_goal.y = 0xFF;
			newPeep->pathfind_goal.z = 0xFF;
			newPeep->pathfind_goal.direction = 0xFF;

			uint8 colour = staff_get_colour(staff_type);
			newPeep->tshirt_colour = colour;
			newPeep->trousers_colour = colour;

			// Staff energy determines their walking speed
			newPeep->energy = 0x60;
			newPeep->energy_growth_rate = 0x60;
			newPeep->var_E2 = 0;

			peep_update_name_sort(newPeep);

			newPeep->staff_id = newStaffId;

			gStaffModes[newStaffId] = STAFF_MODE_WALK;

			for (i = 0; i < 128; i++) {
				uint32 *addr = (uint32*)((uintptr_t)gStaffPatrolAreas + (newStaffId << 9) + i * 4);
				*addr = 0;
			}
		}

		if ((staff_type == STAFF_TYPE_HANDYMAN) && gConfigGeneral.handymen_mow_default) {
			uint8 new_orders = newPeep->staff_orders | STAFF_ORDERS_MOWING;
			game_do_command(newPeep->x, ((sint32)new_orders << 8) | 1, newPeep->y, newPeep->sprite_index, GAME_COMMAND_SET_STAFF_ORDER, 0, 0);
		}

		*newPeep_sprite_index = newPeep->sprite_index;
	}
	return 0;
}

/**
 *
 *  rct2: 0x006BEFA1
 */
void game_command_hire_new_staff_member(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
	*ebx = staff_hire_new_staff_member((*ebx & 0xFF00) >> 8,
									   *ebx & 0xFF,
									   *eax & 0xFFFF,
									   *ecx & 0xFFFF,
									   *edx & 0xFFFF,
									   (*ebx & 0xFF0000) >> 16,
									   edi);
}

void game_command_callback_hire_new_staff_member(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
	sint32 sprite_index = edi;
	if (sprite_index == SPRITE_INDEX_NULL)
	{
		rct_window *window = window_find_by_class(WC_STAFF_LIST);
		window_invalidate(window);
	}
	else
	{
		rct_peep *peep = &get_sprite(sprite_index)->peep;
		window_staff_open(peep);
	}
}

/** rct2: 0x00982134 */
static const bool peep_slow_walking_types[] = {
	false,	// PEEP_SPRITE_TYPE_NORMAL
	false,	// PEEP_SPRITE_TYPE_HANDYMAN
	false,	// PEEP_SPRITE_TYPE_MECHANIC
	false,	// PEEP_SPRITE_TYPE_SECURITY
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_PANDA
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_TIGER
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_ELEPHANT
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_ROMAN
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_GORILLA
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_SNOWMAN
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_KNIGHT
	true,	// PEEP_SPRITE_TYPE_ENTERTAINER_ASTRONAUT
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_BANDIT
	false,	// PEEP_SPRITE_TYPE_ENTERTAINER_SHERIFF
	true,	// PEEP_SPRITE_TYPE_ENTERTAINER_PIRATE
	true,	// PEEP_SPRITE_TYPE_BALLOON
};

/**
 *
 *  rct2: 0x006C0BB5
 */
void game_command_set_staff_order(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_WAGES;
	uint8 order_id = *ebx >> 8;
	uint16 sprite_id = *edx;
	if (sprite_id >= MAX_SPRITES)
	{
		log_warning("Invalid game command, sprite_id = %u", sprite_id);
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	if (*ebx & GAME_COMMAND_FLAG_APPLY) {
		rct_peep *peep = &get_sprite(sprite_id)->peep;
		if(order_id & 0x80){ // change costume
			uint8 sprite_type = order_id & ~0x80;
			sprite_type += 4;
			if (sprite_type >= countof(peep_slow_walking_types)) {
				log_error("Invalid change costume order for sprite_type %u", sprite_type);
				*ebx = MONEY32_UNDEFINED;
				return;
			}
			peep->sprite_type = sprite_type;
			peep->peep_flags &= ~PEEP_FLAGS_SLOW_WALK;
			if(peep_slow_walking_types[sprite_type]){
				peep->peep_flags |= PEEP_FLAGS_SLOW_WALK;
			}
			peep->action_frame = 0;
			peep_update_current_action_sprite_type(peep);
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
void game_command_set_staff_patrol(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
	if(*ebx & GAME_COMMAND_FLAG_APPLY){
		sint32 x = *eax;
		sint32 y = *ecx;
		uint16 sprite_id = *edx;
		if (sprite_id >= MAX_SPRITES)
		{
			*ebx = MONEY32_UNDEFINED;
			log_warning("Invalid sprite id %u", sprite_id);
			return;
		}
		rct_sprite *sprite = get_sprite(sprite_id);
		if (sprite->unknown.sprite_identifier != SPRITE_IDENTIFIER_PEEP || sprite->peep.type != PEEP_TYPE_STAFF)
		{
			*ebx = MONEY32_UNDEFINED;
			log_warning("Invalid type of sprite %u for game command", sprite_id);
			return;
		}
		rct_peep *peep = &sprite->peep;
		sint32 patrolOffset = peep->staff_id * (64 * 64 / 8);

		staff_toggle_patrol_area(peep->staff_id, x, y);

		sint32 ispatrolling = 0;
		for(sint32 i = 0; i < 128; i++){
			ispatrolling |= *((uint32*)((uintptr_t)gStaffPatrolAreas + patrolOffset + (i * 4)));
		}

		gStaffModes[peep->staff_id] &= ~2;
		if(ispatrolling){
			gStaffModes[peep->staff_id] |= 2;
		}

		for(sint32 y2 = 0; y2 < 4; y2++){
			for(sint32 x2 = 0; x2 < 4; x2++){
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
void game_command_fire_staff_member(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp)
{
	gCommandExpenditureType = RCT_EXPENDITURE_TYPE_WAGES;
	if(*ebx & GAME_COMMAND_FLAG_APPLY){
		window_close_by_class(WC_FIRE_PROMPT);
		uint16 sprite_id = *edx;
		if (sprite_id >= MAX_SPRITES)
		{
			log_warning("Invalid game command, sprite_id = %u", sprite_id);
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		rct_peep *peep = &get_sprite(sprite_id)->peep;
		if (peep->sprite_identifier != SPRITE_IDENTIFIER_PEEP || peep->type != PEEP_TYPE_STAFF)
		{
			log_warning("Invalid game command, peep->sprite_identifier = %u, peep->type = %u", peep->sprite_identifier, peep->type);
			*ebx = MONEY32_UNDEFINED;
			return;
		}
		if (peep->state == PEEP_STATE_PICKED) {
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
	gGameCommandErrorTitle = STR_CANT_HIRE_NEW_STAFF;

	sint32 command_x, ebx, command_y, command_z, esi, new_sprite_index, ebp;
	command_y = command_z = esi = new_sprite_index = ebp = 0;
	command_x = 0x8000;

	sint32 autoposition = gConfigGeneral.auto_staff_placement;
	if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z) {
		autoposition = autoposition ^ 1;
	}

	ebx = autoposition << 16 | staffType << 8 | GAME_COMMAND_FLAG_APPLY;

	game_command_callback = game_command_callback_hire_new_staff_member;
	sint32 result = game_do_command_p(GAME_COMMAND_HIRE_NEW_STAFF_MEMBER, &command_x, &ebx, &command_y, &command_z, &esi, &new_sprite_index, &ebp);

	if (result == MONEY32_UNDEFINED)
		return SPRITE_INDEX_NULL;

	return new_sprite_index;
}

/**
 *
 *  rct2: 0x006C0C3F
 */
void staff_update_greyed_patrol_areas()
{
	rct_peep* peep;

	for (sint32 staff_type = 0; staff_type < STAFF_TYPE_COUNT; ++staff_type)
	{
		uint32 *addr = (uint32*)((uintptr_t)gStaffPatrolAreas + ((staff_type + STAFF_MAX_COUNT) * 512));
		for (sint32 i = 0; i < 128; i++) {
			addr[i] = 0;
		}

		for (uint16 sprite_index = gSpriteListHead[SPRITE_LIST_PEEP]; sprite_index != SPRITE_INDEX_NULL; sprite_index = peep->next)
		{
			peep = GET_PEEP(sprite_index);

			if (peep->type == PEEP_TYPE_STAFF && staff_type == peep->staff_type)
			{
				uint32 *addr2 = (uint32*)((uintptr_t)gStaffPatrolAreas + (peep->staff_id * 512));
				for (sint32 i = 0; i < 128; i++) {
					addr[i] |= addr2[i];
				}
			}
		}
	}
}

static sint32 staff_is_location_in_patrol_area(rct_peep *peep, sint32 x, sint32 y)
{
	// Patrol quads are stored in a bit map (8 patrol quads per byte)
	// Each patrol quad is 4x4
	// Therefore there are in total 64 x 64 patrol quads in the 256 x 256 map
	return staff_is_patrol_area_set(peep->staff_id, x, y);
}

/**
 *
 *  rct2: 0x006C0905
 */
sint32 staff_is_location_in_patrol(rct_peep *staff, sint32 x, sint32 y)
{
	// Check if location is in the park
	if (!map_is_location_owned_or_has_rights(x, y))
		return 0;

	// Check if staff has patrol area
	if (!(gStaffModes[staff->staff_id] & 2))
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
		peep->time_in_park = gDateMonthsElapsed;
		peep->staff_lawns_mown = 0;
		peep->staff_rides_fixed = 0;
		peep->staff_gardens_watered = 0;
		peep->staff_rides_inspected = 0;
		peep->staff_litter_swept = 0;
		peep->staff_bins_emptied = 0;
	}
}

bool staff_is_patrol_area_set(sint32 staffIndex, sint32 x, sint32 y)
{
	x = (x & 0x1F80) >> 7;
	y = (y & 0x1F80) >> 1;

	sint32 peepOffset = staffIndex * 128;
	sint32 offset = (x | y) >> 5;
	sint32 bitIndex = (x | y) & 0x1F;
	return gStaffPatrolAreas[peepOffset + offset] & (((uint32)1) << bitIndex);
}

void staff_set_patrol_area(sint32 staffIndex, sint32 x, sint32 y, bool value)
{
	x = (x & 0x1F80) >> 7;
	y = (y & 0x1F80) >> 1;

	sint32 peepOffset = staffIndex * 128;
	sint32 offset = (x | y) >> 5;
	sint32 bitIndex = (x | y) & 0x1F;
	uint32 *addr = &gStaffPatrolAreas[peepOffset + offset];
	if (value) {
		*addr |= (1 << bitIndex);
	} else {
		*addr &= ~(1 << bitIndex);
	}
}

void staff_toggle_patrol_area(sint32 staffIndex, sint32 x, sint32 y)
{
	x = (x & 0x1F80) >> 7;
	y = (y & 0x1F80) >> 1;

	sint32 peepOffset = staffIndex * 128;
	sint32 offset = (x | y) >> 5;
	sint32 bitIndex = (x | y) & 0x1F;
	gStaffPatrolAreas[peepOffset + offset] ^= (1 << bitIndex);
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

	for (uint16 litterIndex = gSpriteListHead[SPRITE_LIST_LITTER]; litterIndex != 0xFFFF; litterIndex = litter->next){
		litter = &get_sprite(litterIndex)->litter;

		uint16 distance =
			abs(litter->x - peep->x) +
			abs(litter->y - peep->y) +
			abs(litter->z - peep->z) * 4;

		if (distance < nearestLitterDist){
			nearestLitterDist = distance;
			nearestLitter = litter;
		}
	}

	if (nearestLitterDist > 0x60){
		return 0xFF;
	}

	rct_xy16 litterTile = {
		.x = nearestLitter->x & 0xFFE0,
		.y = nearestLitter->y & 0xFFE0
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
		.x = (nearestLitter->x & 0xFFE0) - TileDirectionDelta[nextDirection].x,
		.y = (nearestLitter->y & 0xFFE0) - TileDirectionDelta[nextDirection].y
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

extern const uint8 byte_98D800[4];

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
			if ((mapElement->properties.surface.slope & MAP_ELEMENT_SLOPE_MASK) != byte_98D800[peep->next_var_29 & 0x3])
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

		if ((mapElement->properties.surface.grass_length & 0x7) < GRASS_LENGTH_CLEAR_1)
			continue;

		return chosenDirection;
	}
	return 0xFF;
}

/**
 *
 *  rct2: 0x006BFD9C
 */
static sint32 staff_handyman_direction_rand_surface(rct_peep* peep, uint8 validDirections) {
	uint8 direction = scenario_rand() & 3;
	for (sint32 i = 0; i < 4; ++i, ++direction) {
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
static sint32 staff_path_finding_handyman(rct_peep* peep)
{
	peep->var_E2++;

	uint8 litterDirection = 0xFF;
	uint8 validDirections = staff_get_valid_patrol_directions(peep, peep->next_x, peep->next_y);

	if ((peep->staff_orders & STAFF_ORDERS_SWEEPING) &&
		((gCurrentTicks + peep->sprite_index) & 0xFFF) > 110) {
		litterDirection = staff_handyman_direction_to_nearest_litter(peep);
	}

	uint8 direction = 0xFF;
	if (litterDirection == 0xFF &&
		(peep->staff_orders & STAFF_ORDERS_MOWING) &&
		peep->var_E2 >= 12
	) {
		direction = staff_handyman_direction_to_uncut_grass(peep, validDirections);
	}

	if (direction == 0xFF) {
		if (peep->next_var_29 & 0x18) {
			direction = staff_handyman_direction_rand_surface(peep, validDirections);
		} else {
			rct_map_element* mapElement = map_get_path_element_at(
				peep->next_x / 32,
				peep->next_y / 32,
				peep->next_z);

			if (mapElement == NULL)
				return 1;

			uint8 pathDirections = (mapElement->properties.path.edges & validDirections) & 0xF;
			if (pathDirections == 0) {
				direction = staff_handyman_direction_rand_surface(peep, validDirections);
			} else {
				bool chooseRandom = true;
				if (litterDirection != 0xFF && pathDirections & (1 << litterDirection)) {
					if ((scenario_rand() & 0xFFFF) >= 0x1999) {
						chooseRandom = false;
						direction = litterDirection;
					}
				} else {
					pathDirections &= ~(1 << (peep->direction ^ (1 << 1)));
					if (pathDirections == 0) {
						pathDirections |= 1 << (peep->direction ^ (1 << 1));
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

	peep->direction = direction;
	peep->destination_x = chosenTile.x + 16;
	peep->destination_y = chosenTile.y + 16;
	peep->destination_tolerence = 3;
	if (peep->state == PEEP_STATE_QUEUING) {
		peep->destination_tolerence = (scenario_rand() & 7) + 2;
	}
	return 0;
}

static uint8 staff_direction_surface(rct_peep* peep, uint8 initialDirection) {
	uint8 direction = initialDirection;
	for (sint32 i = 0; i < 3; ++i) {
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
		if (pathDirections & (1 << peep->direction))
			return peep->direction;
	}

	// Modified from original to spam scenario_rand less
	uint8 direction = scenario_rand() & 3;
	for (sint32 i = 0; i < 4; ++i, ++direction) {
		direction &= 3;
		if (pathDirections & (1 << direction))
			return direction;
	}
	// This will never happen as pathDirections always has a bit set.
	return peep->direction;
}

/**
 *
 *  rct2: 0x006C0121
 */
static uint8 staff_mechanic_direction_path(rct_peep* peep, uint8 validDirections, rct_map_element* pathElement) {

	uint8 direction = 0xFF;
	uint8 pathDirections = pathElement->properties.path.edges & 0xF;
	pathDirections &= validDirections;

	if (pathDirections == 0) {
		return staff_mechanic_direction_surface(peep);
	}

	// Check if this is dead end - i.e. only way out is the reverse direction.
	pathDirections &= ~(1 << (peep->direction ^ (1 << 1)));
	if (pathDirections == 0) {
		pathDirections |= (1 << (peep->direction ^ (1 << 1)));
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

	// Mechanic is heading to ride (either broken down or for inspection).
	if (peep->state == PEEP_STATE_ANSWERING || peep->state == PEEP_STATE_HEADING_TO_INSPECTION) {
		rct_ride* ride = get_ride(peep->current_ride);
		uint8 z = ride->station_heights[peep->current_ride_station];
		gPeepPathFindGoalPosition.z = z;

		/* Find location of the exit for the target ride station
		 * or if the ride has no exit, the entrance. */
		uint16 location = ride->exits[peep->current_ride_station];
		if (location == 0xFFFF) {
			location = ride->entrances[peep->current_ride_station];
		}

		rct_xy16 chosenTile = {
			.x = (location & 0xFF) * 32,
			.y = (location >> 8) * 32
		};

		gPeepPathFindGoalPosition.x = chosenTile.x;
		gPeepPathFindGoalPosition.y = chosenTile.y;

		// Find the exit/entrance map_element
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

		gPeepPathFindIgnoreForeignQueues = false;
		gPeepPathFindQueueRideIndex = 255;

		#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
		pathfind_logging_enable(peep);
		#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

		sint32 pathfindDirection = peep_pathfind_choose_direction(peep->next_x, peep->next_y, peep->next_z, peep);

		#if defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1
		pathfind_logging_disable();
		#endif // defined(DEBUG_LEVEL_1) && DEBUG_LEVEL_1

		if (pathfindDirection == -1) {
			/* Heuristic search failed for all directions.
			 * Reset the pathfind_goal - this means that the pathfind_history
			 * will be reset in the next call to peep_pathfind_choose_direction().
			 * This lets the heuristic search "try again" in case the player has
			 * edited the path layout or the mechanic was already stuck in the
			 * save game (e.g. with a worse version of the pathfinding). */
			peep_reset_pathfind_goal(peep);
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
static sint32 staff_path_finding_mechanic(rct_peep* peep) {
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

	peep->direction = direction;
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

	pathDirections &= ~(1 << (peep->direction ^ (1 << 1)));
	if (pathDirections == 0) {
		pathDirections |= (1 << (peep->direction ^ (1 << 1)));
	}

	direction = bitscanforward(pathDirections);
	pathDirections &= ~(1 << direction);
	if (pathDirections == 0) {
		return direction;
	}

	pathDirections |= (1 << direction);

	direction = scenario_rand() & 3;
	for (sint32 i = 0; i < 4; ++i, ++direction) {
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
static sint32 staff_path_finding_misc(rct_peep* peep) {
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

	peep->direction = direction;
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
			if(peep->time_in_queue > 200) {
				peep->time_in_queue -= 200;
			}
			else {
				peep->time_in_queue = 0;
			}
			peep->happiness_growth_rate = min(peep->happiness_growth_rate + 3, 255);
		}
	}
}

/**
 *
 *  rct2: 0x006C05AE
 */
static sint32 staff_path_finding_entertainer(rct_peep* peep) {

	if (((scenario_rand() & 0xFFFF) <= 0x4000) &&
		(peep->action == PEEP_ACTION_NONE_1 || peep->action == PEEP_ACTION_NONE_2)) {

		invalidate_sprite_2((rct_sprite*)peep);

		peep->action = (scenario_rand() & 1) ? PEEP_ACTION_WAVE_2 : PEEP_ACTION_JOY;
		peep->action_frame = 0;
		peep->action_sprite_image_offset = 0;

		peep_update_current_action_sprite_type(peep);
		invalidate_sprite_2((rct_sprite*)peep);
		staff_entertainer_update_nearby_peeps(peep);
	}

	return staff_path_finding_misc(peep);
}

/**
 *
 *  rct2: 0x006BF926
 */
sint32 staff_path_finding(rct_peep* peep) {
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

void game_command_set_staff_name(sint32 *eax, sint32 *ebx, sint32 *ecx, sint32 *edx, sint32 *esi, sint32 *edi, sint32 *ebp) {
	uint16 sprite_index = *ecx & 0xFFFF;

	if (sprite_index >= MAX_SPRITES) {
		*ebx = MONEY32_UNDEFINED;
		return;
	}
	rct_peep *peep = GET_PEEP(sprite_index);
	if (peep->type != PEEP_TYPE_STAFF) {
		*ebx = MONEY32_UNDEFINED;
		return;
	}

	*ebx = set_peep_name(
		*ebx & 0xFF,
		*eax & 0xFFFF,
		sprite_index,
		(uint8*)edx,
		(uint8*)ebp,
		(uint8*)edi
	);
}

void game_command_pickup_staff(sint32* eax, sint32* ebx, sint32* ecx, sint32* edx, sint32* esi, sint32* edi, sint32* ebp)
{
	sint32 peepnum = *eax;
	sint32 x = *edi;
	sint32 y = *ebp;
	sint32 z = *edx;
	sint32 action = *ecx;
	if (peep_pickup_command(peepnum, x, y, z, action, *ebx & GAME_COMMAND_FLAG_APPLY)) {
		*ebx = 0;
	}
	else
	{
		*ebx = MONEY32_UNDEFINED;
	}
}

colour_t staff_get_colour(uint8 staffType)
{
	switch (staffType) {
	case STAFF_TYPE_HANDYMAN:		return gStaffHandymanColour;
	case STAFF_TYPE_MECHANIC:		return gStaffMechanicColour;
	case STAFF_TYPE_SECURITY:		return gStaffSecurityColour;
	case STAFF_TYPE_ENTERTAINER:	return 0;
	default:
		assert(false);
		return 0;
	}
}

bool staff_set_colour(uint8 staffType, colour_t value)
{
	switch (staffType) {
	case STAFF_TYPE_HANDYMAN:
		gStaffHandymanColour = value;
		break;
	case STAFF_TYPE_MECHANIC:
		gStaffMechanicColour = value;
		break;
	case STAFF_TYPE_SECURITY:
		gStaffSecurityColour = value;
		break;
	default:
		return false;
	}
	return true;
}

uint32 staff_get_available_entertainer_costumes()
{
	init_scenery();

	uint32 entertainerCostumes = 0;
	for (sint32 i = 0; i < 19; i++) {
		if (window_scenery_tab_entries[i][0] != -1) {
			rct_scenery_set_entry* scenery_entry = get_scenery_group_entry(i);
			entertainerCostumes |= scenery_entry->entertainer_costumes;
		}
	}

	// For some reason the flags are +4 from the actual costume IDs
	entertainerCostumes >>= 4;

	return entertainerCostumes;
}

sint32 staff_get_available_entertainer_costume_list(uint8 * costumeList)
{
	uint32 availableCostumes = staff_get_available_entertainer_costumes();
	sint32 numCostumes = 0;
	for (uint8 i = 0; i < ENTERTAINER_COSTUME_COUNT; i++) {
		if (availableCostumes & (1 << i)) {
			costumeList[numCostumes++] = i;
		}
	}
	return numCostumes;
}
