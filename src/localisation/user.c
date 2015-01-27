/*****************************************************************************
 * Copyright (c) 2014 Ted John
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
#include "localisation.h"

char *gUserStrings = (char*)0x0135A8F4;

/**
 *
 *  rct2: 0x006C4209
 */
void user_string_clear_all()
{
	memset(gUserStrings, 0, MAX_USER_STRINGS * USER_STRING_MAX_LENGTH);
}

/**
 * 
 *  rct2: 0x006C421D
 */
rct_string_id user_string_allocate(int base, const char *text)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;

	ecx = base;
	edi = (int)text;
	RCT2_CALLFUNC_X(0x006C421D, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	
	return eax & 0xFFFF;
}

/**
 * 
 *  rct2: 0x006C42AC
 */
void user_string_free(rct_string_id id)
{
	if (id < 0x8000 || id >= 0x9000)
		return;

	id %= MAX_USER_STRINGS;
	gUserStrings[id * USER_STRING_MAX_LENGTH] = 0;
}