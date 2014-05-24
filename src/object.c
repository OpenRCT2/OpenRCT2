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
 *  rct2: 0x006A985D
 */
int object_load(int groupIndex, rct_object_entry *entry)
{
	RCT2_CALLPROC_X(0x006A985D, 0, 0, groupIndex, 0, 0, 0, (int)entry);
	#ifdef _MSC_VER
	__asm jb fail
	#else
	__asm__ goto ( "jb %l0" : : : : fail );
	#endif
	return 1;
fail:
	return 0;
}

/**
 * 
 *  rct2: 0x006A9CAF
 */
void object_unload(int groupIndex, rct_object_entry_extended *entry)
{
	RCT2_CALLPROC_X(0x006A9CAF, 0, groupIndex, 0, 0, 0, 0, (int)entry);
}