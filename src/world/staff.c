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
#include "peep.h"
#include "sprite.h"
#include "staff.h"

/**
*
*  rct2: 0x00669E55
*/
void game_command_update_staff_colour()
{
	uint8 staff_type, colour, _bl;
	int spriteIndex;
	rct_peep *peep;
	
	#ifdef _MSC_VER
	__asm mov _bl, bl
	#else
	__asm__("mov %[_bl], bl " : [_bl] "+m" (_bl));
	#endif

	#ifdef _MSC_VER
	__asm mov staff_type, bh
	#else
	__asm__("mov %[staff_type], bh " : [staff_type] "+m" (staff_type));
	#endif

	#ifdef _MSC_VER
	__asm mov colour, dh
	#else
	__asm__("mov %[colour], bh " : [colour] "+m" (colour));
	#endif

	if (_bl & 1) {
		RCT2_ADDRESS(RCT2_ADDRESS_HANDYMAN_COLOUR, uint8)[staff_type] = colour;

		FOR_ALL_PEEPS(spriteIndex, peep) {
			if (peep->type == PEEP_TYPE_STAFF && peep->staff_type == staff_type) {
				peep->tshirt_colour = colour;
				peep->trousers_colour = colour;
			}
		}
	}

	gfx_invalidate_screen();
	
	#ifdef _MSC_VER
	__asm mov ebx, 0
	#else
	__asm__("mov ebx, 0 ");
	#endif
}

/**
*
*  rct2: 0x006BEFA1
*/
void game_command_hire_new_staff_member(int* eax, int* ebx, int* ecx, int* edx,
	int* esi, int* edi, int* ebp)
{
	uint8 _bl = *ebx & 0xFF, staff_type = (*ebx & 0xFF00) >> 8;
	uint16 _ax = *eax & 0xFFFF, _cx = *ecx & 0xFFFF, _dx = *edx & 0xFFFF;

	RCT2_GLOBAL(0x0141F56C, uint8) = 0x28;
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
		RCT2_CALLPROC_X(0x0069EDB6, 0, 0, _ecx, 0, (int)newPeep, 0, 0);
	}
	else {
		move_sprite_to_list((rct_sprite *)newPeep, SPRITE_LINKEDLIST_OFFSET_PEEP);

		newPeep->sprite_identifier = 1;
		newPeep->var_09 = 0x0F;
		newPeep->var_15 = 5;
		newPeep->var_14 = 8;
		newPeep->sprite_direction = 0;

		RCT2_CALLPROC_X(0x0069E9D3, _ax, 0, *ecx, _dx, (int)newPeep, 0, 0);

		newPeep->state = PEEP_STATE_PICKED;
		if (newPeep->x != -32768) {
			newPeep->state = 0;
		}

		newPeep->var_45 = 0;
		newPeep->var_71 = 0xFF;
		newPeep->var_6D = 0;
		newPeep->var_70 = 0;
		newPeep->var_E0 = 0;
		newPeep->var_6E = 0;
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
		newPeep->var_14 = *((uint8*)_edx);
		newPeep->var_09 = *((uint8*)(_edx + 1));
		newPeep->var_15 = *((uint8*)(_edx + 2));

		RCT2_CALLPROC_X(0x0069E9D3, newPeep->x, 0, newPeep->y, newPeep->z, (int)newPeep, 0, 0);
		RCT2_CALLPROC_X(0x006EC473, *eax, 0, 0, 0, (int)newPeep, 0, 0);

		newPeep->var_AD = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint8);
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
void update_staff_colour(uint8 staff_type, uint16 colour)
{
	game_do_command(
		0,
		(staff_type << 8) + 1,
		0,
		(colour << 8) + 4,
		GAME_COMMAND_SET_STAFF_COLOUR,
		0,
		0);
}

/*
 * Hires a new staff member of the given type. If the hire cannot be completed (eg. the maximum
 * number of staff is reached or there are too many people in the game) it returns 0xFFFF.
*/
uint16 hire_new_staff_member(uint8 staff_type)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_STRING_ID, uint16) = STR_CANT_HIRE_NEW_STAFF;

	int eax, ebx, ecx, edx, esi, edi, ebp;
	eax = 0x8000;
	ebx = staff_type << 8 | 1;

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
