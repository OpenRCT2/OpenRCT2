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

#include "addresses.h"
#include "object.h"

/**
 * 
 *  rct2: 0x006A8B40
 */
void object_load_list()
{
	RCT2_CALLPROC_EBPSAFE(0x006A8B40);
}

/**
 * 
 *  rct2: 0x006AA0C6
 */
void object_read_and_load_entries(HFILE hFile)
{
	RCT2_CALLPROC_EBPSAFE(0x006AA0C6);

	// int i;
	// rct_object_entry *entries;
	// entries = malloc(721 * sizeof(rct_object_entry));
	// sawyercoding_read_chunk(hFile, entries);
	// for (i = 0; i < 721; i++) {
	// 	RCT2_CALLPROC_X(0x006A985D, 0, 0, i, 0, 0, 0, 0);
	// }
	// free(entries);
}

/**
 * 
 *  rct2: 0x006AA2B7
 */
int object_load_packed()
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	RCT2_CALLFUNC_X(0x006AA2B7, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	return eax;
}